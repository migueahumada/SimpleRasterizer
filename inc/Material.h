#pragma once

/*
* Simple GBuffer implementation with this layout
* Pos | x | y | z | M |
* Col | x | y | z | R |
* Nm  | R | G | B | A |
*/

/*
	- Albedo
	- Normales
	- Metallic
	- Roughness
	- AmbientOclussion
*/
#include "Texture.h"
#include "HelperMacros.h"

//Texture Manager podría hacerse

namespace TextureType
{
	enum E{
		ALBEDO,
		NORMALS,
		METALLIC,
		ROUGHNESS,
		AMBIENT_OCCLUSION
	};
}

class Material
{
public:
	Material();
	~Material() = default;

	SPtr<Texture> getAlbedo(){
		return m_albedo;
	}

	SPtr<Texture> getNormals() {
		return m_normals;
	}

	SPtr<Texture> getRoughness() {
		return m_roughness;
	}

	SPtr<Texture> getMetallic() {
		return m_metallic;
	}

	void SetMaterialTexture(const String& path, TextureType::E textureType);

	UnorderedMap<TextureType::E, String> m_filePaths;


private:

	SPtr<Texture> m_albedo;
	SPtr<Texture> m_normals;
	SPtr<Texture> m_metallic;
	SPtr<Texture> m_roughness;
	SPtr<Texture> m_ambientOcclusion;

	

};


