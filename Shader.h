#pragma once
#include <d3d11_2.h>
#include <string>
#include <fstream>
#include <vector>
#include <d3dcompiler.h>
#include "HelperMacros.h"

//This should be cstring

template <typename T>
using Vector = std::vector<T>;

using Path = std::wstring;
using String = std::string;

class Shader
{
public:
	Shader() = default;
	virtual ~Shader() {
		SAFE_RELEASE(m_pBlob);
	};

	bool Compile( const Path& filePath, 
								const String& entryFunction,
								const String& shaderModel);

	
	ID3DBlob* GetBlob() const { return m_pBlob; }
private:
	ID3DBlob* m_pBlob = nullptr;
};

class VertexShader : public Shader
{
public:
	VertexShader() = default;
	virtual ~VertexShader() {
		SAFE_RELEASE(m_pVertexShader);
	};

protected:
	friend class GraphicsAPI;
	ID3D11VertexShader* m_pVertexShader = nullptr;
};

class PixelShader : public Shader
{
public:
	PixelShader() = default;
	virtual ~PixelShader() {
		SAFE_RELEASE(m_pPixelShader);
	};
protected:
	friend class GraphicsAPI;
	ID3D11PixelShader* m_pPixelShader = nullptr;
};
