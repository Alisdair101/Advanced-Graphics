#pragma once

#include <windows.h>
#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <directxcolors.h>
#include <sstream>
#include "resource.h"
#include <iostream>

#include <vector>

#include "Structures.h"
#include "Camera.h"
#include "DDSTextureLoader.h"
#include "GameObject.h"
#include "SceneLight.h"
#include "RenderToTextureProcess.h"
#include "TextureHandler.h"

using namespace DirectX;

class Application
{
private:
#pragma region DirectX Initialisation Variables

	HINSTANCE               _hInst;
	HWND                    _hWnd;
	D3D_DRIVER_TYPE         _pDriverType;
	D3D_FEATURE_LEVEL       _pFeatureLevel;
	ID3D11Device*           _pd3dDevice;
	ID3D11DeviceContext*    _pImmediateContext;
	IDXGISwapChain*         _pSwapChain;

#pragma endregion

#pragma region Windows Decriptive Variables

	UINT _pWindowHeight;
	UINT _pWindowWidth;

	// Render dimensions - Change here to alter screen resolution
	UINT _pRenderHeight = 1080;
	UINT _pRenderWidth = 1920;

#pragma endregion

#pragma region Rasterizer States

	ID3D11DepthStencilState* _pDSLessEqual;
	ID3D11RasterizerState* _pRSCullNone;

	ID3D11RasterizerState* _pCCWcullMode;
	ID3D11RasterizerState* _pCWcullMode;

#pragma endregion

#pragma region Render Process Variables

	RenderToTextureProcess* _pSSAONormalMapRP;
	RenderToTextureProcess* _pSSAODepthMapRP;
	RenderToTextureProcess* _pSSAOAmbientMapRP;

	RenderToTextureProcess* _pBasicDepthMapRP;

	RenderToTextureProcess* _pBaseSceneRP;
	RenderToTextureProcess* _pDeferredRenderRP;

	RenderToTextureProcess* _pWhiteLightShadowMapRP;
	RenderToTextureProcess* _pRedLightShadowMapRP;
	RenderToTextureProcess* _pGreenLightShadowMapRP;
	RenderToTextureProcess* _pBlueLightShadowMapRP;

	RenderToTextureProcess* _pHBlurRP;
	RenderToTextureProcess* _pVBlurRP;
	RenderToTextureProcess* _pDOFHBlurRP;
	RenderToTextureProcess* _pDOFVBlurRP;

	RenderToTextureProcess* _pFinalPassRP;

	RenderToTextureProcess* _pBasicRP;
	RenderToTextureProcess* _pVertexRP;
	RenderToTextureProcess* _pPixelRP;
	RenderToTextureProcess* _pCurrentRenderProcess;

	bool _pDeferred;

#pragma endregion

#pragma region Buffer Variables

	ID3D11Buffer*           _pVertexBuffer;
	ID3D11Buffer*           _pIndexBuffer;

	ID3D11Buffer*           _pPlaneVertexBuffer;
	ID3D11Buffer*           _pPlaneIndexBuffer;

	ID3D11Buffer*           _pConstantBuffer;
	ID3D11Buffer*			_pSMConstantBuffer;
	ID3D11Buffer*			_pSSAOConstantBuffer;

#pragma endregion
	
#pragma region Handlers

	TextureHandler* _pTextureHandler;

#pragma endregion

#pragma region Light Variables

	vector<SceneLight*> _pLights;
	int _pCurrentLightIndex;

	bool _pToggleBasicLightPressed;
	bool _pToggleRedLightPressed;
	bool _pToggleGreenLightPressed;
	bool _pToggleBlueLightPressed;

	bool _pShadowsOn;
	bool _pToggleShadowsOnPressed;

	bool _pToggleChangeCurrentLight;

#pragma endregion

#pragma region Blur Intensity Controls

	float _pBlurIntensity;
	int _pBlurPassCount;
	bool _pDOFBlurActive;

#pragma endregion

#pragma region Game Objects

	vector<GameObject*> _pGameObjects;

#pragma endregion

#pragma region Camera

	Camera * _pCamera;

#pragma endregion

private:
	HRESULT InitWindow(HINSTANCE hInstance, int nCmdShow);
	HRESULT InitDevice();
	HRESULT InitConstantBuffers();
	HRESULT InitRasterizerState();
	void Cleanup();
	
	HRESULT InitRenderProcesses();
	HRESULT InitVertexBuffer();
	HRESULT InitIndexBuffer();

	void InitCamera();
	void InitTextures();
	void InitLights(Geometry geometry, Material material);
	void InitGameObjectCubes(Geometry cubeGeometry, Material noSpecMaterial, Material shinyMaterial);
	
	Light GetLightFromSceneLight(SceneLight* light);

	void UpdateLightsControls(float deltaTime);
	void SetupSSAOConstantBuffer(SSAOConstantBuffer* ssaoCB, ConstantBuffer* cb);
	void BuildRandomVectorMap();

public:
	Application();
	~Application();

	HRESULT Initialise(HINSTANCE hInstance, int nCmdShow);

	bool HandleKeyboard(MSG msg, float timeStep);

	void Update(float deltaTime, float FPS);
	void Draw();
};

