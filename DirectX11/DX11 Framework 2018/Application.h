#pragma once

#include <d3dcompiler.h>
#include <directxcolors.h>
#include "resource.h"
#include <time.h>
#include "DDSTextureLoader.h"
#include "Camera.h"
#include "Structures.h"
#include "OBJLoader.h"

//#define WM_MOUSEMOVE 0x0200

using namespace DirectX;

//struct SimpleVertex
//{
//    XMFLOAT3 Pos;
//   // XMFLOAT4 Color; //matches the input layout, change input layout when this is changed
//	XMFLOAT3 normal;
//	XMFLOAT2 TexC;
//};

struct ConstantBuffer //has to match in other constant buffer in .fx
{
	XMMATRIX mWorld;
	XMMATRIX mView;
	XMMATRIX mProjection;
	//float time;
	XMFLOAT4 DiffuseMtrl;
	XMFLOAT4 DiffuseLight;
	XMFLOAT3 LightVecW;
	float pad;
	XMFLOAT4 AmbientMtrl;
	XMFLOAT4 AmbientLight;
	XMFLOAT4 SpecularMtrl;
	XMFLOAT4 SpecularLight;
	float SpecularPower;
	XMFLOAT3 EyePosW; 	// Camera position in world space

};

class Application
{
private:
	HINSTANCE               _hInst;
	HWND                    _hWnd;
	D3D_DRIVER_TYPE         _driverType;
	D3D_FEATURE_LEVEL       _featureLevel;
	ID3D11Device*           _pd3dDevice;
	ID3D11DeviceContext*    _pImmediateContext;
	IDXGISwapChain*         _pSwapChain;
	ID3D11RenderTargetView* _pRenderTargetView;
	ID3D11VertexShader*     _pVertexShader;
	ID3D11PixelShader*      _pPixelShader;
	ID3D11InputLayout*      _pVertexLayout;
	ID3D11Buffer*           _pVertexBuffer;
	ID3D11Buffer*           _pIndexBuffer;
	ID3D11Buffer*           _pConstantBuffer;
	XMFLOAT4X4              _world, _world2, _world3, _world4, _world5; //a world for each object
	XMFLOAT4X4              _view;
	XMFLOAT4X4              _projection;

	ID3D11Buffer*           _pPyramidVertexBuffer;
	ID3D11Buffer*           _pPyramidIndexBuffer;

	ID3D11Buffer*           _pGridVertexBuffer;
	ID3D11Buffer*           _pGridIndexBuffer;

	XMFLOAT3 lightDirection;
	XMFLOAT4 diffuseMaterial;
	XMFLOAT4 diffuseLight;

	XMFLOAT4 ambientMaterial;
	XMFLOAT4 ambientLight;

	XMFLOAT4 specularMtrl;
	XMFLOAT4 specularLight;
	float specularPower;
	XMFLOAT3 eyePosW;

	ID3D11BlendState* Transparency;

	Camera _camera;

	XMFLOAT3 xPos;
	XMFLOAT3 yPos;

	MeshData objMeshData;


private:
	HRESULT InitWindow(HINSTANCE hInstance, int nCmdShow);
	HRESULT InitDevice();
	void Cleanup();
	HRESULT CompileShaderFromFile(WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut);
	HRESULT InitShadersAndInputLayout();

	HRESULT InitVertexBuffer();
	HRESULT InitIndexBuffer();

	HRESULT InitPyramidVertexBuffer();
	HRESULT InitPyramidIndexBuffer();

	HRESULT InitGridVertexBuffer();
	HRESULT InitGridIndexBuffer();

	UINT _WindowHeight;
	UINT _WindowWidth;

	ID3D11DepthStencilView* _depthStencilView;
	ID3D11Texture2D* _depthStencilBuffer;

	ID3D11RasterizerState* _wireFrame;
	ID3D11RasterizerState* _solidState;

	ID3D11ShaderResourceView * _pTextureRV = nullptr;
	ID3D11SamplerState * _pSamplerLinear = nullptr;

	ID3D11ShaderResourceView * _pTextureRV2 = nullptr;
	ID3D11SamplerState * _pSamplerLinear2 = nullptr;

public:
	Application();
	~Application();

	HRESULT Initialise(HINSTANCE hInstance, int nCmdShow);

	void Update();
	void Draw();
};

