#include "Camera.h"

Camera::Camera(XMFLOAT3 position, XMFLOAT3 at, XMFLOAT3 up, FLOAT windowWidth, FLOAT windowHeight, FLOAT nearDepth, FLOAT farDepth)
	: _pEye(position), _pAt(at), _pUp(up), _pWindowWidth(windowWidth), _pWindowHeight(windowHeight), _pNearDepth(nearDepth), _pFarDepth(farDepth)
{
	_pFieldOfView = 0.25f * XM_PI;

	_pCameraOrbitRadiusMin = 0.1f;
	_pCameraOrbitRadiusMax = 1000.0f;
	_pCameraSpeed = 2.0f;
	_pCameraOrbitAngleXZ = 135;
	_pCameraOrbitRadius = -42;

	Update();
}

Camera::~Camera()
{
}

void Camera::Update()
{
	// Update camera
	float angleAroundZ = XMConvertToRadians(_pCameraOrbitAngleXZ);

	float x = _pCameraOrbitRadius * cos(angleAroundZ);
	float z = _pCameraOrbitRadius * sin(angleAroundZ);

	_pEye.x = x;
	_pEye.z = z;

    // Initialize the view matrix
	XMFLOAT4 eye = XMFLOAT4(_pEye.x, _pEye.y, _pEye.z, 1.0f);
	XMFLOAT4 at = XMFLOAT4(_pAt.x, _pAt.y, _pAt.z, 1.0f);
	XMFLOAT4 up = XMFLOAT4(_pUp.x, _pUp.y, _pUp.z, 0.0f);

	XMVECTOR EyeVector = XMLoadFloat4(&eye);
	XMVECTOR AtVector = XMLoadFloat4(&at);
	XMVECTOR UpVector = XMLoadFloat4(&up);

	XMStoreFloat4x4(&_pView, XMMatrixLookAtLH(EyeVector, AtVector, UpVector));

    // Initialize the projection matrix
	XMStoreFloat4x4(&_pProjection, XMMatrixPerspectiveFovLH(_pFieldOfView, _pWindowWidth / _pWindowHeight, _pNearDepth, _pFarDepth));
}

void Camera::Reshape(FLOAT windowWidth, FLOAT windowHeight, FLOAT nearDepth, FLOAT farDepth)
{
	_pWindowWidth = windowWidth;
	_pWindowHeight = windowHeight;
	_pNearDepth = nearDepth;
	_pFarDepth = farDepth;
}

XMFLOAT4X4 Camera::GetViewProjection() const 
{ 
	XMMATRIX view = XMLoadFloat4x4(&_pView);
	XMMATRIX projection = XMLoadFloat4x4(&_pProjection);

	XMFLOAT4X4 viewProj;

	XMStoreFloat4x4(&viewProj, view * projection);

	return viewProj;
}

bool Camera::HandleControls(MSG msg)
{
	if (GetAsyncKeyState(VK_SHIFT))
	{
		if (GetAsyncKeyState(VK_UP))
		{
			_pEye.y += 0.5f;
		}
		else if (GetAsyncKeyState(VK_DOWN))
		{
			_pEye.y -= 0.5f;
		}
	}
	else
	{
		switch (msg.wParam)
		{
		case VK_UP:
			_pCameraOrbitRadius = max(_pCameraOrbitRadiusMin, _pCameraOrbitRadius - (_pCameraSpeed * 0.2f));
			return true;
			break;

		case VK_DOWN:
			_pCameraOrbitRadius = min(_pCameraOrbitRadiusMax, _pCameraOrbitRadius + (_pCameraSpeed * 0.2f));
			return true;
			break;

		case VK_RIGHT:
			_pCameraOrbitAngleXZ -= _pCameraSpeed;
			return true;
			break;

		case VK_LEFT:
			_pCameraOrbitAngleXZ += _pCameraSpeed;
			return true;
			break;
		}
	}
}