#include "Model.h"
#include <cassert>
#include <fstream>
#include "GraphicsAPI.h"
#include <iostream>
#include <assimp/DefaultLogger.hpp>

using std::fstream;
using std::ios;

bool Model::LoadFromFile(const char* filePath)
{

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

    //TODO: Ignorar comentarios

    

    Vector<String> lines = split(fileData, '\n');

    Vector<Vector3> temp_pos;
    Vector<float2> temp_tc;
    Vector<Vector3> temp_norm;

    UnorderedMap<FaceVertex, uint32> uniqueVertices;

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
        else if (tokens[0] == "vn")
        {
          Vector3 norm;
          norm.x = std::stof(tokens[1]);
          norm.y = std::stof(tokens[2]);
          norm.z = std::stof(tokens[3]);
          temp_norm.push_back(norm);

        }
        else if (tokens[0] == "f")
        {
            Vector<uint32> faceIndex;

            //assert(tokens.size() == 4);
            for (size_t i = 1; i < tokens.size(); ++i)
            {
                Vector<String> fi = split(tokens[i], '/');

                FaceVertex fv;
                fv.vertex_index = std::stoi(fi[0]) - 1;
                fv.uv_index = std::stoi(fi[1]) - 1;
                fv.normal_index = std::stoi(fi[2]) - 1;

                if (uniqueVertices.find(fv) == uniqueVertices.end())
                {
                    uniqueVertices[fv] = static_cast<uint32>(m_vertices.size());

                    MODEL_VERTEX mvertex;
                    mvertex.position = temp_pos[fv.vertex_index];
                    mvertex.color = Vector3(1.0f, 1.0f, 1.0f);
                    mvertex.normal = temp_norm[fv.normal_index];
                    mvertex.u = temp_tc[fv.uv_index].u;
                    mvertex.v = 1.0 - temp_tc[fv.uv_index].v;
                    m_vertices.push_back(mvertex);
                }

                faceIndex.push_back(uniqueVertices[fv]);
            }

            m_indices.push_back(faceIndex[0]);
            m_indices.push_back(faceIndex[1]);
            m_indices.push_back(faceIndex[2]);
            //indices.push_back(faceIndex[3]);
        }
    }

    m_meshes.resize(1);
    auto& mesh = m_meshes[0];
    mesh.baseVertex = 0;
    mesh.numVertices = m_vertices.size();
    mesh.baseIndex = 0;
    mesh.numIndices = m_indices.size();
    mesh.topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
    //mesh

    ComputeTangentSpace();

    //Create graphics buffers
    Vector<char> vertex_data;
    vertex_data.resize(m_vertices.size() * sizeof(MODEL_VERTEX));
    memcpy(vertex_data.data(), m_vertices.data(), m_vertices.size() * sizeof(MODEL_VERTEX));

    m_pVertexBuffer = g_graphicsAPI().CreateVertexBuffer(vertex_data);
    if (!m_pVertexBuffer)
    {
        return false;
    }

    Vector<char> index_data;
    index_data.resize(m_indices.size() * sizeof(uint32));
    memcpy(index_data.data(), m_indices.data(), m_indices.size() * sizeof(uint32));

    m_pIndexBuffer = g_graphicsAPI().CreateIndexBuffer(index_data);
    if (!m_pIndexBuffer)
    {
        return false;
    }


    return true;
    
}

void Model::ComputeTangentSpace()
{
  Vector<MODEL_VERTEX>& vertices  = m_vertices;
  Vector<uint32>& indices = m_indices;

  //Compute tangents
  Vector<Vector3> tan1(vertices.size(), Vector3(0.0f, 0.0f, 0.0f));
  Vector<Vector3> tan2(vertices.size(), Vector3(0.0f, 0.0f, 0.0f));


  for(auto& mesh : m_meshes)
  {
    if (mesh.topology != D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST)
    {
      continue;
    }

    int numTriangles = mesh.numIndices / 3;

    for (int t = 0; t < numTriangles; ++t)
    {
      int i = mesh.baseIndex + (t * 3);
      unsigned int i1 = indices[i];
      unsigned int i2 = indices[i + 1];
      unsigned int i3 = indices[i + 2];

      const Vector3& v1 = vertices[i1].position;
      const Vector3& v2 = vertices[i2].position;
      const Vector3& v3 = vertices[i3].position;
    
      const Vector2& w1 = Vector2(vertices[i1].u, vertices[i1].v);
      const Vector2& w2 = Vector2(vertices[i2].u, vertices[i2].v);
      const Vector2& w3 = Vector2(vertices[i3].u, vertices[i3].v);

      float x1 = v2.x - v1.x;
      float x2 = v3.x - v1.x;
      float y1 = v2.y - v1.y;
      float y2 = v3.y - v1.y;
      float z1 = v2.z - v1.z;
      float z2 = v3.z - v1.z;

      float s1 = w2.x - w1.x;
      float s2 = w3.x - w1.x;
      float t1 = w2.y - w1.y;
      float t2 = w3.y - w1.y;

      float denominator = s1 * t2 - s2 * t1;
      if (denominator == 0.0f)
      {
        continue;
      }

      float r = 1.0f / denominator;

      Vector3 sdir = Vector3((t2 * x1 - t1 * x2) * r,
                             (t2 * y1 - t1 * y2) * r,
                             (t2 * z1 - t1 * z2) * r);
      
      Vector3 tdir = Vector3((s1 * x2 - s2 * x1) * r,
                             (s1 * y2 - s2 * y1) * r,
                             (s1 * z2 - s2 * z1) * r);
      
      tan1[i1] += sdir;
      tan1[i2] += sdir;
      tan1[i3] += sdir;

      tan2[i1] += tdir;
      tan2[i2] += tdir;
      tan2[i3] += tdir;
    }
    for (int i = mesh.baseVertex; i < mesh.baseVertex + mesh.numVertices; ++i)
    {
      const Vector3& n = vertices[i].normal;
      if (std::abs(n.y) > 0.999f)
      {//Esta en un polo
        Vector3 helper = (n.dot(Vector3(0.0f, 1.0f, 0.0f))) < 0.999f ? 
          Vector3(1.0f,0.0f,0.0f) :
          Vector3(0.0f,0.0f,1.0f);
        vertices[i].tangent = helper.cross(n);
        vertices[i].tangent.normalize();
        continue;
      }
      
      //GraM-Schmidt orthogonalize
      const Vector3& t = tan1[i];
      auto tmpTang = t - n * n.dot(t);
      
      if (tmpTang == Vector3(0.0f,0.0f,0.0f))
      {
        Vector3 helper = (n.z) < 0.999f ?
          Vector3(0.0f, 0.0f, 1.0f) :
          Vector3(1.0f, 0.0f, 0.0f);
        vertices[i].tangent = helper.cross(n);
        vertices[i].tangent.normalize();
        continue;
      }

      vertices[i].tangent = tmpTang;
      vertices[i].tangent.normalize();

    }
  }
}

bool Model::LoadWithAssimp(const char* filePath)
{
  /*Assimp::DefaultLogger::create("asdfasdf.txt", Assimp::Logger::VERBOSE);*/
  Assimp::Importer importer;

  uint32 flags =       aiProcess_CalcTangentSpace |
                       aiProcess_Triangulate |
                       aiProcess_JoinIdenticalVertices |
                       aiProcess_SortByPType |
                       aiProcess_ConvertToLeftHanded;

  const aiScene* scene = importer.ReadFile(filePath, flags);

  if (!scene)
  {
    //Assimp::DefaultLogger::get()->info("This is a messahe");
    MessageBox(nullptr, L"Couldn't load model with assimp", L"Error", MB_OK);
    //Assimp::DefaultLogger::kill;
    return false;
  }

  
  int accumVertex = 0;
  int accumIndex = 0;

  //MESHES
  for (size_t i = 0; i < scene->mNumMeshes; ++i)
  { 
    //VERTICES
    for (size_t j = 0; j < scene->mMeshes[i]->mNumVertices; ++j)
    { 
      MODEL_VERTEX modelVertex;
      memset(&modelVertex, 0, sizeof(MODEL_VERTEX));

      modelVertex.position.x = scene->mMeshes[i]->mVertices[j].x;
      modelVertex.position.y = scene->mMeshes[i]->mVertices[j].y;
      modelVertex.position.z = scene->mMeshes[i]->mVertices[j].z;

      modelVertex.color.x = 1.0f;
      modelVertex.color.y = 1.0f;
      modelVertex.color.z = 1.0f;

      modelVertex.normal.x = scene->mMeshes[i]->mNormals[j].x;
      modelVertex.normal.y = scene->mMeshes[i]->mNormals[j].y;
      modelVertex.normal.z = scene->mMeshes[i]->mNormals[j].z;

      modelVertex.tangent.x = scene->mMeshes[i]->mTangents[j].x;
      modelVertex.tangent.y = scene->mMeshes[i]->mTangents[j].y;
      modelVertex.tangent.z = scene->mMeshes[i]->mTangents[j].z;

      modelVertex.u = static_cast<float>(scene->mMeshes[i]->mTextureCoords[0][j].x);
      modelVertex.v = static_cast<float>(scene->mMeshes[i]->mTextureCoords[0][j].y);

      m_vertices.push_back(modelVertex);
    }

    //ÍNDICES
    for (size_t j = 0; j < scene->mMeshes[i]->mNumFaces; ++j)
    {

      //3 índices por que la cara es de triángulos.
      m_indices.push_back(scene->mMeshes[i]->mFaces[j].mIndices[0]);
      m_indices.push_back(scene->mMeshes[i]->mFaces[j].mIndices[1]);
      m_indices.push_back(scene->mMeshes[i]->mFaces[j].mIndices[2]);
    } 

    Mesh currentMesh;
    currentMesh.topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
    currentMesh.numVertices = scene->mMeshes[i]->mNumVertices;
    currentMesh.baseVertex = accumVertex;
    currentMesh.baseIndex = accumIndex;
    currentMesh.numIndices = scene->mMeshes[i]->mNumFaces * 3;

    accumVertex += scene->mMeshes[i]->mNumVertices;

    accumIndex += scene->mMeshes[i]->mNumFaces * 3;

    m_meshes.push_back(currentMesh);

  }
  
  Vector<char> vectorData;
  vectorData.resize(m_vertices.size() * sizeof(MODEL_VERTEX));
  memcpy(vectorData.data(), m_vertices.data(), m_vertices.size() * sizeof(MODEL_VERTEX));

  m_pVertexBuffer = g_graphicsAPI().CreateVertexBuffer(vectorData);
  if (!m_pVertexBuffer)
  {
    MessageBox(nullptr, L"Couldn't create the vertex buffer with assimp", L"Error", MB_OK);
    return false;
  }

  Vector<char> indexData;
  indexData.resize(m_indices.size() * sizeof(uint32));
  memcpy(indexData.data(),m_indices.data(),m_indices.size() * sizeof(uint32));

  m_pIndexBuffer = g_graphicsAPI().CreateIndexBuffer(indexData);
  if (!m_pIndexBuffer)
  {
    MessageBox(nullptr, L"Couldn't create the index buffer with assimp", L"Error", MB_OK);
    return false;
  }
  
  //__debugbreak();
  //Assimp::DefaultLogger::kill;

  return true;
}





