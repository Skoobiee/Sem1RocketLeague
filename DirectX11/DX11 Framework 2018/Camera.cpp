#include "Camera.h"


Camera::Camera()
{
	// Initialize the view matrix
	//XMVECTOR Eye = XMVectorSet(3.0f, 0.0f, -3.0f, 0.0f);
	XMVECTOR Eye = XMVectorSet(3.0f, 0.0f, -3.0f, 0.0f);
	XMVECTOR At = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	XMVECTOR Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	XMStoreFloat4x4(&_view, XMMatrixLookAtLH(Eye, At, Up)); //creates a camera that always looks at the same thing, stays still, can change to XMMatrixLookTo

	_WindowWidth = 1920;
	_WindowHeight = 1061;

	// Initialize the projection matrix
	XMStoreFloat4x4(&_projection, XMMatrixPerspectiveFovLH(XM_PIDIV2, _WindowWidth / (FLOAT)_WindowHeight, 0.01f, 100.0f)); //changes the near clipping and far clipping plane, increase to increase view space

	//eyePosW = XMFLOAT3(3.0f, 0.0f, -3.0f);
	/*xPosCar = 5.0f;
	yPosCar = -700.0f;
	zPosCar = -1.0f;*/
	
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

	/*&TargetPosition.x + TrackingOffset.x = xThirdPerson;
	&TargetPosition.y + TrackingOffset.y = yThirdPerson;
	&TargetPosition.z + TrackingOffset.z = zThirdPerson;*/

	thirdPerson = false;

	//xPosCar = 5.0f;
}

Camera::~Camera()
{

}

XMFLOAT4X4 Camera::Update()
{
	//xThirdPerson = xPosCar + 10.0f;
	//XMVECTOR Eye3 = XMVectorSet(TargetPosition.x + TrackingOffset.x, TargetPosition.y + TrackingOffset.y, TargetPosition.z + TrackingOffset.z, 0.0f);

	if (GetAsyncKeyState('1')) //static forward
	{
		thirdPerson = false;

		XMVECTOR Eye = XMVectorSet(2.0f, -2.0f, -5.0f, 0.0f);
		XMVECTOR At = XMVectorSet(1.0f, -1.0f, 5.0f, 0.0f);
		XMVECTOR Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

		XMStoreFloat4x4(&_view, XMMatrixLookAtLH(Eye, At, Up));
	}
	else if (GetAsyncKeyState('2')) //static top down
	{
		thirdPerson = false;

		XMVECTOR Eye = XMVectorSet(0.0f, 30.0f, -5.0f, 0.0f);
		XMVECTOR At = XMVectorSet(0.0f, -1.0f, 2.0f, 0.0f);
		XMVECTOR Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

		XMStoreFloat4x4(&_view, XMMatrixLookAtLH(Eye, At, Up));
	}
	else if (GetAsyncKeyState('3')) //first person
	{
		thirdPerson = false;



	}
	else if (GetAsyncKeyState('4')) //third person
	{	
		thirdPerson = true;
		/*XMVECTOR At = XMVectorSet(0.0f, -1.0f, 3.0f, 0.0f);
		XMVECTOR Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

		XMStoreFloat4x4(&_view, XMMatrixLookAtLH(Eye3, At, Up));*/

		//ThirdPersonCamera();
	}
	
	if (thirdPerson)
	{
		ThirdPersonCamera();
	}

	//ThirdPersonCamera();

	return _projection;
}

XMFLOAT4X4 Camera::ThirdPersonCamera()
{
	//XMVECTOR Eye3 = XMVectorSet(TargetPosition.x + TrackingOffset.x, TargetPosition.y + TrackingOffset.y, TargetPosition.z + TrackingOffset.z, 0.0f);
	XMVECTOR Eye3 = XMVectorSet(xThirdPerson, yThirdPerson, zThirdPerson, 0.0f);
	XMVECTOR At = XMVectorSet(0.0f, -1.0f, 3.0f, 0.0f);
	XMVECTOR Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	XMStoreFloat4x4(&_view, XMMatrixLookAtLH(Eye3, At, Up));

	//TargetPosition.x = 0;
	//TargetPosition.y = 0;
	//TargetPosition.z = 0;

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

float SetThirdPersonCamera(float x, float y, float z)
{
	x = 0.0f;
	y = 1.0f;
	z = -6.5f;

	return x, y, z;
}

//XMFLOAT4X4 Camera::GetThirdPersonCamera()
//{
//	return xPosCar; //, yPosCar, zPosCar
//}