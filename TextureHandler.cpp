#include "TextureHandler.h"



TextureHandler::TextureHandler()
{
}


TextureHandler::~TextureHandler()
{
}

void TextureHandler::AddTexture(ID3D11Device* d3dDevice, string textureSetName, LPCWSTR texturePath, LPCWSTR normalMapPath, LPCWSTR heightMapPath)
{
	ID3D11ShaderResourceView* texture = nullptr;
	ID3D11ShaderResourceView* normalMap = nullptr;
	ID3D11ShaderResourceView* heightMap = nullptr;

	if (texturePath != nullptr)
	{
		CreateDDSTextureFromFile(d3dDevice, texturePath, nullptr, &texture);
	}
	
	if (normalMapPath != nullptr)
	{
		CreateDDSTextureFromFile(d3dDevice, normalMapPath, nullptr, &normalMap);
	}

	if (heightMapPath != nullptr)
	{
		CreateDDSTextureFromFile(d3dDevice, heightMapPath, nullptr, &heightMap);
	}

	TextureSet* textureSet = new TextureSet(textureSetName, texture, normalMap, heightMap);

	_pTextureList.push_back(textureSet);
}

vector<ID3D11ShaderResourceView*> TextureHandler::GetTextureSet(string textureSetName)
{
	vector<ID3D11ShaderResourceView*> textures;

	for (ID3D11ShaderResourceView* shadderResView : textures)
	{
		shadderResView = nullptr;
	}

	for (TextureSet* textureSet : _pTextureList)
	{
		if (textureSet->name == textureSetName)
		{
			textures.push_back(textureSet->texture);
			textures.push_back(textureSet->normalMap);
			textures.push_back(textureSet->heightMap);

			return textures;
		}
	}

	return textures;
}