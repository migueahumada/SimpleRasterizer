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

class Material
{
public:
	Material();
	~Material();

private:
	
};


