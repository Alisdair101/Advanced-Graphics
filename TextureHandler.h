#pragma once

#include <windows.h>
#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <directxcolors.h>
#include "DDSTextureLoader.h"
#include "resource.h"
#include "Camera.h"

#include <vector>

#include "RenderProcess.h"

class TextureHandler
{
	struct TextureSet
	{
		TextureSet(string newName, ID3D11ShaderResourceView* newTexture, ID3D11ShaderResourceView* newNormalMap, ID3D11ShaderResourceView* newHeightMap)
		{
			name = newName;

			texture = newTexture;
			normalMap = newNormalMap;
			heightMap = newHeightMap;
		}

		string name;

		ID3D11ShaderResourceView* texture;
		ID3D11ShaderResourceView* normalMap;
		ID3D11ShaderResourceView* heightMap;
	};

public:
	TextureHandler();
	~TextureHandler();

	void AddTexture(ID3D11Device* d3dDevice, string textureSetName, LPCWSTR texturePath, LPCWSTR normalMapPath, LPCWSTR heightMapPath);
	vector<ID3D11ShaderResourceView*> GetTextureSet(string textureSetName);

private:
	vector<TextureSet*> _pTextureList;

};

