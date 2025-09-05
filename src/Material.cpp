#include "Material.h"

Material::Material()
{
  m_albedo = make_shared<Texture>();
	m_normals = make_shared<Texture>();
	m_metallic = make_shared<Texture>();
	m_roughness = make_shared<Texture>();
	m_ambientOcclusion = make_shared<Texture>();
}

void Material::SetMaterialTexture(const String& path, TextureType::E textureType)
{
	switch (textureType)
	{

	case TextureType::ALBEDO:
		
		m_albedo->createFromFile(path);
		m_filePaths.insert(std::pair<TextureType::E, String>(TextureType::ALBEDO,path));
		
		break;

	case TextureType::NORMALS:
		
		m_normals->createFromFile(path);
		m_filePaths.insert(std::pair<TextureType::E, String>(TextureType::NORMALS, path));

		break;

	case TextureType::METALLIC:
		m_metallic->createFromFile(path);
		m_filePaths.insert(std::pair<TextureType::E, String>(TextureType::METALLIC, path));
		break;

	case TextureType::ROUGHNESS:
		m_roughness->createFromFile(path);
		m_filePaths.insert(std::pair<TextureType::E, String>(TextureType::ROUGHNESS, path));
		break;

	case TextureType::AMBIENT_OCCLUSION:
		m_ambientOcclusion->createFromFile(path);
		m_filePaths.insert(std::pair<TextureType::E, String>(TextureType::AMBIENT_OCCLUSION, path));
		break;

	default:
		break;
	}
}
