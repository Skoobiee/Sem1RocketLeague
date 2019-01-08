#pragma once
#include <windows.h>
#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <directxcolors.h>

using namespace DirectX;

class Camera
{
public:
	Camera();
	~Camera();

	XMFLOAT4X4 GetView();
	XMFLOAT4X4 GetProjection();

	XMFLOAT4X4 Update();

	//float GetxPosCar() { return xPosCar; }
	//float GetyPosCar() { return yPosCar; }
	//float GetzPosCar() { return zPosCar; }

	float SetThirdPersonCamera(float x, float y, float z);

	//float GetxThirdPerson() { return xThirdPerson; }
	//float GetyThirdPerson() { return yThirdPerson; }
	//float GetzThirdPerson() { return zThirdPerson; }

private:

	XMFLOAT4X4              _view;
	XMFLOAT4X4              _projection;
	UINT _WindowHeight;
	UINT _WindowWidth;

	//float xPosCar;
	//float yPosCar;
	//float zPosCar;

	float xThirdPerson;
	float yThirdPerson;
	float zThirdPerson;
};

