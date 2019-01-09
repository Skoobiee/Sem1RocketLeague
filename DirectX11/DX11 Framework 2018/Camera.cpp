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
	
	xThirdPerson = 0.0f;
	yThirdPerson = 1.0f;
	zThirdPerson = -6.5f;

	//xPosCar = 5.0f;
}

Camera::~Camera()
{

}

XMFLOAT4X4 Camera::Update()
{
	//xThirdPerson = xPosCar + 10.0f;

	if (GetAsyncKeyState('1')) //static forward
	{
		XMVECTOR Eye = XMVectorSet(2.0f, -2.0f, -5.0f, 0.0f);
		XMVECTOR At = XMVectorSet(1.0f, -1.0f, 5.0f, 0.0f);
		XMVECTOR Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

		XMStoreFloat4x4(&_view, XMMatrixLookAtLH(Eye, At, Up));
	}
	else if (GetAsyncKeyState('2')) //static top down
	{
		XMVECTOR Eye = XMVectorSet(0.0f, 12.0f, -1.0f, 0.0f);
		XMVECTOR At = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
		XMVECTOR Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

		XMStoreFloat4x4(&_view, XMMatrixLookAtLH(Eye, At, Up));
	}
	else if (GetAsyncKeyState('3')) //first person
	{
		XMVECTOR Eye = XMVectorSet(2.0f, 2.0f, -5.0f, 0.0f);
		XMVECTOR At = XMVectorSet(1.0f, -1.0f, 3.0f, 0.0f);
		XMVECTOR Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

		XMStoreFloat4x4(&_view, XMMatrixLookAtLH(Eye, At, Up));
	}
	else if (GetAsyncKeyState('4')) //third person
	{
		//Application::xPosCar = 1.0f;
		//Application::GetxPosCar;
		
		XMVECTOR Eye = XMVectorSet(xThirdPerson, yThirdPerson, zThirdPerson, 0.0f);
		XMVECTOR At = XMVectorSet(0.0f, -1.0f, 3.0f, 0.0f);
		XMVECTOR Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

		XMStoreFloat4x4(&_view, XMMatrixLookAtLH(Eye, At, Up));
	}

	return _projection;
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