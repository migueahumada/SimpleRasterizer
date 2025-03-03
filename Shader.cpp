#include "Shader.h"

bool Shader::Compile(const Path& filePath, const String& entryFunction, const String& shaderModel)
{

	std::fstream shaderFile(filePath, std::ios::in | std::ios::ate);
	if (!shaderFile.is_open())
	{
		return false;
	}

	size_t fileSize = shaderFile.tellg();
	shaderFile.seekg(0, std::ios::beg);

	Vector<char> vShaderCode(fileSize);
	shaderFile.read(vShaderCode.data(), fileSize);

	UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined(_DEBUG)
	flags |= D3DCOMPILE_DEBUG;
#endif

	ID3DBlob* pErrorBlob = nullptr;

	HRESULT hr = D3DCompile(vShaderCode.data(),
		vShaderCode.size(),
		nullptr,
		nullptr,
		nullptr,
		entryFunction.c_str(),
		shaderModel.c_str(),
		flags,
		0,
		&m_pBlob,
		&pErrorBlob);

	if (FAILED(hr)) {
		MessageBoxA(nullptr, (LPCSTR)pErrorBlob->GetBufferPointer(), "Error", MB_OK);
		SAFE_RELEASE(pErrorBlob);
		return false;
	}

	return true;
}

