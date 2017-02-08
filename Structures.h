#pragma once

#include <windows.h>
#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <directxcolors.h>
#include "DDSTextureLoader.h"
#include "Camera.h"

struct SimpleVertex
{
	XMFLOAT3 PosL;
	XMFLOAT3 NormL;
	XMFLOAT2 Tex;
	XMFLOAT3 Tangent;
};

struct QuadVertex
{
	XMFLOAT3 PosL;
	XMFLOAT3 NormL;
	XMFLOAT2 Tex;
};

struct SMConstantBuffer
{
	XMMATRIX World;
	XMMATRIX View;
	XMMATRIX Projection;
};

struct SSAOConstantBuffer
{
	XMMATRIX World;
	XMMATRIX View;
	XMMATRIX Projection;

	XMMATRIX ViewToTex;
	XMMATRIX WorldInvTransposeView;

	XMFLOAT4 OffsetVectors[14];
	XMFLOAT4 FrustumCorners[4];

	float HasNormalMap;
};

struct SurfaceInfo
{
	XMFLOAT4 AmbientMtrl;
	XMFLOAT4 DiffuseMtrl;
	XMFLOAT4 SpecularMtrl;
};

struct Light
{
	XMMATRIX View;
	XMMATRIX Projection;

	XMFLOAT4 AmbientLight;
	XMFLOAT4 DiffuseLight;
	XMFLOAT4 SpecularLight;

	float SpecularPower;
	XMFLOAT3 LightVecW;

	XMFLOAT3 paddingLightAmount;
	float lightOn;
};

struct ConstantBuffer
{
	XMMATRIX World;
	XMMATRIX View;
	XMMATRIX Projection;

	SurfaceInfo surface;
	Light lights[4];

	XMFLOAT3 EyePosW;
	float HasTexture;

	float HasNormalMap;
	float HasHeightMap;
	float shadowsOn;
	float screenWidth;
	float screenHeight;
	float blurIntensity;
};