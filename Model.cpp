#include "Model.h"
#include <cassert>
#include <fstream>

#include "GraphicsAPI.h"

using std::fstream;
using std::ios;

bool Model::LoadFromFile(const char* filePath, WPtr<GraphicsAPI> pGraphicsAPI)
{
    auto GAPI = pGraphicsAPI.lock();

    fstream objFile(filePath, ios::in | ios::ate);
    
    if (!objFile.is_open())
    {
        return false;
    }

    auto fileSize = objFile.tellg();
    objFile.seekg(ios::beg);

    String fileData;
    fileData.resize(fileSize);
    objFile.read(&fileData[0],fileSize);

    Vector<String> lines = split(fileData, '\n');
    Vector<MODEL_VERTEX> vertices;
    Vector<unsigned short> indices;

    Vector<Vector3> temp_pos;
    Vector<float2> temp_tc;
    UnorderedMap<FaceVertex, unsigned short> uniqueVertices;

    for (const auto& line: lines)
    {
        Vector<String> tokens = split(line, ' '); 
        if (tokens.empty())
        {
            continue;
        }
        else if (tokens[0] == "v")
        {
            Vector3 pos;
            pos.x = std::stof(tokens[1]);
            pos.y = std::stof(tokens[2]);
            pos.z = std::stof(tokens[3]);
            temp_pos.push_back(pos);
        }
        else if (tokens[0] == "vt")
        {
            float2 uv;
            uv.u = std::stof(tokens[1]);
            uv.v = std::stof(tokens[2]);
            temp_tc.push_back(uv);
        }
        else if (tokens[0] == "f")
        {
            Vector<unsigned short> faceIndex;

            //assert(tokens.size() == 4);
            for (size_t i = 1; i < tokens.size(); ++i)
            {
                Vector<String> fi = split(tokens[i], '/');

                FaceVertex fv;
                fv.vertex_index = std::stoi(fi[0]) - 1;
                fv.uv_index = std::stoi(fi[1]) - 1;    

                if (uniqueVertices.find(fv) == uniqueVertices.end())
                {
                    uniqueVertices[fv] = static_cast<unsigned short>(vertices.size()); 

                    MODEL_VERTEX mvertex;
                    mvertex.position = temp_pos[fv.vertex_index];
                    mvertex.color = Vector3(1.0f, 1.0f, 1.0f);
                    mvertex.u = temp_tc[fv.uv_index].u;
                    mvertex.v = 1.0 - temp_tc[fv.uv_index].v;
                    vertices.push_back(mvertex);
                }

                faceIndex.push_back(uniqueVertices[fv]);
            }

            indices.push_back(faceIndex[0]);
            indices.push_back(faceIndex[1]);
            indices.push_back(faceIndex[2]);
        }
    }

    m_meshes.resize(1);
    auto& mesh = m_meshes[0];
    mesh.baseVertex = 0;
    mesh.numVertices = vertices.size();
    mesh.baseIndex = 0;
    mesh.numIndices = indices.size();
    mesh.topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
    //mesh

    //Create graphics buffers
    Vector<char> vertex_data;
    vertex_data.resize(vertices.size() * sizeof(MODEL_VERTEX));
    memcpy(vertex_data.data(), vertices.data(), vertices.size() * sizeof(MODEL_VERTEX));

    m_pVertexBuffer = GAPI->CreateVertexBuffer(vertex_data);
    if (!m_pVertexBuffer)
    {
        return false;
    }

    Vector<char> index_data;
    index_data.resize(indices.size() * sizeof(unsigned short));
    memcpy(index_data.data(), indices.data(), indices.size() * sizeof(unsigned short));

    m_pIndexBuffer = GAPI->CreateIndexBuffer(index_data);
    if (!m_pIndexBuffer)
    {
        return false;
    }


    return true;
    
}
