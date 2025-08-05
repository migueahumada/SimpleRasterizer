#pragma once

#include "HelperMacros.h"
#include "MathObjects.h"
#include <vector>
#include <string>

class GraphicsAPI;
class GraphicsBuffer;

template <typename T>
using Vector = std::vector<T>;

// Mandar bloques con alineación de 16 bytes
//LA GPU funciona con registro de 128 bits de 16 bytes
// Es mejor para uso de memoria cuando las cosas 
// están en bloques de 4.

// uN INTRÍNSECO E S UNA OPERACIÓN VECTORIAL DONDE 
// LLENAS 4 FLOTANTES EN UN SOLO REGISTRO
// UN DOT PRODUCT E SUNA MULTIPLICACIÓN -> EN DOS 
// BLOQUES DE 128 SE PUEDE METER
// EN DOS REGISTROS Y TENER UN RESULTADO EN 
// OTRO REGISTRO.
// 4 OPERACIONES EN COSTO DE UNA.
struct MODEL_VERTEX
{
	Vector3 position;
	Vector3 color;
	Vector3 normal;
	Vector3 tangent;
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
	int vertex_index = -1;
	int uv_index = -1;
	int normal_index = -1;

	bool operator==(const FaceVertex& vertex) const 
	{
		return vertex_index == vertex.vertex_index &&
					 uv_index == vertex.uv_index &&
					 normal_index == vertex.normal_index;
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
	~Model() = default;

	bool LoadFromFile(const char* filePath);
	void ComputeTangentSpace();
	//TODO: Iterar sobre el vector de meshes y ahí estarán los buffer
	Vector<Mesh> m_meshes;

	Vector<MODEL_VERTEX> m_vertices;
	Vector<unsigned short> m_indices;
	
	//Cuántos buffers voy a querer generar
	SPtr<GraphicsBuffer> m_pVertexBuffer;
	SPtr<GraphicsBuffer> m_pIndexBuffer;

};

