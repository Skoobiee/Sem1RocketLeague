#include "Camera.h"


Camera::Camera()
{
	// Initialize the view matrix
	XMVECTOR Eye = XMVectorSet(3.0f, 0.0f, -3.0f, 0.0f);
	XMVECTOR At = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	XMVECTOR Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	XMStoreFloat4x4(&_view, XMMatrixLookAtLH(Eye, At, Up)); //creates a camera that always looks at the same thing, stays still, can change to XMMatrixLookTo

	_WindowWidth = 1920;
	_WindowHeight = 1061;

	// Initialize the projection matrix
	XMStoreFloat4x4(&_projection, XMMatrixPerspectiveFovLH(XM_PIDIV2, _WindowWidth / (FLOAT)_WindowHeight, 0.01f, 100.0f)); //changes the near clipping and far clipping plane, increase to increase view space
	
	/*xThirdPerson = 0.0f;
	yThirdPerson = 1.0f;
	zThirdPerson = -6.5f;*/


	//TrackingOffset.x = -5.0f;
	//TrackingOffset.y = 701.0f;
	//TrackingOffset.z = -6.0f;

	//TargetPosition.x = 2.0f;
	//TargetPosition.y = -2.0f;
	//TargetPosition.z = -5.0f;

	TrackingOffset.x = -5.0f;
	TrackingOffset.y = 701.0f;
	TrackingOffset.z = -6.0f;

	TargetPosition.x = 5.0f;
	TargetPosition.y = -700.0f;
	TargetPosition.z = -1.0f;

	xThirdPerson = TrackingOffset.x + TargetPosition.x;
	yThirdPerson = TrackingOffset.y + TargetPosition.y;
	zThirdPerson = TrackingOffset.z + TargetPosition.z;

	thirdPerson = false;
	firstPerson = false;
}

Camera::~Camera()
{

}

XMFLOAT4X4 Camera::Update()
{
	if (GetAsyncKeyState('1')) //static forward
	{
		thirdPerson = false;
		firstPerson = false;

		XMVECTOR Eye = XMVectorSet(2.0f, -2.0f, -5.0f, 0.0f);
		XMVECTOR At = XMVectorSet(1.0f, -1.0f, 5.0f, 0.0f);
		XMVECTOR Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

		XMStoreFloat4x4(&_view, XMMatrixLookAtLH(Eye, At, Up));
	}
	else if (GetAsyncKeyState('2')) //static top down
	{
		thirdPerson = false;
		firstPerson = false;

		XMVECTOR Eye = XMVectorSet(0.0f, 30.0f, -5.0f, 0.0f);
		XMVECTOR At = XMVectorSet(0.0f, -1.0f, 2.0f, 0.0f);
		XMVECTOR Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

		XMStoreFloat4x4(&_view, XMMatrixLookAtLH(Eye, At, Up));
	}
	else if (GetAsyncKeyState('3')) //first person
	{
		thirdPerson = false;
		firstPerson = true;
	}
	else if (GetAsyncKeyState('4')) //third person
	{	
		thirdPerson = true;
		firstPerson = false;
	}
	
	if (thirdPerson || firstPerson)
	{
		ThirdPersonCamera();
	}

	return _projection;
}

XMFLOAT4X4 Camera::ThirdPersonCamera()
{
	TargetPosition.x *= 0.005;
	TargetPosition.y *= 0.005;
	TargetPosition.z *= 0.005;
	TrackingOffset.x *= 0.005;
	TrackingOffset.y = 700* 0.005;
	TrackingOffset.z *= 0.005;

	if (thirdPerson)
	{
		XMVECTOR Eye = XMVectorSet(TargetPosition.x + TrackingOffset.x, TargetPosition.y + TrackingOffset.y, TargetPosition.z + TrackingOffset.z, 0.0f);
		XMVECTOR At = XMVectorSet(TargetPosition.x, 0.0f, 50.0f, 0.0f);
		XMVECTOR Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

		XMStoreFloat4x4(&_view, XMMatrixLookAtLH(Eye, At, Up));
	}
	else if (firstPerson)
	{
		XMVECTOR Eye = XMVectorSet(TargetPosition.x + TrackingOffset.x, TargetPosition.y + TrackingOffset.y, TargetPosition.z + TrackingOffset.z, 0.0f);
		XMVECTOR At = XMVectorSet(TargetPosition.x, 0.0f, 50.0f, 0.0f);
		XMVECTOR Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

		XMStoreFloat4x4(&_view, XMMatrixLookAtLH(Eye, At, Up));
	}

	return _view;
}

void Camera::SetTargetPosition(float x, float y, float z)
{
	TargetPosition.x = x;
	TargetPosition.y = y;
	TargetPosition.z = z;
}

XMFLOAT4X4 Camera::GetView()
{
	return _view;
}

XMFLOAT4X4 Camera::GetProjection()
{
	return _projection;
}