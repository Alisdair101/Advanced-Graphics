#pragma once

#include <windows.h>
#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <directxmath.h>

using namespace DirectX;

class Camera
{
private:
	XMFLOAT3 _pEye;
	XMFLOAT3 _pAt;
	XMFLOAT3 _pUp;

	FLOAT _pWindowWidth;
	FLOAT _pWindowHeight;
	FLOAT _pNearDepth;
	FLOAT _pFarDepth;

	float _pCameraOrbitRadius = 0.5f;
	float _pCameraOrbitRadiusMin = 0.1f;
	float _pCameraOrbitRadiusMax = 1000.0f;
	float _pCameraOrbitAngleXZ = -90.0f;
	float _pCameraSpeed = 2.0f;

	FLOAT _pFieldOfView;

	XMFLOAT4X4 _pView;
	XMFLOAT4X4 _pProjection;

public:
	Camera(XMFLOAT3 position, XMFLOAT3 at, XMFLOAT3 up, FLOAT windowWidth, FLOAT windowHeight, FLOAT nearDepth, FLOAT farDepth);
	~Camera();

	void Update();

	XMFLOAT4X4 GetView() const { return _pView; }
	XMFLOAT4X4 GetProjection() const { return _pProjection; }

	XMFLOAT4X4 GetViewProjection() const;

	XMFLOAT3 GetPosition() const { return _pEye; }
	XMFLOAT3 GetLookAt() const { return _pAt; }
	XMFLOAT3 GetUp() const { return _pUp; }

	FLOAT GetFarDepth() { return _pFarDepth; }
	FLOAT GetNearDepth() { return _pNearDepth; }
	FLOAT GetFieldOfView() { return _pFieldOfView; }

	void SetPosition(XMFLOAT3 position) { _pEye = position; }
	void SetLookAt(XMFLOAT3 lookAt) { _pAt = lookAt; }
	void SetUp(XMFLOAT3 up) { _pUp = up; }

	void Reshape(FLOAT windowWidth, FLOAT windowHeight, FLOAT nearDepth, FLOAT farDepth);

	bool HandleControls(MSG msg);
};

