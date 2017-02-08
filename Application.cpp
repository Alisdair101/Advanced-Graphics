#include "Application.h"

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}

bool Application::HandleKeyboard(MSG msg, float deltaTime)
{
	_pCamera->HandleControls(msg);

	for (GameObject* gameObject : _pGameObjects)
	{
		gameObject->HandleControls();
	}

	UpdateLightsControls(deltaTime);

	if (GetAsyncKeyState('U'))
	{
		_pGameObjects.at(4)->SetPosition(XMFLOAT3(_pGameObjects.at(4)->GetPosition().x, _pGameObjects.at(4)->GetPosition().y, _pGameObjects.at(4)->GetPosition().z + 0.1f));
	}
	if (GetAsyncKeyState('H'))
	{
		_pGameObjects.at(4)->SetPosition(XMFLOAT3(_pGameObjects.at(4)->GetPosition().x - 0.1f, _pGameObjects.at(4)->GetPosition().y, _pGameObjects.at(4)->GetPosition().z));
	}
	if (GetAsyncKeyState('J'))
	{
		_pGameObjects.at(4)->SetPosition(XMFLOAT3(_pGameObjects.at(4)->GetPosition().x, _pGameObjects.at(4)->GetPosition().y, _pGameObjects.at(4)->GetPosition().z - 0.1f));
	}
	if (GetAsyncKeyState('K'))
	{
		_pGameObjects.at(4)->SetPosition(XMFLOAT3(_pGameObjects.at(4)->GetPosition().x + 0.1f, _pGameObjects.at(4)->GetPosition().y, _pGameObjects.at(4)->GetPosition().z));
	}

#pragma region Blur Controls

	// Increase and decrease Blur

	if (!GetAsyncKeyState(VK_CONTROL))
	{
		if (GetAsyncKeyState(VK_OEM_MINUS))
		{
			_pBlurIntensity -= 0.01f * deltaTime;
		}
		else if (GetAsyncKeyState(VK_OEM_PLUS))
		{
			_pBlurIntensity += 0.01f * deltaTime;
		}
	}
	else
	{
		if (GetAsyncKeyState(VK_OEM_MINUS))
		{
			_pBlurPassCount -= 10;
		}
		else if (GetAsyncKeyState(VK_OEM_PLUS))
		{
			_pBlurPassCount += 10;
		}
	}
	if (_pBlurIntensity < 0)
	{
		_pBlurIntensity = 0;
	}

	if (_pBlurPassCount < 0)
	{
		_pBlurPassCount = 0;
	}
	else if (_pBlurPassCount > 150)
	{
		_pBlurPassCount = 150;
	}

#pragma endregion

#pragma region Toggle Current Shaders

	if (!GetAsyncKeyState(VK_CONTROL))
	{
		// Change Shader File
		if (GetAsyncKeyState('1'))
		{
			_pCurrentRenderProcess = _pBasicRP;
			_pHBlurRP->RemoveShaderResources();
			_pHBlurRP->AddShaderResource(_pBasicRP->GetShaderTargetTexture("OutputText"));
		}
		else if (GetAsyncKeyState('2'))
		{
			_pCurrentRenderProcess = _pVertexRP;
			_pHBlurRP->RemoveShaderResources();
			_pHBlurRP->AddShaderResource(_pVertexRP->GetShaderTargetTexture("OutputText"));
		}
		else if (GetAsyncKeyState('3'))
		{
			_pCurrentRenderProcess = _pPixelRP;
			_pHBlurRP->RemoveShaderResources();
			_pHBlurRP->AddShaderResource(_pPixelRP->GetShaderTargetTexture("OutputText"));
		}
		else if (GetAsyncKeyState('4'))
		{
			_pBaseSceneRP->RemoveShaderResources();
			_pBaseSceneRP->AddShaderResource(_pWhiteLightShadowMapRP->GetDepthMapResourceView());
			_pBaseSceneRP->AddShaderResource(_pRedLightShadowMapRP->GetDepthMapResourceView());
			_pBaseSceneRP->AddShaderResource(_pGreenLightShadowMapRP->GetDepthMapResourceView());
			_pBaseSceneRP->AddShaderResource(_pBlueLightShadowMapRP->GetDepthMapResourceView());
			_pBaseSceneRP->SetCurrentShaderIndex(0);
			_pCurrentRenderProcess = _pBaseSceneRP;
			_pHBlurRP->RemoveShaderResources();
			_pHBlurRP->AddShaderResource(_pCurrentRenderProcess->GetShaderTargetTexture("ColourMap"));
			_pFinalPassRP->RemoveShaderResources();
			_pFinalPassRP->AddShaderResource(_pVBlurRP->GetShaderTargetTexture("OutputText"));

			_pDeferred = false;
			_pDOFBlurActive = false;
		}
		else if (GetAsyncKeyState('5'))
		{
			_pBaseSceneRP->RemoveShaderResources();
			_pBaseSceneRP->SetCurrentShaderIndex(1);
			_pCurrentRenderProcess = _pBaseSceneRP;
			_pHBlurRP->RemoveShaderResources();
			_pCurrentRenderProcess = _pBaseSceneRP;
			_pHBlurRP->RemoveShaderResources();
			_pHBlurRP->AddShaderResource(_pCurrentRenderProcess->GetShaderTargetTexture("ColourMap"));
			_pDeferred = true;
		}
		else if (GetAsyncKeyState('6'))
		{
			_pBaseSceneRP->RemoveShaderResources();
			_pBaseSceneRP->SetCurrentShaderIndex(1);
			_pCurrentRenderProcess = _pBaseSceneRP;
			_pHBlurRP->RemoveShaderResources();
			_pHBlurRP->AddShaderResource(_pCurrentRenderProcess->GetShaderTargetTexture("NormalMap"));
			_pDeferred = true;
		}
		else if (GetAsyncKeyState('7'))
		{
			_pBaseSceneRP->SetCurrentShaderIndex(1);
			_pCurrentRenderProcess = _pBaseSceneRP;
			_pHBlurRP->RemoveShaderResources();
			_pHBlurRP->AddShaderResource(_pCurrentRenderProcess->GetShaderTargetTexture("PositionMap"));
			_pDeferred = true;
		}
		else if (GetAsyncKeyState(VK_SHIFT) && GetAsyncKeyState('8'))
		{
			_pBaseSceneRP->RemoveShaderResources();
			_pBaseSceneRP->SetCurrentShaderIndex(1);
			_pCurrentRenderProcess = _pBaseSceneRP;
			_pHBlurRP->RemoveShaderResources();
			_pHBlurRP->AddShaderResource(_pDeferredRenderRP->GetShaderTargetTexture("OutputText"));
			_pFinalPassRP->RemoveShaderResources();
			_pFinalPassRP->AddShaderResource(_pVBlurRP->GetShaderTargetTexture("OutputText"));
			_pDeferred = true;
			_pDOFBlurActive = false;
		}
		else if (GetAsyncKeyState('9') && !_pToggleShadowsOnPressed)
		{
			_pShadowsOn = !_pShadowsOn;
			_pToggleShadowsOnPressed = true;
		}
		else if (GetAsyncKeyState('0'))
		{
			//_pBaseSceneRP->SetCurrentShaderIndex(0);
			if (_pDeferred)
			{
				_pCurrentRenderProcess = _pBaseSceneRP;
				_pHBlurRP->RemoveShaderResources();
				_pHBlurRP->AddShaderResource(_pDeferredRenderRP->GetShaderTargetTexture("OutputText"));
				_pFinalPassRP->RemoveShaderResources();
				_pFinalPassRP->AddShaderResource(_pDOFVBlurRP->GetShaderTargetTexture("OutputText"));
			}
			else
			{
				_pCurrentRenderProcess = _pBaseSceneRP;
				_pHBlurRP->RemoveShaderResources();
				_pHBlurRP->AddShaderResource(_pCurrentRenderProcess->GetShaderTargetTexture("ColourMap"));
				_pFinalPassRP->RemoveShaderResources();
				_pFinalPassRP->AddShaderResource(_pDOFVBlurRP->GetShaderTargetTexture("OutputText"));
			}

			_pDOFBlurActive = true;
		}
	}

#pragma endregion

#pragma region Toggle Shadows

	if (!GetAsyncKeyState('9') && _pToggleShadowsOnPressed)
	{
		_pToggleShadowsOnPressed = !_pToggleShadowsOnPressed;
	}

#pragma endregion

#pragma region Turn Lights ON/OFF

	// Toggle Light ON/OFF
	if (GetAsyncKeyState(VK_CONTROL))
	{
		if (GetAsyncKeyState('1') && !_pToggleBasicLightPressed)
		{
			_pLights.at(0)->ToggleLightOn();
			_pToggleBasicLightPressed = !_pToggleBasicLightPressed;
		}
		else if (GetAsyncKeyState('2') && !_pToggleRedLightPressed)
		{
			_pLights.at(1)->ToggleLightOn();
			_pToggleRedLightPressed = !_pToggleRedLightPressed;
		}
		else if (GetAsyncKeyState('3') && !_pToggleGreenLightPressed)
		{
			_pLights.at(2)->ToggleLightOn();
			_pToggleGreenLightPressed = !_pToggleGreenLightPressed;
		}
		else if (GetAsyncKeyState('4') && !_pToggleBlueLightPressed)
		{
			_pLights.at(3)->ToggleLightOn();
			_pToggleBlueLightPressed = !_pToggleBlueLightPressed;
		}
	}

	if (!GetAsyncKeyState('1') && _pToggleBasicLightPressed)
	{
		_pToggleBasicLightPressed = !_pToggleBasicLightPressed;
	}
	if (!GetAsyncKeyState('2') && _pToggleRedLightPressed)
	{
		_pToggleRedLightPressed = !_pToggleRedLightPressed;
	}
	if (!GetAsyncKeyState('3') && _pToggleGreenLightPressed)
	{
		_pToggleGreenLightPressed = !_pToggleGreenLightPressed;
	}
	if (!GetAsyncKeyState('4') && _pToggleBlueLightPressed)
	{
		_pToggleBlueLightPressed = !_pToggleBlueLightPressed;
	}

#pragma endregion

	return false;
}

Application::Application()
{
	_hInst = nullptr;
	_hWnd = nullptr;
	_pDriverType = D3D_DRIVER_TYPE_NULL;
	_pFeatureLevel = D3D_FEATURE_LEVEL_11_0;
	_pd3dDevice = nullptr;
	_pImmediateContext = nullptr;
	_pSwapChain = nullptr;

	_pVertexBuffer = nullptr;
	_pIndexBuffer = nullptr;
	_pConstantBuffer = nullptr;
	_pSMConstantBuffer = nullptr;
	_pSSAOConstantBuffer = nullptr;

	_pDSLessEqual = nullptr;
	_pRSCullNone = nullptr;
}

Application::~Application()
{
	Cleanup();
}

HRESULT Application::Initialise(HINSTANCE hInstance, int nCmdShow)
{
    if (FAILED(InitWindow(hInstance, nCmdShow)))
	{
        return E_FAIL;
	}

    RECT rc;
    GetClientRect(_hWnd, &rc);
    _pWindowWidth = rc.right - rc.left;
    _pWindowHeight = rc.bottom - rc.top;

    if (FAILED(InitDevice()))
    {
        Cleanup();

        return E_FAIL;
    }

	_pShadowsOn = false;
	_pToggleShadowsOnPressed = false;
	_pToggleChangeCurrentLight = false;

	Geometry cubeGeometry;
	cubeGeometry.indexBuffer = _pIndexBuffer;
	cubeGeometry.vertexBuffer = _pVertexBuffer;
	cubeGeometry.numberOfIndices = 36;
	cubeGeometry.vertexBufferOffset = 0;
	cubeGeometry.vertexBufferStride = sizeof(SimpleVertex);

	Geometry planeGeometry;
	planeGeometry.indexBuffer = _pPlaneIndexBuffer;
	planeGeometry.vertexBuffer = _pPlaneVertexBuffer;
	planeGeometry.numberOfIndices = 6;
	planeGeometry.vertexBufferOffset = 0;
	planeGeometry.vertexBufferStride = sizeof(SimpleVertex);

	Material shinyMaterial;
	shinyMaterial.ambient = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	shinyMaterial.diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	shinyMaterial.specular = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	shinyMaterial.specularPower = 10.0f;

	Material noSpecMaterial;
	noSpecMaterial.ambient = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
	noSpecMaterial.diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	noSpecMaterial.specular = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	noSpecMaterial.specularPower = 0.0f;

	InitCamera();
	InitTextures();
	InitLights(cubeGeometry, shinyMaterial);
	InitGameObjectCubes(cubeGeometry, noSpecMaterial, shinyMaterial);

	return S_OK;
}

void Application::InitCamera()
{
	// Setup Camera
	XMFLOAT3 eye = XMFLOAT3(35.0f, 15.0f, -35.0f);
	XMFLOAT3 at = XMFLOAT3(0.0f, 2.0f, 0.0f);
	XMFLOAT3 up = XMFLOAT3(0.0f, 1.0f, 0.0f);

	_pCamera = new Camera(eye, at, up, (float)_pRenderWidth, (float)_pRenderHeight, 0.01f, 200.0f);

	_pCamera->SetPosition(eye);

}

void Application::InitTextures()
{
	_pTextureHandler = new TextureHandler();

	_pTextureHandler->AddTexture(_pd3dDevice, "Brick Texture", L"Resources\\brickTexture.dds", L"Resources\\brick_NRM.dds", L"Resources\\brick_DISP.dds");
	_pTextureHandler->AddTexture(_pd3dDevice, "Stone Texture", L"Resources\\stoneTexture.dds", L"Resources\\stone_NRM.dds", L"Resources\\stone_DISP.dds");
	_pTextureHandler->AddTexture(_pd3dDevice, "Crate Texture", L"Resources\\Crate_COLOR.dds", L"Resources\\Crate_NRM.dds", L"Resources\\Crate_DISP.dds");
	_pTextureHandler->AddTexture(_pd3dDevice, "Floor Texture", L"Resources\\floorTexture.dds", L"Resources\\floor_NRM.dds", L"Resources\\floor_DISP.dds");
	_pTextureHandler->AddTexture(_pd3dDevice, "Cracked Rock Texture", L"Resources\\crackedRockTexture.dds", L"Resources\\crackedRock_NRM.dds", L"Resources\\crackedRock_DISP.dds");
	_pTextureHandler->AddTexture(_pd3dDevice, "White Texture", L"Resources\\whiteColourTexture.dds", nullptr, nullptr);
	_pTextureHandler->AddTexture(_pd3dDevice, "Red Texture", L"Resources\\redColourTexture.dds", nullptr, nullptr);
	_pTextureHandler->AddTexture(_pd3dDevice, "Green Texture", L"Resources\\greenColourTexture.dds", nullptr, nullptr);
	_pTextureHandler->AddTexture(_pd3dDevice, "Blue Texture", L"Resources\\blueColourTexture.dds", nullptr, nullptr);
}

void Application::InitLights(Geometry geometry, Material material)
{
	SceneLight* whiteLight = new SceneLight("WhiteLightPosCube", _pTextureHandler->GetTextureSet("White Texture").at(0), geometry, material);
	SceneLight* redLight = new SceneLight("RedLightPosCube", _pTextureHandler->GetTextureSet("Red Texture").at(0), geometry, material);
	SceneLight* greenLight = new SceneLight("GreenLightPosCube", _pTextureHandler->GetTextureSet("Green Texture").at(0), geometry, material);
	SceneLight* blueLight = new SceneLight("BlueLightPosCube", _pTextureHandler->GetTextureSet("Blue Texture").at(0), geometry, material);

	// Setup the scene's light
	whiteLight->SetAmbientLight(XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f));
	whiteLight->SetDiffuseLight(XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
	whiteLight->SetSpecularLight(XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f));
	whiteLight->SetSpecularPower(20.0f);
	whiteLight->SetLightVecW(XMFLOAT3(-4.0f, 10.0f, 10.0f));
	whiteLight->SetPaddingLightAmount(XMFLOAT3(0.0f, 0.0f, 0.0f));
	whiteLight->SetLightOn(0.0f);
	whiteLight->GetLightCubeGO()->SetPosition(whiteLight->GetLightVecW());
	_pToggleBasicLightPressed = false;

	_pCurrentLightIndex = 1;

	// Setup the scene's RED light
	redLight->SetAmbientLight(XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f));
	redLight->SetDiffuseLight(XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f));
	redLight->SetSpecularLight(XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f));
	redLight->SetSpecularPower(20.0f);
	redLight->SetLightVecW(XMFLOAT3(0.0f, 10.0f, 10.0f));
	redLight->SetPaddingLightAmount(XMFLOAT3(0.0f, 0.0f, 0.0f));
	redLight->SetLightOn(1.0f);
	redLight->GetLightCubeGO()->SetPosition(redLight->GetLightVecW());
	_pToggleRedLightPressed = false;

	// Setup the scene's GREEN light
	greenLight->SetAmbientLight(XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f));
	greenLight->SetDiffuseLight(XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f));
	greenLight->SetSpecularLight(XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f));
	greenLight->SetSpecularPower(20.0f);
	greenLight->SetLightVecW(XMFLOAT3(4.0f, 10.0f, 10.0f));
	greenLight->SetPaddingLightAmount(XMFLOAT3(0.0f, 0.0f, 0.0f));
	greenLight->SetLightOn(0.0f);
	greenLight->GetLightCubeGO()->SetPosition(greenLight->GetLightVecW());
	_pToggleGreenLightPressed = false;

	// Setup the scene's BLUE light
	blueLight->SetAmbientLight(XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f));
	blueLight->SetDiffuseLight(XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f));
	blueLight->SetSpecularLight(XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f));
	blueLight->SetSpecularPower(20.0f);
	blueLight->SetLightVecW(XMFLOAT3(8.0f, 10.0f, 10.0f));
	blueLight->SetPaddingLightAmount(XMFLOAT3(0.0f, 0.0f, 0.0f));
	blueLight->SetLightOn(0.0f);
	blueLight->GetLightCubeGO()->SetPosition(blueLight->GetLightVecW());
	_pToggleBlueLightPressed = false;

	_pLights.push_back(whiteLight);
	_pLights.push_back(redLight);
	_pLights.push_back(greenLight);
	_pLights.push_back(blueLight);

	_pGameObjects.push_back(whiteLight->GetLightCubeGO());
	_pGameObjects.push_back(redLight->GetLightCubeGO());
	_pGameObjects.push_back(greenLight->GetLightCubeGO());
	_pGameObjects.push_back(blueLight->GetLightCubeGO());

	_pCurrentLightIndex = 1;
}

void Application::InitGameObjectCubes(Geometry cubeGeometry, Material noSpecMaterial, Material shinyMaterial)
{
	GameObject* gameObject = new GameObject("Floor", cubeGeometry, noSpecMaterial);
	gameObject->SetPosition(0.0f, -15.0f, 0.0f);
	gameObject->SetScale(15.0f, 15.0f, 15.0f);
	gameObject->SetRotation(0.0f, 0.0f, 0.0f);
	gameObject->SetTextures(_pTextureHandler->GetTextureSet("Floor Texture"));

	_pGameObjects.push_back(gameObject);

	_pBlurIntensity = 0;
	_pBlurPassCount = 0;
	_pDOFBlurActive = false;

	for (auto i = 0; i < 5; i++)
	{
		gameObject = new GameObject("Cube " + i, cubeGeometry, shinyMaterial);
		gameObject->SetScale(1.0f, 1.0f, 1.0f);
		gameObject->SetPosition(-4.0f + (i * 4.0f), 5.0f, 10.0f);
		gameObject->SetRotation(0.0f, 0.0f, 0.0f);

		switch (i)
		{
		case 0:
			gameObject->SetTextures(_pTextureHandler->GetTextureSet("Floor Texture"));
			break;

		case 1:
			gameObject->SetTextures(_pTextureHandler->GetTextureSet("Stone Texture"));
			break;

		case 2:
			gameObject->SetTextures(_pTextureHandler->GetTextureSet("Crate Texture"));
			break;

		case 3:
			gameObject->SetTextures(_pTextureHandler->GetTextureSet("Floor Texture"));
			break;

		case 4:
			gameObject->SetTextures(_pTextureHandler->GetTextureSet("Cracked Rock Texture"));
			break;
		}

		_pGameObjects.push_back(gameObject);
	}
}

HRESULT Application::InitVertexBuffer()
{
	HRESULT hr;

#pragma region Create Cube Vertex Buffer

	// Frank Luna Normals and Tangents
	SimpleVertex vertices[] =
	{
		// Top Face
		{ XMFLOAT3(-1.0f, 1.0f, -1.0f),		XMFLOAT3(0.0f, 1.0f, 0.0f),		XMFLOAT2(1.0f, 0.0f),  XMFLOAT3(-1.0f, 0.0f, 0.0f) },	// 0
		{ XMFLOAT3(1.0f, 1.0f, -1.0f),		XMFLOAT3(0.0f, 1.0f, 0.0f),		XMFLOAT2(0.0f, 0.0f),  XMFLOAT3(-1.0f, 0.0f, 0.0f) },	// 1
		{ XMFLOAT3(1.0f, 1.0f, 1.0f),		XMFLOAT3(0.0f, 1.0f, 0.0f),		XMFLOAT2(0.0f, 1.0f),  XMFLOAT3(-1.0f, 0.0f, 0.0f) },	// 2
		{ XMFLOAT3(-1.0f, 1.0f, 1.0f),		XMFLOAT3(0.0f, 1.0f, 0.0f),		XMFLOAT2(1.0f, 1.0f),  XMFLOAT3(-1.0f, 0.0f, 0.0f) },	// 3

		// Bottom Face
		{ XMFLOAT3(-1.0f, -1.0f, -1.0f),	XMFLOAT3(0.0f, 1.0f, 0.0f),		XMFLOAT2(0.0f, 0.0f),  XMFLOAT3(1.0f, 0.0f, 0.0f) },	// 4
		{ XMFLOAT3(1.0f, -1.0f, -1.0f),		XMFLOAT3(0.0f, 1.0f, 0.0f),		XMFLOAT2(1.0f, 0.0f),  XMFLOAT3(1.0f, 0.0f, 0.0f) },	// 5
		{ XMFLOAT3(1.0f, -1.0f, 1.0f),		XMFLOAT3(0.0f, 1.0f, 0.0f),		XMFLOAT2(1.0f, 1.0f),  XMFLOAT3(1.0f, 0.0f, 0.0f) },	// 6
		{ XMFLOAT3(-1.0f, -1.0f, 1.0f),		XMFLOAT3(0.0f, 1.0f, 0.0f),		XMFLOAT2(0.0f, 1.0f),  XMFLOAT3(1.0f, 0.0f, 0.0f) },	// 7

		// Left Face
		{ XMFLOAT3(-1.0f, -1.0f, 1.0f),		XMFLOAT3(-1.0f, 0.0f, 0.0f),	XMFLOAT2(0.0f, 1.0f),  XMFLOAT3(0.0f, 0.0f, -1.0f) },	// 8
		{ XMFLOAT3(-1.0f, -1.0f, -1.0f),	XMFLOAT3(-1.0f, 0.0f, 0.0f),	XMFLOAT2(1.0f, 1.0f),  XMFLOAT3(0.0f, 0.0f, -1.0f) },	// 9
		{ XMFLOAT3(-1.0f, 1.0f, -1.0f),		XMFLOAT3(-1.0f, 0.0f, 0.0f),	XMFLOAT2(1.0f, 0.0f),  XMFLOAT3(0.0f, 0.0f, -1.0f) },	// 10
		{ XMFLOAT3(-1.0f, 1.0f, 1.0f),		XMFLOAT3(-1.0f, 0.0f, 0.0f),	XMFLOAT2(0.0f, 0.0f),  XMFLOAT3(0.0f, 0.0f, -1.0f) },	// 11

		// Right Face
		{ XMFLOAT3(1.0f, -1.0f, 1.0f),		XMFLOAT3(1.0f, 0.0f, 0.0f),		XMFLOAT2(1.0f, 1.0f),  XMFLOAT3(0.0f, 0.0f, 1.0f) },	// 12
		{ XMFLOAT3(1.0f, -1.0f, -1.0f),		XMFLOAT3(1.0f, 0.0f, 0.0f),		XMFLOAT2(0.0f, 1.0f),  XMFLOAT3(0.0f, 0.0f, 1.0f) },	// 13
		{ XMFLOAT3(1.0f, 1.0f, -1.0f),		XMFLOAT3(1.0f, 0.0f, 0.0f),		XMFLOAT2(0.0f, 0.0f),  XMFLOAT3(0.0f, 0.0f, 1.0f) },	// 14
		{ XMFLOAT3(1.0f, 1.0f, 1.0f),		XMFLOAT3(1.0f, 0.0f, 0.0f),		XMFLOAT2(1.0f, 0.0f),  XMFLOAT3(0.0f, 0.0f, 1.0f) },	// 15

		// Front Face
		{ XMFLOAT3(-1.0f, -1.0f, -1.0f),	XMFLOAT3(0.0f, 0.0f, -1.0f),	XMFLOAT2(0.0f, 1.0f),  XMFLOAT3(1.0f, 0.0f, 0.0f) },	// 16
		{ XMFLOAT3(1.0f, -1.0f, -1.0f),		XMFLOAT3(0.0f, 0.0f, -1.0f),	XMFLOAT2(1.0f, 1.0f),  XMFLOAT3(1.0f, 0.0f, 0.0f) },	// 17
		{ XMFLOAT3(1.0f, 1.0f, -1.0f),		XMFLOAT3(0.0f, 0.0f, -1.0f),	XMFLOAT2(1.0f, 0.0f),  XMFLOAT3(1.0f, 0.0f, 0.0f) },	// 18
		{ XMFLOAT3(-1.0f, 1.0f, -1.0f),		XMFLOAT3(0.0f, 0.0f, -1.0f),	XMFLOAT2(0.0f, 0.0f),  XMFLOAT3(1.0f, 0.0f, 0.0f) },	// 19

		// Back Face
		{ XMFLOAT3(-1.0f, -1.0f, 1.0f),		XMFLOAT3(0.0f, 0.0f, 1.0f),		XMFLOAT2(1.0f, 1.0f),  XMFLOAT3(-1.0f, 0.0f, 0.0f) },	// 20
		{ XMFLOAT3(1.0f, -1.0f, 1.0f),		XMFLOAT3(0.0f, 0.0f, 1.0f),		XMFLOAT2(0.0f, 1.0f),  XMFLOAT3(-1.0f, 0.0f, 0.0f) },	// 21
		{ XMFLOAT3(1.0f, 1.0f, 1.0f),		XMFLOAT3(0.0f, 0.0f, 1.0f),		XMFLOAT2(0.0f, 0.0f),  XMFLOAT3(-1.0f, 0.0f, 0.0f) },	// 22
		{ XMFLOAT3(-1.0f, 1.0f, 1.0f),		XMFLOAT3(0.0f, 0.0f, 1.0f),		XMFLOAT2(1.0f, 0.0f),  XMFLOAT3(-1.0f, 0.0f, 0.0f) },	// 23
	};

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(SimpleVertex) * 24;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = vertices;

	hr = _pd3dDevice->CreateBuffer(&bd, &InitData, &_pVertexBuffer);

	if (FAILED(hr))
		return hr;

#pragma endregion

#pragma region Create Plane Vertex Bufffer

	// Create Plane vertex buffer
	SimpleVertex planeVertices[] =
	{
		{ XMFLOAT3(-1.0f, -1.0f, 0.0f), XMFLOAT3(-1.0f, -1.0f, 0.0f),	XMFLOAT2(0.0f, 5.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f) },
		{ XMFLOAT3(1.0f, -1.0f, 0.0f),	XMFLOAT3(1.0f, -1.0f, 0.0f),	XMFLOAT2(5.0f, 5.0f), XMFLOAT3(0.0f, -1.0f, 0.0f) },
		{ XMFLOAT3(1.0f, 1.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 0.0f),		XMFLOAT2(5.0f, 0.0f), XMFLOAT3(1.0f, 0.0f, 0.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, 0.0f),	XMFLOAT3(-1.0f, 1.0f, 0.0f),	XMFLOAT2(0.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f) }
	};

	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(SimpleVertex) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;

	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = planeVertices;

	hr = _pd3dDevice->CreateBuffer(&bd, &InitData, &_pPlaneVertexBuffer);

	if (FAILED(hr))
		return hr;

#pragma endregion

	return S_OK;
}

HRESULT Application::InitIndexBuffer()
{
	HRESULT hr;

    // Create index buffer
    WORD indices[] =
    {
		3, 1, 0,
		2, 1, 3,

		6, 4, 5,
		7, 4, 6,

		11, 9, 8,
		10, 9, 11,

		14, 12, 13,
		15, 12, 14,

		19, 17, 16,
		18, 17, 19,

		22, 20, 21,
		23, 20, 22
    };

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));

    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(WORD) * 36;     
    bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
    InitData.pSysMem = indices;
    hr = _pd3dDevice->CreateBuffer(&bd, &InitData, &_pIndexBuffer);

    if (FAILED(hr))
        return hr;

	// Create plane index buffer
	WORD planeIndices[] =
	{
		0, 3, 1,
		3, 2, 1,
	};

	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(WORD) * 6;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;

	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = planeIndices;
	hr = _pd3dDevice->CreateBuffer(&bd, &InitData, &_pPlaneIndexBuffer);

	if (FAILED(hr))
		return hr;

	return S_OK;
}

HRESULT Application::InitWindow(HINSTANCE hInstance, int nCmdShow)
{
    // Register class
    WNDCLASSEX wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, (LPCTSTR)IDI_TUTORIAL1);
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW );
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = nullptr;
    wcex.lpszClassName = L"TutorialWindowClass";
    wcex.hIconSm = LoadIcon(wcex.hInstance, (LPCTSTR)IDI_TUTORIAL1);
    if (!RegisterClassEx(&wcex))
        return E_FAIL;

    // Create window
    _hInst = hInstance;
    RECT rc = {0, 0, 960, 540};
    AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
    _hWnd = CreateWindow(L"TutorialWindowClass", L"FGGC Semester 2 Framework", WS_OVERLAPPEDWINDOW,
                         CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr, hInstance,
                         nullptr);
    if (!_hWnd)
		return E_FAIL;

    ShowWindow(_hWnd, nCmdShow);

    return S_OK;
}

HRESULT Application::InitDevice()
{
    HRESULT hr = S_OK;

    UINT createDeviceFlags = 0;

#ifdef _DEBUG
    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    D3D_DRIVER_TYPE driverTypes[] =
    {
        D3D_DRIVER_TYPE_HARDWARE,
        D3D_DRIVER_TYPE_WARP,
        D3D_DRIVER_TYPE_REFERENCE,
    };

    UINT numDriverTypes = ARRAYSIZE(driverTypes);

    D3D_FEATURE_LEVEL featureLevels[] =
    {
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
    };

	UINT numFeatureLevels = ARRAYSIZE(featureLevels);

	UINT sampleCount = 4;

    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferCount = 1;
    sd.BufferDesc.Width = _pRenderWidth;
    sd.BufferDesc.Height = _pRenderHeight;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = _hWnd;
	sd.SampleDesc.Count = sampleCount;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;

    for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++)
    {
        _pDriverType = driverTypes[driverTypeIndex];
        hr = D3D11CreateDeviceAndSwapChain(nullptr, _pDriverType, nullptr, createDeviceFlags, featureLevels, numFeatureLevels,
                                           D3D11_SDK_VERSION, &sd, &_pSwapChain, &_pd3dDevice, &_pFeatureLevel, &_pImmediateContext);
        if (SUCCEEDED(hr))
            break;
    }

    if (FAILED(hr))
        return hr;

	hr = InitRenderProcesses();

	if (FAILED(hr))
		return hr;

	// Setup Render Target Views
	// Create a render target view for the back buffer
	ID3D11Texture2D* pBackBuffer = nullptr;
	hr = _pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);

	if (FAILED(hr))
		return hr;

	hr = _pFinalPassRP->SetupBackBufferRTV(_pd3dDevice, pBackBuffer);

	if (FAILED(hr))
		return hr;

#pragma endregion

	hr = InitConstantBuffers();

	if (FAILED(hr))
		return hr;

    // Setup the viewport
    D3D11_VIEWPORT vp;
    vp.Width = (FLOAT)_pRenderWidth;
    vp.Height = (FLOAT)_pRenderHeight;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    _pImmediateContext->RSSetViewports(1, &vp);

	InitVertexBuffer();
	InitIndexBuffer();

    // Set primitive topology
    _pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	hr = InitConstantBuffers();

	return hr;
}

HRESULT Application::InitConstantBuffers()
{
	HRESULT hr;

	// Create the constant buffer
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(ConstantBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	hr = _pd3dDevice->CreateBuffer(&bd, nullptr, &_pConstantBuffer);

	if (FAILED(hr))
		return hr;

	// Create the shadow map constant buffer
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(SMConstantBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	hr = _pd3dDevice->CreateBuffer(&bd, nullptr, &_pSMConstantBuffer);

	if (FAILED(hr))
		return hr;

	// Create the SSAO constant buffer
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(SSAOConstantBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;

	//ZeroMemory(&_pSSAOConstantBuffer, sizeof(SSAOConstantBuffer));
	hr = _pd3dDevice->CreateBuffer(&bd, nullptr, &_pSSAOConstantBuffer);

	if (FAILED(hr))
		return hr;
}

HRESULT Application::InitRasterizerState()
{
	HRESULT hr;

	// Rasterizer
	D3D11_RASTERIZER_DESC cmdesc;

	ZeroMemory(&cmdesc, sizeof(D3D11_RASTERIZER_DESC));
	cmdesc.FillMode = D3D11_FILL_SOLID;
	cmdesc.CullMode = D3D11_CULL_NONE;
	hr = _pd3dDevice->CreateRasterizerState(&cmdesc, &_pRSCullNone);

	if (FAILED(hr))
		return hr;

	D3D11_DEPTH_STENCIL_DESC dssDesc;
	ZeroMemory(&dssDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
	dssDesc.DepthEnable = true;
	dssDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dssDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

	_pd3dDevice->CreateDepthStencilState(&dssDesc, &_pDSLessEqual);

	ZeroMemory(&cmdesc, sizeof(D3D11_RASTERIZER_DESC));

	cmdesc.FillMode = D3D11_FILL_SOLID;
	cmdesc.CullMode = D3D11_CULL_BACK;

	cmdesc.FrontCounterClockwise = true;
	hr = _pd3dDevice->CreateRasterizerState(&cmdesc, &_pCCWcullMode);

	if (FAILED(hr))
		return hr;

	cmdesc.FrontCounterClockwise = false;
	hr = _pd3dDevice->CreateRasterizerState(&cmdesc, &_pCWcullMode);
	return S_OK;

	return hr;
}

HRESULT Application::InitRenderProcesses()
{
	HRESULT hr;

#pragma region Setup Sampler States

	ID3D11SamplerState* _pSamplerLinear;
	ID3D11SamplerState* _pSamplerClamp;
	ID3D11SamplerState* _pSamplerNormalDepth;

	D3D11_SAMPLER_DESC samplerDesc;

	// Create a wrap texture sampler state description.
	ZeroMemory(&samplerDesc, sizeof(samplerDesc));
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	hr = _pd3dDevice->CreateSamplerState(&samplerDesc, &_pSamplerLinear);

	if (FAILED(hr))
		return hr;

	// Create a clamp texture sampler state description.
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;

	hr = _pd3dDevice->CreateSamplerState(&samplerDesc, &_pSamplerClamp);

	if (FAILED(hr))
		return hr;

	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc.BorderColor[0] = 0.0f;
	samplerDesc.BorderColor[1] = 0.0f;
	samplerDesc.BorderColor[2] = 0.0f;
	samplerDesc.BorderColor[3] = 1e5f;

	hr = _pd3dDevice->CreateSamplerState(&samplerDesc, &_pSamplerNormalDepth);

	if (FAILED(hr))
		return hr;

#pragma endregion

#pragma region Set Render Processes as null pointers

	//_pSSAONormalMapRP = nullptr;

	_pWhiteLightShadowMapRP = nullptr;
	_pRedLightShadowMapRP = nullptr;
	_pBaseSceneRP = nullptr;
	_pHBlurRP = nullptr;
	_pVBlurRP = nullptr;
	_pFinalPassRP = nullptr;
	_pDeferred = false;

#pragma endregion

//#pragma region Setup SSAO Normal Map Render Process
//
//	_pSSAONormalMapRP = new RenderToTextureProcess((float)_pRenderWidth, (float)_pRenderHeight);
//	hr = _pSSAONormalMapRP->LoadShaderFilesAndInputLayouts(_pd3dDevice, L"DX11 Framework SSAO Normal Map.fx", L"DX11 Framework SSAO Normal Map.fx", 3);
//
//	if (FAILED(hr))
//		return hr;
//
//	hr = _pSSAONormalMapRP->SetupRTVAndSRV(_pd3dDevice, "SSAONormalMap");
//
//	if (FAILED(hr))
//		return hr;
//
//	_pSSAONormalMapRP->AddSamplerState(_pSamplerLinear);
//
//	_pSSAONormalMapRP->SetClearColour(1.0f, 1.0f, 1.0f, 1.0f);
//
//#pragma endregion
//
//#pragma region Setup SSAO Depth Map Render Process
//
//	_pSSAODepthMapRP = new RenderToTextureProcess((float)_pRenderWidth, (float)_pRenderHeight);
//	hr = _pSSAODepthMapRP->LoadShaderFilesAndInputLayouts(_pd3dDevice, L"DX11 Framework Depth Mapping.fx", L"DX11 Framework Depth Mapping.fx", 1);
//
//	if (FAILED(hr))
//		return hr;
//
//	hr = _pSSAODepthMapRP->SetupRTVAndSRV(_pd3dDevice, "SSAODepthMap");
//
//	if (FAILED(hr))
//		return hr;
//
//	_pSSAODepthMapRP->SetClearColour(1.0f, 1.0f, 1.0f, 1.0f);
//
//#pragma endregion
//
//#pragma region Setup SSAO Ambient Map Render Process
//
//	_pSSAOAmbientMapRP = new RenderToTextureProcess((float)_pRenderWidth, (float)_pRenderHeight);
//	hr = _pSSAOAmbientMapRP->LoadShaderFilesAndInputLayouts(_pd3dDevice, L"DX11 Framework SSAO Ambient Map.fx", L"DX11 Framework SSAO Ambient Map.fx", 2);
//
//	if (FAILED(hr))
//		return hr;
//
//	hr = _pSSAOAmbientMapRP->SetupRTVAndSRV(_pd3dDevice, "SSAOAmbientMap");
//
//	if (FAILED(hr))
//		return hr;
//
//	hr = _pSSAOAmbientMapRP->SetupRenderQuad(_pd3dDevice);
//
//	if (FAILED(hr))
//		return hr;
//
//	_pSSAOAmbientMapRP->AddSamplerState(_pSamplerLinear);
//	_pSSAOAmbientMapRP->AddSamplerState(_pSamplerNormalDepth);
//	_pSSAOAmbientMapRP->AddSamplerState(_pSamplerClamp);
//
//	_pSSAOAmbientMapRP->AddShaderResource(_pSSAONormalMapRP->GetShaderTargetTexture("SSAONormalMap"));
//	_pSSAOAmbientMapRP->AddShaderResource(_pSSAODepthMapRP->GetDepthMapResourceView());
//	BuildRandomVectorMap();
//
//	if (FAILED(hr))
//		return hr;
//
//	_pSSAOAmbientMapRP->SetClearColour(1.0f, 1.0f, 1.0f, 1.0f);
//
//#pragma endregion

#pragma region Setup White Light Shadow Mapping Render Process

	_pWhiteLightShadowMapRP = new RenderToTextureProcess((float)_pRenderWidth, (float)_pRenderHeight);
	hr = _pWhiteLightShadowMapRP->LoadShaderFilesAndInputLayouts(_pd3dDevice, L"DX11 Framework Depth Mapping.fx", L"DX11 Framework Depth Mapping.fx", 1);

	if (FAILED(hr))
		return hr;

	hr = _pWhiteLightShadowMapRP->SetupRTVAndSRV(_pd3dDevice, "DepthMap");

	if (FAILED(hr))
		return hr;

	_pWhiteLightShadowMapRP->SetClearColour(1.0f, 1.0f, 1.0f, 1.0f);

#pragma endregion

#pragma region Setup Red Light Shadow Mapping Render Process

	_pRedLightShadowMapRP = new RenderToTextureProcess((float)_pRenderWidth, (float)_pRenderHeight);
	hr = _pRedLightShadowMapRP->LoadShaderFilesAndInputLayouts(_pd3dDevice, L"DX11 Framework Depth Mapping.fx", L"DX11 Framework Depth Mapping.fx", 1);

	if (FAILED(hr))
		return hr;

	hr = _pRedLightShadowMapRP->SetupRTVAndSRV(_pd3dDevice, "DepthMap");

	if (FAILED(hr))
		return hr;

	_pRedLightShadowMapRP->SetClearColour(1.0f, 1.0f, 1.0f, 1.0f);

#pragma endregion

#pragma region Setup Green Light Shadow Mapping Render Process

	_pGreenLightShadowMapRP = new RenderToTextureProcess((float)_pRenderWidth, (float)_pRenderHeight);
	hr = _pGreenLightShadowMapRP->LoadShaderFilesAndInputLayouts(_pd3dDevice, L"DX11 Framework Depth Mapping.fx", L"DX11 Framework Depth Mapping.fx", 1);

	if (FAILED(hr))
		return hr;

	hr = _pGreenLightShadowMapRP->SetupRTVAndSRV(_pd3dDevice, "DepthMap");

	if (FAILED(hr))
		return hr;

	_pGreenLightShadowMapRP->SetClearColour(1.0f, 1.0f, 1.0f, 1.0f);

#pragma endregion

#pragma region Setup Red Light Shadow Mapping Render Process

	_pBlueLightShadowMapRP = new RenderToTextureProcess((float)_pRenderWidth, (float)_pRenderHeight);
	hr = _pBlueLightShadowMapRP->LoadShaderFilesAndInputLayouts(_pd3dDevice, L"DX11 Framework Depth Mapping.fx", L"DX11 Framework Depth Mapping.fx", 1);

	if (FAILED(hr))
		return hr;

	hr = _pBlueLightShadowMapRP->SetupRTVAndSRV(_pd3dDevice, "DepthMap");

	if (FAILED(hr))
		return hr;

	_pBlueLightShadowMapRP->SetClearColour(1.0f, 1.0f, 1.0f, 1.0f);

#pragma endregion


#pragma region Setup Base Scene Normal + Deferred Render Process

	_pBaseSceneRP = new RenderToTextureProcess((float)_pRenderWidth, (float)_pRenderHeight);
	hr = _pBaseSceneRP->LoadShaderFilesAndInputLayouts(_pd3dDevice, L"DX11 Framework Parallax SS Diffuse Mapping.fx", L"DX11 Framework Parallax SS Diffuse Mapping.fx", 2);
	hr = _pBaseSceneRP->LoadShaderFilesAndInputLayouts(_pd3dDevice, L"DX11 Framework Parallax SS Deferred.fx", L"DX11 Framework Parallax SS Deferred.fx", 2);

	if (FAILED(hr))
		return hr;

	hr = _pBaseSceneRP->SetupRTVAndSRV(_pd3dDevice, "ColourMap");
	hr = _pBaseSceneRP->SetupRTVAndSRV(_pd3dDevice, "NormalMap");
	hr = _pBaseSceneRP->SetupRTVAndSRV(_pd3dDevice, "TexCoordOffsetMap");
	hr = _pBaseSceneRP->SetupRTVAndSRV(_pd3dDevice, "PositionMap");
	hr = _pBaseSceneRP->SetupRTVAndSRV(_pd3dDevice, "TangentMap");
	hr = _pBaseSceneRP->SetupRTVAndSRV(_pd3dDevice, "BiNormalMap");
	hr = _pBaseSceneRP->SetupRTVAndSRV(_pd3dDevice, "WorldNormalMap");

	if (FAILED(hr))
		return hr;

	_pBaseSceneRP->AddSamplerState(_pSamplerLinear);
	_pBaseSceneRP->AddSamplerState(_pSamplerClamp);

	_pBaseSceneRP->AddShaderResource(_pWhiteLightShadowMapRP->GetDepthMapResourceView());
	_pBaseSceneRP->AddShaderResource(_pRedLightShadowMapRP->GetDepthMapResourceView());
	_pBaseSceneRP->AddShaderResource(_pGreenLightShadowMapRP->GetDepthMapResourceView());
	_pBaseSceneRP->AddShaderResource(_pBlueLightShadowMapRP->GetDepthMapResourceView());

	_pBaseSceneRP->SetClearColour(0.0f, 0.0f, 0.0f, 1.0f);

	_pCurrentRenderProcess = _pBaseSceneRP;

#pragma endregion

#pragma region Setup Basic Scene Render Process

	_pBasicRP = new RenderToTextureProcess((float)_pRenderWidth, (float)_pRenderHeight);
	hr = _pBasicRP->LoadShaderFilesAndInputLayouts(_pd3dDevice, L"DX11 Framework Basic.fx", L"DX11 Framework Basic.fx", 3);

	if (FAILED(hr))
		return hr;

	hr = _pBasicRP->SetupRTVAndSRV(_pd3dDevice, "OutputText");

	if (FAILED(hr))
		return hr;

	_pBasicRP->AddSamplerState(_pSamplerLinear);

	_pBasicRP->SetClearColour(0.0f, 0.0f, 0.0f, 1.0f);

#pragma endregion

#pragma region Setup Vertex Scene Render Process

	_pVertexRP = new RenderToTextureProcess((float)_pRenderWidth, (float)_pRenderHeight);
	hr = _pVertexRP->LoadShaderFilesAndInputLayouts(_pd3dDevice, L"DX11 Framework Vertex.fx", L"DX11 Framework Vertex.fx", 2);

	if (FAILED(hr))
		return hr;

	hr = _pVertexRP->SetupRTVAndSRV(_pd3dDevice, "OutputText");

	if (FAILED(hr))
		return hr;

	_pVertexRP->AddSamplerState(_pSamplerLinear);

	_pVertexRP->SetClearColour(0.0f, 0.0f, 0.0f, 1.0f);

#pragma endregion

#pragma region Setup Pixel Scene Render Process

	_pPixelRP= new RenderToTextureProcess((float)_pRenderWidth, (float)_pRenderHeight);
	hr = _pPixelRP->LoadShaderFilesAndInputLayouts(_pd3dDevice, L"DX11 Framework Pixel SS.fx", L"DX11 Framework Pixel SS.fx", 2);

	if (FAILED(hr))
		return hr;

	hr = _pPixelRP->SetupRTVAndSRV(_pd3dDevice, "OutputText");

	if (FAILED(hr))
		return hr;

	_pPixelRP->AddSamplerState(_pSamplerLinear);

	_pPixelRP->AddShaderResource(_pWhiteLightShadowMapRP->GetDepthMapResourceView());
	_pPixelRP->AddShaderResource(_pRedLightShadowMapRP->GetDepthMapResourceView());
	_pPixelRP->AddShaderResource(_pGreenLightShadowMapRP->GetDepthMapResourceView());
	_pPixelRP->AddShaderResource(_pBlueLightShadowMapRP->GetDepthMapResourceView());

	_pPixelRP->SetClearColour(0.0f, 0.0f, 0.0f, 1.0f);

#pragma endregion

#pragma region Setup Deferred Lighting Render Process

	_pDeferredRenderRP = new RenderToTextureProcess((float)_pRenderWidth, (float)_pRenderHeight);
	hr = _pDeferredRenderRP->LoadShaderFilesAndInputLayouts(_pd3dDevice, L"DX11 Framework Deferred Lighting.fx", L"DX11 Framework Deferred Lighting.fx", 2);

	if (FAILED(hr))
		return hr;

	hr = _pDeferredRenderRP->SetupRTVAndSRV(_pd3dDevice, "OutputText");

	if (FAILED(hr))
		return hr;

	hr = _pDeferredRenderRP->SetupRenderQuad(_pd3dDevice);

	if (FAILED(hr))
		return hr;

	_pDeferredRenderRP->AddSamplerState(_pSamplerLinear);
	_pDeferredRenderRP->AddSamplerState(_pSamplerClamp);

	_pDeferredRenderRP->AddShaderResource(_pWhiteLightShadowMapRP->GetDepthMapResourceView());
	_pDeferredRenderRP->AddShaderResource(_pRedLightShadowMapRP->GetDepthMapResourceView());
	_pDeferredRenderRP->AddShaderResource(_pGreenLightShadowMapRP->GetDepthMapResourceView());
	_pDeferredRenderRP->AddShaderResource(_pBlueLightShadowMapRP->GetDepthMapResourceView());

	_pDeferredRenderRP->AddShaderResource(_pCurrentRenderProcess->GetShaderTargetTexture("ColourMap"));
	_pDeferredRenderRP->AddShaderResource(_pCurrentRenderProcess->GetShaderTargetTexture("NormalMap"));
	_pDeferredRenderRP->AddShaderResource(_pCurrentRenderProcess->GetShaderTargetTexture("TexCoordOffsetMap"));
	_pDeferredRenderRP->AddShaderResource(_pCurrentRenderProcess->GetShaderTargetTexture("PositionMap"));
	_pDeferredRenderRP->AddShaderResource(_pCurrentRenderProcess->GetShaderTargetTexture("TangentMap"));
	_pDeferredRenderRP->AddShaderResource(_pCurrentRenderProcess->GetShaderTargetTexture("WorldNormalMap"));
	_pDeferredRenderRP->AddShaderResource(_pCurrentRenderProcess->GetShaderTargetTexture("BiNormalMap"));

	_pDeferredRenderRP->SetClearColour(0.0f, 0.0f, 0.0f, 1.0f);

#pragma endregion

#pragma region Setup Horizontal Blur Render Process

	_pHBlurRP = new RenderToTextureProcess((float)_pRenderWidth, (float)_pRenderHeight);
	hr = _pHBlurRP->LoadShaderFilesAndInputLayouts(_pd3dDevice, L"DX11 Framework Horizontal Blur.fx", L"DX11 Framework Horizontal Blur.fx", 3);

	if (FAILED(hr))
		return hr;

	hr = _pHBlurRP->SetupRTVAndSRV(_pd3dDevice, "OutputText");

	if (FAILED(hr))
		return hr;

	hr = _pHBlurRP->SetupRenderQuad(_pd3dDevice);

	if (FAILED(hr))
		return hr;

	_pHBlurRP->AddSamplerState(_pSamplerLinear);
	_pHBlurRP->AddShaderResource(_pBaseSceneRP->GetShaderTargetTexture("ColourMap"));

	_pHBlurRP->SetClearColour(0.0f, 0.0f, 0.0f, 1.0f);

#pragma endregion

#pragma region Setup Vertical Blur Render Process

	_pVBlurRP = new RenderToTextureProcess((float)_pRenderWidth, (float)_pRenderHeight);
	hr = _pVBlurRP->LoadShaderFilesAndInputLayouts(_pd3dDevice, L"DX11 Framework Vertical Blur.fx", L"DX11 Framework Vertical Blur.fx", 3);

	if (FAILED(hr))
		return hr;

	hr = _pVBlurRP->SetupRTVAndSRV(_pd3dDevice, "OutputText");

	if (FAILED(hr))
		return hr;

	hr = _pVBlurRP->SetupRenderQuad(_pd3dDevice);

	if (FAILED(hr))
		return hr;

	_pVBlurRP->AddSamplerState(_pSamplerLinear);
	_pVBlurRP->AddShaderResource(_pHBlurRP->GetShaderTargetTexture("OutputText"));

	_pVBlurRP->SetClearColour(0.0f, 0.0f, 0.0f, 1.0f);

#pragma endregion

#pragma region Setup Basic Depth Map Render Process

	_pBasicDepthMapRP = new RenderToTextureProcess((float)_pRenderWidth, (float)_pRenderHeight);
	hr = _pBasicDepthMapRP->LoadShaderFilesAndInputLayouts(_pd3dDevice, L"DX11 Framework Basic Depth Mapping.fx", L"DX11 Framework Basic Depth Mapping.fx", 3);

	if (FAILED(hr))
		return hr;

	hr = _pBasicDepthMapRP->SetupRTVAndSRV(_pd3dDevice, "DepthMap");

	if (FAILED(hr))
		return hr;

	_pBasicDepthMapRP->SetClearColour(0.0f, 0.0f, 0.0f, 1.0f);

#pragma endregion

#pragma region Setup DOF H Blur Render Process

	_pDOFHBlurRP = new RenderToTextureProcess((float)_pRenderWidth, (float)_pRenderHeight);
	hr = _pDOFHBlurRP->LoadShaderFilesAndInputLayouts(_pd3dDevice, L"DX11 Framework DOF Hor Blur.fx", L"DX11 Framework DOF Hor Blur.fx", 3);

	if (FAILED(hr))
		return hr;

	hr = _pDOFHBlurRP->SetupRTVAndSRV(_pd3dDevice, "OutputText");

	if (FAILED(hr))
		return hr;

	hr = _pDOFHBlurRP->SetupRenderQuad(_pd3dDevice);

	if (FAILED(hr))
		return hr;

	_pDOFHBlurRP->AddSamplerState(_pSamplerLinear);
	_pDOFHBlurRP->AddSamplerState(_pSamplerClamp);
	_pDOFHBlurRP->AddShaderResource(_pVBlurRP->GetShaderTargetTexture("OutputText"));
	_pDOFHBlurRP->AddShaderResource(_pBasicDepthMapRP->GetShaderTargetTexture("DepthMap"));

	_pDOFHBlurRP->SetClearColour(0.0f, 0.0f, 0.0f, 1.0f);

#pragma endregion

#pragma region Setup DOF V Blur Render Process

	_pDOFVBlurRP = new RenderToTextureProcess((float)_pRenderWidth, (float)_pRenderHeight);
	hr = _pDOFVBlurRP->LoadShaderFilesAndInputLayouts(_pd3dDevice, L"DX11 Framework DOF Vert Blur.fx", L"DX11 Framework DOF Vert Blur.fx", 3);

	if (FAILED(hr))
		return hr;

	hr = _pDOFVBlurRP->SetupRTVAndSRV(_pd3dDevice, "OutputText");

	if (FAILED(hr))
		return hr;

	hr = _pDOFVBlurRP->SetupRenderQuad(_pd3dDevice);

	if (FAILED(hr))
		return hr;

	_pDOFVBlurRP->AddSamplerState(_pSamplerLinear);
	_pDOFVBlurRP->AddSamplerState(_pSamplerClamp);
	_pDOFVBlurRP->AddShaderResource(_pDOFHBlurRP->GetShaderTargetTexture("OutputText"));
	_pDOFVBlurRP->AddShaderResource(_pBasicDepthMapRP->GetShaderTargetTexture("DepthMap"));

	_pDOFVBlurRP->SetClearColour(0.0f, 0.0f, 0.0f, 1.0f);

#pragma endregion

#pragma region Setup Final Pass Render Process

	_pFinalPassRP = new RenderToTextureProcess((float)_pRenderWidth, (float)_pRenderHeight);
	hr = _pFinalPassRP->LoadShaderFilesAndInputLayouts(_pd3dDevice, L"DX11 Framework Render To Texture.fx", L"DX11 Framework Render To Texture.fx", 3);

	if (FAILED(hr))
		return hr;

	hr = _pFinalPassRP->SetupRenderQuad(_pd3dDevice);

	if (FAILED(hr))
		return hr;

	_pFinalPassRP->AddSamplerState(_pSamplerLinear);
	_pFinalPassRP->AddShaderResource(_pVBlurRP->GetShaderTargetTexture("OutputText"));

	_pFinalPassRP->SetClearColour(0.0f, 0.0f, 1.0f, 1.0f);

#pragma endregion

	return hr;
}

void Application::BuildRandomVectorMap()
{
	ID3D11ShaderResourceView* shaderResourceView;

	CreateDDSTextureFromFile(_pd3dDevice, L"Resources\\randomVec.dds", nullptr, &shaderResourceView);

	_pSSAOAmbientMapRP->AddShaderResource(shaderResourceView);
}

void Application::Cleanup()
{

#pragma region DirectX Initialisation Variables

	if (_pSwapChain) _pSwapChain->Release();
	if (_pImmediateContext) _pImmediateContext->ClearState();
	if (_pImmediateContext) _pImmediateContext->Release();
	if (_pd3dDevice) _pd3dDevice->Release();

#pragma endregion

#pragma region Rasterizer States

	if (_pDSLessEqual) _pDSLessEqual->Release();
	if (_pRSCullNone) _pRSCullNone->Release();

	if (_pCCWcullMode) _pCCWcullMode->Release();
	if (_pCWcullMode) _pCWcullMode->Release();

#pragma endregion

#pragma region Buffer Variables

	if (_pVertexBuffer) _pVertexBuffer->Release();
	if (_pIndexBuffer) _pIndexBuffer->Release();

	if (_pPlaneVertexBuffer) _pPlaneVertexBuffer->Release();
	if (_pPlaneIndexBuffer) _pPlaneIndexBuffer->Release();

	if (_pConstantBuffer) _pConstantBuffer->Release();
	if (_pSMConstantBuffer) _pSMConstantBuffer->Release();
	if (_pSSAOConstantBuffer) _pSSAOConstantBuffer->Release();

#pragma endregion

#pragma region Light Variables

	vector<SceneLight*> _pLights;

	for (auto light : _pLights)
	{
		if (light)
		{
			delete light;
			light = nullptr;
		}
	}

#pragma endregion

#pragma region Game Objects

	for (auto gameObject : _pGameObjects)
	{
		if (gameObject)
		{
			delete gameObject;
			gameObject = nullptr;
		}
	}

#pragma endregion

#pragma region Camera

	if (_pCamera)
	{
		delete _pCamera;
		_pCamera = nullptr;
	}

#pragma endregion

}

Light Application::GetLightFromSceneLight(SceneLight* light)
{
	Light newLight;

	XMMATRIX lightView = XMLoadFloat4x4(&light->GetView());
	XMMATRIX lightProjection = XMLoadFloat4x4(&light->GetProjection());

	newLight.View =					XMMatrixTranspose(lightView);
	newLight.Projection =			XMMatrixTranspose(lightProjection);
	newLight.AmbientLight =			light->GetAmbientLight();
	newLight.DiffuseLight =			light->GetDiffuseLight();
	newLight.SpecularLight =		light->GetSpecularLight();
	newLight.SpecularPower =		light->GetSpecularPower();
	newLight.LightVecW =			light->GetLightVecW();
	newLight.paddingLightAmount =	light->GetPaddingLightAmount();
	newLight.lightOn =				light->GetLightOn();

	return newLight;
}

void Application::UpdateLightsControls(float deltaTime)
{

#pragma region Toggle Current Light

	if (GetAsyncKeyState('L') && !_pToggleChangeCurrentLight)
	{
		_pToggleChangeCurrentLight = true;

		if (_pCurrentLightIndex == _pLights.size()-1)
		{
			_pCurrentLightIndex = 0;
		}
		else
		{
			_pCurrentLightIndex += 1;
		}
	}

	if (!GetAsyncKeyState('L') && _pToggleChangeCurrentLight)
	{
		_pToggleChangeCurrentLight = false;
	}

	_pLights.at(_pCurrentLightIndex)->HandleLightControls(deltaTime);

#pragma endregion

}

void Application::Update(float deltaTime, float FPS)
{
	wstringstream wss;
	wss << FPS;

	SetWindowText(_hWnd, wss.str().c_str());

	// Update our time
	static float timeSinceStart = 0.0f;
	static DWORD dwTimeStart = 0;

	DWORD dwTimeCur = GetTickCount();

	if (dwTimeStart == 0)
		dwTimeStart = dwTimeCur;

	timeSinceStart = (dwTimeCur - dwTimeStart) / 1000.0f;

#pragma region Update Camera

	_pCamera->Update();

#pragma endregion

#pragma region Update Game Objects

	// Update objects
	for (auto gameObject : _pGameObjects)
	{
		gameObject->Update(timeSinceStart, deltaTime);
	}

#pragma endregion

#pragma region Update Lights

	for (SceneLight* light : _pLights)
	{
		light->UpdateLightCube(timeSinceStart, deltaTime);
		light->UpdateLight((float)_pRenderWidth, (float)_pRenderHeight);
	}

#pragma endregion

}

void Application::Draw()
{

#pragma region Initialise Draw Variables

	SMConstantBuffer smCB;
	SSAOConstantBuffer ssaoCB;

	XMFLOAT4X4 viewAsFloats;
	XMFLOAT4X4 projectionAsFloats;
	XMFLOAT4X4 shadowTransformAsFloats;
	XMMATRIX view;
	XMMATRIX projection;
	XMMATRIX shadowTransform;

#pragma endregion

#pragma region Initialise Constant Buffer

	ConstantBuffer cb;

	cb.World = XMMatrixTranspose(_pGameObjects.at(0)->GetWorldMatrix());

	viewAsFloats = _pCamera->GetView();
	projectionAsFloats = _pCamera->GetProjection();

	view = XMLoadFloat4x4(&viewAsFloats);
	projection = XMLoadFloat4x4(&projectionAsFloats);

	cb.View = XMMatrixTranspose(view);
	cb.Projection = XMMatrixTranspose(projection);

	cb.surface.AmbientMtrl = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	cb.surface.DiffuseMtrl = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	cb.surface.SpecularMtrl = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);

	cb.lights[0] = GetLightFromSceneLight(_pLights.at(0));
	cb.lights[1] = GetLightFromSceneLight(_pLights.at(1));
	cb.lights[2] = GetLightFromSceneLight(_pLights.at(2));
	cb.lights[3] = GetLightFromSceneLight(_pLights.at(3));

	cb.EyePosW = _pCamera->GetPosition();
	cb.HasTexture = 0.0f;
	cb.HasNormalMap = 0.0f;
	cb.HasHeightMap = 0.0f;
	cb.shadowsOn = _pShadowsOn;
	cb.screenWidth = _pRenderWidth;
	cb.screenHeight = _pRenderHeight;
	cb.blurIntensity = _pBlurIntensity;

#pragma endregion

//#pragma region SSAO Normal Map Pass
//
//	SetupSSAOConstantBuffer(&ssaoCB, &cb);
//
//	// Switch Render Target to the SSAO Normal Depth Map Render Target
//	_pSSAONormalMapRP->SetupRenderProcess(_pImmediateContext, _pSSAOConstantBuffer, true);
//	_pSSAONormalMapRP->RenderGameObjects(_pImmediateContext, _pGameObjects, _pSSAOConstantBuffer, &ssaoCB);
//
//#pragma endregion
//
//#pragma region SSAO Depth Map Pass
//
//	smCB.View = cb.View;
//	smCB.Projection = cb.Projection;
//
//	_pSSAODepthMapRP->RenderSceneDepthMap(_pImmediateContext, _pGameObjects, _pSMConstantBuffer, &smCB);
//
//#pragma endregion
//
//#pragma region SSAO Ambient Map Pass
//
//	_pSSAOAmbientMapRP->SetupRenderProcess(_pImmediateContext, _pSSAOConstantBuffer, false);
//	_pSSAOAmbientMapRP->RenderToTexture(_pImmediateContext, _pSSAOConstantBuffer, &ssaoCB);
//
//#pragma endregion

	if (_pShadowsOn)
	{
		if (_pLights.at(0)->GetLightOn())
		{
		#pragma region Shadow Mapping Pass - White Light

			viewAsFloats = _pLights.at(0)->GetView();
			projectionAsFloats = _pLights.at(0)->GetProjection();
			shadowTransformAsFloats = _pLights.at(0)->GetShadowTransform();

			view = XMLoadFloat4x4(&viewAsFloats);
			projection = XMLoadFloat4x4(&projectionAsFloats);
			shadowTransform = XMLoadFloat4x4(&projectionAsFloats);

			smCB.View = XMMatrixTranspose(view);
			smCB.Projection = XMMatrixTranspose(projection);

			// Switch Render Target to the White Light Shadow Map Render Target
			//_pWhiteLightShadowMapRP->SetupRenderProcess(_pImmediateContext, _pSMConstantBuffer, true);
			_pWhiteLightShadowMapRP->RenderSceneDepthMap(_pImmediateContext, _pGameObjects, _pSMConstantBuffer, &smCB);

		#pragma endregion
		}

		if (_pLights.at(1)->GetLightOn())
		{
		#pragma region Shadow Mapping Pass - Red Light

			viewAsFloats = _pLights.at(1)->GetView();
			projectionAsFloats = _pLights.at(1)->GetProjection();
			shadowTransformAsFloats = _pLights.at(1)->GetShadowTransform();

			view = XMLoadFloat4x4(&viewAsFloats);
			projection = XMLoadFloat4x4(&projectionAsFloats);
			shadowTransform = XMLoadFloat4x4(&projectionAsFloats);

			smCB.View = XMMatrixTranspose(view);
			smCB.Projection = XMMatrixTranspose(projection);

			// Switch Render Target to the Red Light Shadow Map Render Target
			//_pRedLightShadowMapRP->SetupRenderProcess(_pImmediateContext, _pSMConstantBuffer, true);
			_pRedLightShadowMapRP->RenderSceneDepthMap(_pImmediateContext, _pGameObjects, _pSMConstantBuffer, &smCB);

		#pragma endregion
		}

		if (_pLights.at(2)->GetLightOn())
		{
		#pragma region Shadow Mapping Pass - Green Light

			viewAsFloats = _pLights.at(2)->GetView();
			projectionAsFloats = _pLights.at(2)->GetProjection();
			shadowTransformAsFloats = _pLights.at(2)->GetShadowTransform();

			view = XMLoadFloat4x4(&viewAsFloats);
			projection = XMLoadFloat4x4(&projectionAsFloats);
			shadowTransform = XMLoadFloat4x4(&projectionAsFloats);

			smCB.View = XMMatrixTranspose(view);
			smCB.Projection = XMMatrixTranspose(projection);

			// Switch Render Target to the White Light Shadow Map Render Target
			//_pWhiteLightShadowMapRP->SetupRenderProcess(_pImmediateContext, _pSMConstantBuffer, true);
			_pGreenLightShadowMapRP->RenderSceneDepthMap(_pImmediateContext, _pGameObjects, _pSMConstantBuffer, &smCB);

		#pragma endregion
		}

		if (_pLights.at(3)->GetLightOn())
		{
		#pragma region Shadow Mapping Pass - Blue Light

			viewAsFloats = _pLights.at(3)->GetView();
			projectionAsFloats = _pLights.at(3)->GetProjection();
			shadowTransformAsFloats = _pLights.at(3)->GetShadowTransform();

			view = XMLoadFloat4x4(&viewAsFloats);
			projection = XMLoadFloat4x4(&projectionAsFloats);
			shadowTransform = XMLoadFloat4x4(&projectionAsFloats);

			smCB.View = XMMatrixTranspose(view);
			smCB.Projection = XMMatrixTranspose(projection);

			// Switch Render Target to the Red Light Shadow Map Render Target
			//_pRedLightShadowMapRP->SetupRenderProcess(_pImmediateContext, _pSMConstantBuffer, true);
			_pBlueLightShadowMapRP->RenderSceneDepthMap(_pImmediateContext, _pGameObjects, _pSMConstantBuffer, &smCB);

		#pragma endregion
		}
	}

#pragma region Base Scene Normal + Deferred Pass

	_pCurrentRenderProcess->SetupRenderProcess(_pImmediateContext, _pConstantBuffer, true);
	_pCurrentRenderProcess->RenderGameObjects(_pImmediateContext, _pGameObjects, _pConstantBuffer, &cb);

#pragma endregion

	if (_pDeferred)
	{
	#pragma region Deferred Lighting Pass

		_pDeferredRenderRP->SetupRenderProcess(_pImmediateContext, _pConstantBuffer, false);
		_pDeferredRenderRP->RenderToTexture(_pImmediateContext, _pConstantBuffer, &cb);

	#pragma endregion
	}
	
#pragma region Blur Passes

	if (_pBlurPassCount != 0)
	{
		for (int i = 0; i < _pBlurPassCount; i++)
		{
	#pragma region Post Process Horizontal Blur Pass

			_pHBlurRP->SetupRenderProcess(_pImmediateContext, _pConstantBuffer, false);
			_pHBlurRP->RenderToTexture(_pImmediateContext, _pConstantBuffer, &cb);

	#pragma endregion

	#pragma region Post Process Vertical Blur Pass

			_pVBlurRP->SetupRenderProcess(_pImmediateContext, _pConstantBuffer, false);
			_pVBlurRP->RenderToTexture(_pImmediateContext, _pConstantBuffer, &cb);

	#pragma endregion

			if (i == 0)
			{
				_pHBlurRP->RemoveShaderResources();
				_pHBlurRP->AddShaderResource(_pVBlurRP->GetShaderTargetTexture("OutputText"));
			}
		}

		_pHBlurRP->RemoveShaderResources();
		_pHBlurRP->AddShaderResource(_pCurrentRenderProcess->GetShaderTargetTexture("ColourMap"));
	}
	else
	{
	#pragma region Post Process Horizontal Blur Pass

		_pHBlurRP->SetupRenderProcess(_pImmediateContext, _pConstantBuffer, false);
		_pHBlurRP->RenderToTexture(_pImmediateContext, _pConstantBuffer, &cb);

	#pragma endregion

	#pragma region Post Process Vertical Blur Pass

		_pVBlurRP->SetupRenderProcess(_pImmediateContext, _pConstantBuffer, false);
		_pVBlurRP->RenderToTexture(_pImmediateContext, _pConstantBuffer, &cb);

	#pragma endregion
	}


#pragma endregion

#pragma region Basic Depth Map Pass

	_pBasicDepthMapRP->SetupRenderProcess(_pImmediateContext, _pConstantBuffer, true);
	_pBasicDepthMapRP->RenderGameObjects(_pImmediateContext, _pGameObjects, _pConstantBuffer, &cb);

#pragma endregion

	if (_pDOFBlurActive)
	{
	#pragma region Post Process DOF Horizontol Blur Pass

		_pDOFHBlurRP->SetupRenderProcess(_pImmediateContext, _pConstantBuffer, false);
		_pDOFHBlurRP->RenderToTexture(_pImmediateContext, _pConstantBuffer, &cb);

	#pragma endregion

	#pragma region Post Process DOF Vertical Blur Pass

		_pDOFVBlurRP->SetupRenderProcess(_pImmediateContext, _pConstantBuffer, false);
		_pDOFVBlurRP->RenderToTexture(_pImmediateContext, _pConstantBuffer, &cb);

	#pragma endregion

	}

#pragma region Post Process Final Pass

	_pFinalPassRP->SetupRenderProcess(_pImmediateContext, _pConstantBuffer, false);
	_pFinalPassRP->RenderToTexture(_pImmediateContext, _pConstantBuffer, &cb);
	
#pragma endregion

    // Present our back buffer to our front buffer
    _pSwapChain->Present(0, 0);
}

void Application::SetupSSAOConstantBuffer(SSAOConstantBuffer* ssaoCB, ConstantBuffer* cb)
{
	ssaoCB->View = cb->View;
	ssaoCB->Projection = cb->Projection;

	// Setup View to Texture Matrix
	static const XMMATRIX T(
		0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.0f, 1.0f);

	ssaoCB->ViewToTex = XMMatrixMultiply(cb->Projection, T);

	// Setup Offset Vectors
	ssaoCB->OffsetVectors[0] = XMFLOAT4(+1.0f, +1.0f, +1.0f, 0.0f);
	ssaoCB->OffsetVectors[1] = XMFLOAT4(-1.0f, -1.0f, -1.0f, 0.0f);

	ssaoCB->OffsetVectors[2] = XMFLOAT4(-1.0f, +1.0f, +1.0f, 0.0f);
	ssaoCB->OffsetVectors[3] = XMFLOAT4(+1.0f, -1.0f, -1.0f, 0.0f);

	ssaoCB->OffsetVectors[4] = XMFLOAT4(+1.0f, +1.0f, -1.0f, 0.0f);
	ssaoCB->OffsetVectors[5] = XMFLOAT4(-1.0f, -1.0f, +1.0f, 0.0f);

	ssaoCB->OffsetVectors[6] = XMFLOAT4(-1.0f, +1.0f, -1.0f, 0.0f);
	ssaoCB->OffsetVectors[7] = XMFLOAT4(+1.0f, -1.0f, +1.0f, 0.0f);

	ssaoCB->OffsetVectors[8] = XMFLOAT4(-1.0f, 0.0f, 0.0f, 0.0f);
	ssaoCB->OffsetVectors[9] = XMFLOAT4(+1.0f, 0.0f, 0.0f, 0.0f);

	ssaoCB->OffsetVectors[10] = XMFLOAT4(0.0f, -1.0f, 0.0f, 0.0f);
	ssaoCB->OffsetVectors[11] = XMFLOAT4(0.0f, +1.0f, 0.0f, 0.0f);

	ssaoCB->OffsetVectors[12] = XMFLOAT4(0.0f, 0.0f, -1.0f, 0.0f);
	ssaoCB->OffsetVectors[13] = XMFLOAT4(0.0f, 0.0f, +1.0f, 0.0f);

	for (int i = 0; i < 14; i++)
	{
		float randomLength = 0.25f + ((float)(rand()) / (float)RAND_MAX) * (1.0f - 0.25f);
		XMVECTOR vector = randomLength * XMVector4Normalize(XMLoadFloat4(&ssaoCB->OffsetVectors[i]));
		XMStoreFloat4(&ssaoCB->OffsetVectors[i], vector);
	}

	// Setup View Frustum Corner Vectors
	float aspect = (float)_pRenderWidth / (float)_pRenderHeight;

	float halfHeight = _pCamera->GetFarDepth() * tanf(0.5f * _pCamera->GetFieldOfView());
	float halfWidth = aspect * halfHeight;

	ssaoCB->FrustumCorners[0] = XMFLOAT4(-halfWidth, -halfHeight, _pCamera->GetFarDepth(), 0.0f);
	ssaoCB->FrustumCorners[1] = XMFLOAT4(-halfWidth, +halfHeight, _pCamera->GetFarDepth(), 0.0f);
	ssaoCB->FrustumCorners[2] = XMFLOAT4(+halfWidth, +halfHeight, _pCamera->GetFarDepth(), 0.0f);
	ssaoCB->FrustumCorners[3] = XMFLOAT4(+halfWidth, -halfHeight, _pCamera->GetFarDepth(), 0.0f);

	//ssaoCB->OffsetVectors[0] = XMFLOAT4(0.0f, 0.5f, 0.0f, 0.0f);
}