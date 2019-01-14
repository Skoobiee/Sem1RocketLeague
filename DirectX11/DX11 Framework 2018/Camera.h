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

	XMFLOAT4X4 ThirdPersonCamera();

	void SetTargetPosition(float x, float y, float z);

	XMFLOAT3 TargetPosition;
	XMFLOAT3 TrackingOffset;

private:

	XMFLOAT4X4              _view;
	XMFLOAT4X4              _projection;
	UINT _WindowHeight;
	UINT _WindowWidth;

	bool thirdPerson;
	bool firstPerson;

};

