#pragma once
#include "GraphicsBuffer.h"
#include "HelperMacros.h"
#include "MathObjects.h"
#include <vector>
#include <string>
#include "Image.h"
#include "Texture.h"
#include "Camera.h"

class GraphicsAPI;
class Camera;

template <typename T>
using Vector = std::vector<T>;

struct MODEL_VERTEX
{
	Vector3 position;
	Vector3 color;
	float u = 0.0; 
	float v = 0.0;
};

struct float2 {
	float u; float v;
	bool operator==(const float2& rhs) const 
	{ 
		return u == rhs.u && v == rhs.v; 
	}
	bool operator!=(const float2& rhs) const 
	{ 
		return !(*this == rhs); 
	}
};

struct FaceVertex {
	int vertex_index;
	int uv_index;

	bool operator==(const FaceVertex& vertex) const 
	{
		return vertex_index == vertex.vertex_index &&
			uv_index == vertex.uv_index;
	}
};

namespace std {
	template<>
	struct hash<FaceVertex>
	{
		size_t operator()(const FaceVertex& vertex) const 
{
			return	hash<int>()(vertex.vertex_index) ^
					hash<int>()(vertex.uv_index);
		}
	};
}

struct Mesh {

	int topology;

	int baseVertex;
	int numVertices;

	int baseIndex;
	int numIndices;

};

//TODO: tener un vertexbuffer y un index buffer por modelo.

/*Un modelo tiene un vertex buffer y un index buffer*/
class Model
{
public:
	Model() = default;
	
	/*Model(UPtr<GraphicsAPI>& pGraphicsAPI,
				const char* modelPath, 
				const char* texturePath,
				MatrixCollection& WVP,
				UPtr<GraphicsBuffer>& pGraphicsBuffer,
				UPtr<Camera>& pCamera)
				: m_pGraphicsAPI(pGraphicsAPI),
					m_WVP(WVP),
					m_pCB(pGraphicsBuffer),
					m_pCamera(pCamera)
	{


	}*/

	~Model() = default;

	bool LoadFromFile(const char* filePath, 
										const UPtr<GraphicsAPI>& pGraphicsAPI);


	/*void Init();
	void Update();
	void Render();*/

	Vector<Mesh> m_meshes;
	

	//Cuántos buffers voy a querer generar
	UPtr<GraphicsBuffer> m_pVertexBuffer;
	UPtr<GraphicsBuffer> m_pIndexBuffer;

	/*UPtr<Image> m_pSrcImage;
	UPtr<Texture> m_pTexture;
	const char* m_modelPath;
	const char* m_texturePath;

	UPtr<GraphicsAPI>& m_pGraphicsAPI;
	MatrixCollection& m_WVP;
	UPtr<GraphicsBuffer>& m_pCB;
	UPtr<Camera>& m_pCamera;*/
	
};

