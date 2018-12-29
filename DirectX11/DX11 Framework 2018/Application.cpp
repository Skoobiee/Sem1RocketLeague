#include "Application.h"
#include <iostream>

using namespace std;

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

Application::Application()
{
	_hInst = nullptr;
	_hWnd = nullptr;
	_driverType = D3D_DRIVER_TYPE_NULL;
	_featureLevel = D3D_FEATURE_LEVEL_11_0;
	_pd3dDevice = nullptr;
	_pImmediateContext = nullptr;
	_pSwapChain = nullptr;
	_pRenderTargetView = nullptr;
	_pVertexShader = nullptr;
	_pPixelShader = nullptr;
	_pVertexLayout = nullptr;
	_pVertexBuffer = nullptr;
	_pIndexBuffer = nullptr;
	_pPyramidVertexBuffer = nullptr;
	_pPyramidIndexBuffer = nullptr;
	_pGridVertexBuffer = nullptr;
	_pGridIndexBuffer = nullptr;
	_pCarVertexBuffer = nullptr;
	_pCarIndexBuffer = nullptr;
	_pPowerupVertexBuffer = nullptr;
	_pPowerupIndexBuffer = nullptr;
	_pConstantBuffer = nullptr;

	srand(time(NULL));
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
    _WindowWidth = rc.right - rc.left;
    _WindowHeight = rc.bottom - rc.top;

    if (FAILED(InitDevice()))
    {
        Cleanup();

        return E_FAIL;
    }

	// Initialize the world matrix
	XMStoreFloat4x4(&_world, XMMatrixIdentity());

	 // Light direction from surface (XYZ)
	lightDirection = XMFLOAT3(0.25f, 0.5f, -1.0f);
	// Diffuse material properties (RGBA)
	diffuseMaterial = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	// Diffuse light colour (RGBA)
	diffuseLight = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	ambientMaterial = XMFLOAT4(0.2f, 0.7f, 1.0f, 0.2f);
	ambientLight = XMFLOAT4(0.2f, 0.2f, 0.2f, 0.2f);

	specularMtrl = XMFLOAT4 (0.8f, 0.8f, 0.8f, 1.0f);
	specularLight = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	specularPower = 10.0f;
	eyePosW = XMFLOAT3 (3.0f, 0.0f, -3.0f);

	// Create the sample state
	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory(&sampDesc, sizeof(sampDesc));
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

	
	_pd3dDevice->CreateSamplerState(&sampDesc, &_pSamplerLinear);

	CreateDDSTextureFromFile(_pd3dDevice, L"Crate_COLOR.dds", 0, &_pTextureRV); //L"texture.dds"
	_pImmediateContext->PSSetSamplers(0, 1, &_pSamplerLinear);

	CreateDDSTextureFromFile(_pd3dDevice, L"Grass.dds", 0, &_pTextureGrass);

	CreateDDSTextureFromFile(_pd3dDevice, L"Crate_NRM.dds", 0, &_pTextureBlue);

	CreateDDSTextureFromFile(_pd3dDevice, L"ChainLink.dds", 0, &_pTextureCar);

	CreateDDSTextureFromFile(_pd3dDevice, L"Orange.dds", 0, &_pTexturePowerup);

	D3D11_BLEND_DESC blendDesc;
	ZeroMemory(&blendDesc, sizeof(blendDesc));

	D3D11_RENDER_TARGET_BLEND_DESC rtbd;
	ZeroMemory(&rtbd, sizeof(rtbd));

	rtbd.BlendEnable = true;
	rtbd.SrcBlend = D3D11_BLEND_SRC_COLOR;
	rtbd.DestBlend = D3D11_BLEND_BLEND_FACTOR;
	rtbd.BlendOp = D3D11_BLEND_OP_ADD;
	rtbd.SrcBlendAlpha = D3D11_BLEND_ONE;
	rtbd.DestBlendAlpha = D3D11_BLEND_ZERO;
	rtbd.BlendOpAlpha = D3D11_BLEND_OP_ADD;
	rtbd.RenderTargetWriteMask = D3D10_COLOR_WRITE_ENABLE_ALL;

	blendDesc.AlphaToCoverageEnable = false;
	blendDesc.RenderTarget[0] = rtbd;

	_pd3dDevice->CreateBlendState(&blendDesc, &Transparency);

	_camera = Camera();

	//xPos = GET_X_LPARAM(lParam);
	//yPos = GET_Y_LPARAM(lParam);

	objMeshData = OBJLoader::Load("car.obj", _pd3dDevice, false);
	objMeshDataSphere = OBJLoader::Load("sphere.obj", _pd3dDevice);

	timeOfDay = 0;
	timeOfNight = 0;
	daytime = true;

	carMoved = false;
	carSpeed = 2.0f;
	xPosCar = 5.0f;
	yPosCar = -700.0f;
	zPosCar = -1.0f;
	xRotation = 0;
	carIsMoving = false;

	boost = 100;

	//Grid
	XMStoreFloat4x4(&_world4, XMMatrixScaling(0.47f, 0.5f, 0.5f) *
		XMMatrixTranslation(-0.7f, -3.5f, 1.0f)*
		XMMatrixScaling(40.1f, 1.0f, 40.1f));

	//Powerup
	XMStoreFloat4x4(&_worldPowerup, XMMatrixScaling(5.0f, 5.0f, 5.0f) *
		XMMatrixTranslation(15.0f, -5.0f, -1.0f) *
		XMMatrixScaling(0.1f, 0.1f, 0.1f));

	XMStoreFloat4x4(&_worldPowerup2, XMMatrixScaling(5.0f, 5.0f, 5.0f) *
		XMMatrixTranslation(25.0f, -5.0f, 20.0f) *
		XMMatrixScaling(0.1f, 0.1f, 0.1f));

	XMStoreFloat4x4(&_worldPowerup3, XMMatrixScaling(5.0f, 5.0f, 5.0f) *
		XMMatrixTranslation(50.0f, -5.0f, 20.0f) *
		XMMatrixScaling(0.1f, 0.1f, 0.1f));

	return S_OK;
}

HRESULT Application::InitShadersAndInputLayout()
{
	HRESULT hr;

    // Compile the vertex shader
    ID3DBlob* pVSBlob = nullptr;
    hr = CompileShaderFromFile(L"DX11 Framework.fx", "VS", "vs_4_0", &pVSBlob);

    if (FAILED(hr))
    {
        MessageBox(nullptr,
                   L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
        return hr;
    }

	// Create the vertex shader
	hr = _pd3dDevice->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), nullptr, &_pVertexShader);

	if (FAILED(hr))
	{	
		pVSBlob->Release();
        return hr;
	}

	// Compile the pixel shader
	ID3DBlob* pPSBlob = nullptr;
    hr = CompileShaderFromFile(L"DX11 Framework.fx", "PS", "ps_4_0", &pPSBlob);

    if (FAILED(hr))
    {
        MessageBox(nullptr,
                   L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
        return hr;
    }

	// Create the pixel shader
	hr = _pd3dDevice->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), nullptr, &_pPixelShader);
	pPSBlob->Release();

    if (FAILED(hr))
        return hr;

    // Define the input layout, defines the vertex data structure, can specify exactly the data
    D3D11_INPUT_ELEMENT_DESC layout[] =
    {                                                    // position starts at byte 0, color starts at byte 12
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }, //rand() % 2
        { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	//change input layout when this is changed

	UINT numElements = ARRAYSIZE(layout);

    // Create the input layout
	hr = _pd3dDevice->CreateInputLayout(layout, numElements, pVSBlob->GetBufferPointer(),
                                        pVSBlob->GetBufferSize(), &_pVertexLayout);
	pVSBlob->Release();

	if (FAILED(hr))
        return hr;

    // Set the input layout
    _pImmediateContext->IASetInputLayout(_pVertexLayout);

	return hr;
}

HRESULT Application::InitVertexBuffer()
{
	HRESULT hr;

    // Create vertex buffer
    SimpleVertex vertices[] = 
    {
		{ XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT2(0.0f, 0.0f), }, 
		{ XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT2(1.0f, 0.0f), },
		{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f),   XMFLOAT2(0.0f, 1.0f), },
		{ XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f),  XMFLOAT2(1.0f, 1.0f), },

		{ XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f),  XMFLOAT2(0.0f, 0.0f), },
		{ XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f),  XMFLOAT2(1.0f, 0.0f), },
		{ XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f),	 XMFLOAT2(0.0f, 1.0f), },
		{ XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f),  XMFLOAT2(1.0f, 1.0f), },

		{ XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT2(0.0f, 0.0f), },
		{ XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f),  XMFLOAT2(1.0f, 0.0f), },
		{ XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f),  XMFLOAT2(0.0f, 1.0f), },
		{ XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT2(1.0f, 1.0f), },


		{ XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f),   XMFLOAT2(0.0f, 0.0f), },
		{ XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f),  XMFLOAT2(1.0f, 0.0f), },
		{ XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f),  XMFLOAT2(0.0f, 1.0f), },
		{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f),  XMFLOAT2(1.0f, 1.0f), },

		{ XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(0.0f, 0.0f), },
		{ XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f),   XMFLOAT2(1.0f, 0.0f), },
		{ XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f),	 XMFLOAT2(0.0f, 1.0f), },
		{ XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f),  XMFLOAT2(1.0f, 1.0f), },

		{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT3(0.0f, -1.0f, 0.0f), XMFLOAT2(0.0f, 0.0f), },
		{ XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT3(0.0f, -1.0f, 0.0f),  XMFLOAT2(1.0f, 0.0f), },
		{ XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT3(0.0f, -1.0f, 0.0f),  XMFLOAT2(0.0f, 1.0f), },
		{ XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT3(0.0f, -1.0f, 0.0f),  XMFLOAT2(1.0f, 1.0f) }

    };

    D3D11_BUFFER_DESC bd; //copy the vertices over to the graphics card
	ZeroMemory(&bd, sizeof(bd));
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(SimpleVertex) * 24; //8
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
    InitData.pSysMem = vertices;

    hr = _pd3dDevice->CreateBuffer(&bd, &InitData, &_pVertexBuffer);

    if (FAILED(hr))
        return hr;

	return S_OK;
}

HRESULT Application::InitPyramidVertexBuffer()
{
	HRESULT hr;

	SimpleVertex vertices[] =
	{
		{ XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT2(0.0f, 0.0f), }, //left
		{ XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(0.0f, 0.0f), }, //front
		{ XMFLOAT3(1.0f, 0.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(0.0f, 0.0f), }, //right
		{ XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(0.0f, 0.0f), }, //behind
		{ XMFLOAT3(0.5f, 1.0f, 0.5f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(0.0f, 0.0f), }, //top

		{ XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT2(1.0f, 1.0f), }, //left
		{ XMFLOAT3(0.5f, 1.0f, 0.5f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(1.0f, 0.0f), }, //top
		{ XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(0.0f, 1.0f), }, //front

		{ XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(1.0f, 1.0f), }, //front
		{ XMFLOAT3(0.5f, 1.0f, 0.5f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(1.0f, 0.0f), }, //top
		{ XMFLOAT3(1.0f, 0.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(0.0f, 1.0f), }, //right
		
		{ XMFLOAT3(1.0f, 0.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(1.0f, 1.0f), }, //right
		{ XMFLOAT3(0.5f, 1.0f, 0.5f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(1.0f, 0.0f), }, //top
		{ XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(0.0f, 1.0f), }, //behind

		{ XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(1.0f, 1.0f), }, //behind
		{ XMFLOAT3(0.5f, 1.0f, 0.5f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(1.0f, 0.0f), }, //top
		{ XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT2(0.0f, 1.0f), }, //left
	};

	D3D11_BUFFER_DESC bd; //copy the vertices over to the graphics card
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(SimpleVertex) * 17; //5
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = vertices;

	hr = _pd3dDevice->CreateBuffer(&bd, &InitData, &_pPyramidVertexBuffer);

	if (FAILED(hr))
		return hr;

	return S_OK;
}

HRESULT Application::InitGridVertexBuffer()
{
	HRESULT hr;

	SimpleVertex vertices[] =
	{
		{ XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(0.0f, 0.0f), },
		{ XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(0.0f, 1.0f), },
		{ XMFLOAT3(1.0f, 0.0f, -1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(1.0f, 0.0f), },
		{ XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(1.0f, 1.0f), },

		{ XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(0.0f, 0.0f), },
		{ XMFLOAT3(2.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(0.0f, 1.0f), },
		{ XMFLOAT3(2.0f, 0.0f, -1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(1.0f, 0.0f), },
		{ XMFLOAT3(1.0f, 0.0f, -1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(1.0f, 1.0f), },

		{ XMFLOAT3(2.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(0.0f, 0.0f), },
		{ XMFLOAT3(3.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(0.0f, 1.0f), },
		{ XMFLOAT3(3.0f, 0.0f, -1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(1.0f, 0.0f), },
		{ XMFLOAT3(2.0f, 0.0f, -1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(1.0f, 1.0f), },

		{ XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(0.0f, 0.0f), },
		{ XMFLOAT3(1.0f, 0.0f, -1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(0.0f, 1.0f), },
		{ XMFLOAT3(1.0f, 0.0f, -2.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(1.0f, 0.0f), },
		{ XMFLOAT3(0.0f, 0.0f, -2.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(1.0f, 1.0f), },

		{ XMFLOAT3(1.0f, 0.0f, -1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(0.0f, 0.0f), },
		{ XMFLOAT3(2.0f, 0.0f, -1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(0.0f, 1.0f), },
		{ XMFLOAT3(2.0f, 0.0f, -2.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(1.0f, 0.0f), },
		{ XMFLOAT3(1.0f, 0.0f, -2.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(1.0f, 1.0f), },

		{ XMFLOAT3(2.0f, 0.0f, -1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(0.0f, 0.0f), },
		{ XMFLOAT3(3.0f, 0.0f, -1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(0.0f, 1.0f), },
		{ XMFLOAT3(3.0f, 0.0f, -2.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(1.0f, 0.0f), },
		{ XMFLOAT3(2.0f, 0.0f, -2.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(1.0f, 1.0f), },

		{ XMFLOAT3(0.0f, 0.0f, -2.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(0.0f, 0.0f), },
		{ XMFLOAT3(1.0f, 0.0f, -2.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(0.0f, 1.0f), },
		{ XMFLOAT3(1.0f, 0.0f, -3.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(1.0f, 0.0f), },
		{ XMFLOAT3(0.0f, 0.0f, -3.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(1.0f, 1.0f), },

		{ XMFLOAT3(1.0f, 0.0f, -2.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(0.0f, 0.0f), },
		{ XMFLOAT3(2.0f, 0.0f, -2.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(0.0f, 1.0f), },
		{ XMFLOAT3(2.0f, 0.0f, -3.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(1.0f, 0.0f), },
		{ XMFLOAT3(1.0f, 0.0f, -3.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(1.0f, 1.0f), },

		{ XMFLOAT3(2.0f, 0.0f, -2.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(0.0f, 0.0f), },
		{ XMFLOAT3(3.0f, 0.0f, -2.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(0.0f, 1.0f), },
		{ XMFLOAT3(3.0f, 0.0f, -3.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(1.0f, 0.0f), },
		{ XMFLOAT3(2.0f, 0.0f, -3.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(1.0f, 1.0f), }
	};

	D3D11_BUFFER_DESC bd; //copy the vertices over to the graphics card
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(SimpleVertex) * 36;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = vertices;

	hr = _pd3dDevice->CreateBuffer(&bd, &InitData, &_pGridVertexBuffer);

	if (FAILED(hr))
		return hr;

	return S_OK;
}

HRESULT Application::InitCarVertexBuffer()
{
	HRESULT hr;
	
	D3D11_BUFFER_DESC MeshData;
	ZeroMemory(&MeshData, sizeof(MeshData));
	MeshData.Usage = D3D11_USAGE_DEFAULT;
	MeshData.ByteWidth = sizeof(SimpleVertex) * 300;
	MeshData.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	MeshData.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	//InitData.pSysMem = vertices;

	hr = _pd3dDevice->CreateBuffer(&MeshData, &InitData, &_pCarVertexBuffer);

	if (FAILED(hr))
		return hr;

	return S_OK;
}

HRESULT Application::InitPowerupVertexBuffer()
{
	HRESULT hr;

	D3D11_BUFFER_DESC MeshData;
	ZeroMemory(&MeshData, sizeof(MeshData));
	MeshData.Usage = D3D11_USAGE_DEFAULT;
	MeshData.ByteWidth = sizeof(SimpleVertex) * 300;
	MeshData.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	MeshData.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	//InitData.pSysMem = vertices;

	hr = _pd3dDevice->CreateBuffer(&MeshData, &InitData, &_pPowerupVertexBuffer);

	if (FAILED(hr))
		return hr;

	return S_OK;
}

HRESULT Application::InitIndexBuffer()
{
	HRESULT hr;

    // Create index buffer
    WORD indices[] =
    {
		0,1,3,
		0,3,2,

		4,5,7,
		4,7,6,

		8,9,11,
		8,11,10,

		12,13,15,
		12,15,14,

		16,17,19,
		16,19,18,

		20,21,23,
		20,23,22
    };

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));

    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(WORD) * 36;    //if we change number of indicies, have to change this value to match 
    bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
    InitData.pSysMem = indices;
    hr = _pd3dDevice->CreateBuffer(&bd, &InitData, &_pIndexBuffer);

    if (FAILED(hr))
        return hr;

	return S_OK;
}

HRESULT Application::InitPyramidIndexBuffer()
{
	HRESULT hr;

	
	WORD indices[] =
	{
		1,2,4,
		2,3,4,

		2,5,3,
		3,4,5,
		4,5,1,
		1,5,2
	};

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));

	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(WORD) * 18;    //if we change number of indicies, have to change this value to match 
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = indices;
	hr = _pd3dDevice->CreateBuffer(&bd, &InitData, &_pPyramidIndexBuffer);

	if (FAILED(hr))
		return hr;

	return S_OK;
}

HRESULT Application::InitGridIndexBuffer()
{
	HRESULT hr;


	WORD indices[] =
	{
		0,1,2,
		0,2,3,

		4,5,6,
		4,6,7,

		8,9,10,
		8,10,11,

		12,13,14,
		12,14,15,

		16,17,18,
		16,18,19,

		20,21,22,
		20,22,23,

		24,25,26,
		24,26,27,

		28,29,30,
		28,30,31,

		32,33,34,
		32,34,35
	};

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));

	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(WORD) * 54;    //if we change number of indicies, have to change this value to match 
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = indices;
	hr = _pd3dDevice->CreateBuffer(&bd, &InitData, &_pGridIndexBuffer);

	if (FAILED(hr))
		return hr;

	return S_OK;
}

HRESULT Application::InitCarIndexBuffer()
{
	HRESULT hr;

	D3D11_BUFFER_DESC MeshData;
	ZeroMemory(&MeshData, sizeof(MeshData));

	MeshData.Usage = D3D11_USAGE_DEFAULT;
	MeshData.ByteWidth = sizeof(WORD) * 300;  
	MeshData.BindFlags = D3D11_BIND_INDEX_BUFFER;
	MeshData.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	//InitData.pSysMem = indices;
	hr = _pd3dDevice->CreateBuffer(&MeshData, &InitData, &_pCarIndexBuffer);

	if (FAILED(hr))
		return hr;

	return S_OK;
}

HRESULT Application::InitPowerupIndexBuffer()
{
	HRESULT hr;

	D3D11_BUFFER_DESC MeshData;
	ZeroMemory(&MeshData, sizeof(MeshData));

	MeshData.Usage = D3D11_USAGE_DEFAULT;
	MeshData.ByteWidth = sizeof(WORD) * 300;
	MeshData.BindFlags = D3D11_BIND_INDEX_BUFFER;
	MeshData.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	//InitData.pSysMem = indices;
	hr = _pd3dDevice->CreateBuffer(&MeshData, &InitData, &_pPowerupIndexBuffer);

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
    RECT rc = {0, 0, 1920, 1080}; //size of window, 640, 480
    AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
    _hWnd = CreateWindow(L"TutorialWindowClass", L"Sophie Lambrou, 17012265", WS_OVERLAPPEDWINDOW,
                         CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr, hInstance,
                         nullptr);
    if (!_hWnd)
		return E_FAIL;

    ShowWindow(_hWnd, nCmdShow);

    return S_OK;
}

HRESULT Application::CompileShaderFromFile(WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut)
{
    HRESULT hr = S_OK;

    DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined(DEBUG) || defined(_DEBUG)
    // Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
    // Setting this flag improves the shader debugging experience, but still allows 
    // the shaders to be optimized and to run exactly the way they will run in 
    // the release configuration of this program.
    dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

    ID3DBlob* pErrorBlob;
    hr = D3DCompileFromFile(szFileName, nullptr, nullptr, szEntryPoint, szShaderModel, 
        dwShaderFlags, 0, ppBlobOut, &pErrorBlob);

    if (FAILED(hr))
    {
        if (pErrorBlob != nullptr)
            OutputDebugStringA((char*)pErrorBlob->GetBufferPointer());

        if (pErrorBlob) pErrorBlob->Release();

        return hr;
    }

    if (pErrorBlob) pErrorBlob->Release();

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

    DXGI_SWAP_CHAIN_DESC sd; //consists of the back and front buffer
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferCount = 1;
    sd.BufferDesc.Width = _WindowWidth;
    sd.BufferDesc.Height = _WindowHeight;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; //tells us how many bits are being used per pixel, can change resolution here, UNORM = unsigned normalised = between 0 and 1
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = _hWnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE; //fullscreen?

    for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++) //creates swap chain + device, swap chain creates graphics device
    {
        _driverType = driverTypes[driverTypeIndex];
        hr = D3D11CreateDeviceAndSwapChain(nullptr, _driverType, nullptr, createDeviceFlags, featureLevels, numFeatureLevels,
                                           D3D11_SDK_VERSION, &sd, &_pSwapChain, &_pd3dDevice, &_featureLevel, &_pImmediateContext);
        if (SUCCEEDED(hr))
            break;
    }

    if (FAILED(hr)) 
        return hr;

	//SETUP DEPTH STENCIL HERE
	D3D11_TEXTURE2D_DESC depthStencilDesc;

	depthStencilDesc.Width = _WindowWidth;
	depthStencilDesc.Height = _WindowHeight;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.SampleDesc.Count = 1;
	depthStencilDesc.SampleDesc.Quality = 0;
	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;

	_pd3dDevice->CreateTexture2D(&depthStencilDesc, nullptr, &_depthStencilBuffer);
	_pd3dDevice->CreateDepthStencilView(_depthStencilBuffer, nullptr, &_depthStencilView);

    // Create a render target view
    ID3D11Texture2D* pBackBuffer = nullptr;
    hr = _pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);

    if (FAILED(hr))
        return hr;

    hr = _pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &_pRenderTargetView); //returns to screen, gives back a pointer that we can render to
    pBackBuffer->Release();

    if (FAILED(hr))
        return hr;

    _pImmediateContext->OMSetRenderTargets(1, &_pRenderTargetView, _depthStencilView); //OM = output merger, takes whatever we have rendered and puts onto the screen using pointer ^
	//Currently rendered using painters algorithm, will need to replace nullptr and put depth stencil in

    // Setup the viewport, can do split screen here
    D3D11_VIEWPORT vp;
    vp.Width = (FLOAT)_WindowWidth;
    vp.Height = (FLOAT)_WindowHeight;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    _pImmediateContext->RSSetViewports(1, &vp);

	InitShadersAndInputLayout();

	InitVertexBuffer();
	InitPyramidVertexBuffer();
	InitGridVertexBuffer();
	InitCarVertexBuffer();
	InitPowerupVertexBuffer();

	InitIndexBuffer();
	InitPyramidIndexBuffer();
	InitGridIndexBuffer();
	InitCarIndexBuffer();
	InitPowerupIndexBuffer();

    // Set primitive topology
    _pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

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

	D3D11_RASTERIZER_DESC wfdesc;
	ZeroMemory(&wfdesc, sizeof(D3D11_RASTERIZER_DESC));
	wfdesc.FillMode = D3D11_FILL_WIREFRAME;
	wfdesc.CullMode = D3D11_CULL_NONE;
	hr = _pd3dDevice->CreateRasterizerState(&wfdesc, &_wireFrame);
	_pImmediateContext->RSSetState(_wireFrame);

	D3D11_RASTERIZER_DESC wfdescsolid;
	ZeroMemory(&wfdescsolid, sizeof(D3D11_RASTERIZER_DESC));
	wfdescsolid.FillMode = D3D11_FILL_SOLID; 
	wfdescsolid.CullMode = D3D11_CULL_NONE;
	hr = _pd3dDevice->CreateRasterizerState(&wfdescsolid, &_solidState);
	_pImmediateContext->RSSetState(_solidState);

    return S_OK;
}

void Application::Cleanup()
{
    if (_pImmediateContext) _pImmediateContext->ClearState();

	if (_depthStencilView) _depthStencilView->Release();
	if (_depthStencilBuffer) _depthStencilBuffer->Release();

	if (_wireFrame) _wireFrame->Release();
	if (_solidState) _solidState->Release();

    if (_pConstantBuffer) _pConstantBuffer->Release();
    if (_pVertexBuffer) _pVertexBuffer->Release();
    if (_pIndexBuffer) _pIndexBuffer->Release();

	if (_pPyramidVertexBuffer) _pPyramidVertexBuffer->Release();
	if (_pPyramidIndexBuffer) _pPyramidIndexBuffer->Release();

	if (_pGridVertexBuffer) _pGridVertexBuffer->Release();
	if (_pGridIndexBuffer) _pGridIndexBuffer->Release();

	if (_pCarVertexBuffer) _pCarVertexBuffer->Release();
	if (_pCarIndexBuffer) _pCarIndexBuffer->Release();

	if (_pPowerupVertexBuffer) _pPowerupVertexBuffer->Release();
	if (_pPowerupIndexBuffer) _pPowerupIndexBuffer->Release();

    if (_pVertexLayout) _pVertexLayout->Release();
    if (_pVertexShader) _pVertexShader->Release();
    if (_pPixelShader) _pPixelShader->Release();
    if (_pRenderTargetView) _pRenderTargetView->Release();
    if (_pSwapChain) _pSwapChain->Release();
    if (_pImmediateContext) _pImmediateContext->Release();
    if (_pd3dDevice) _pd3dDevice->Release();

	if (Transparency) Transparency->Release();
}

void Application::Update()
{
	// Update our time
	static float t = 0.0f;

	if (_driverType == D3D_DRIVER_TYPE_REFERENCE)
	{
		t += (float)XM_PI * 0.0125f;
	}
	else
	{
		static DWORD dwTimeStart = 0;
		DWORD dwTimeCur = GetTickCount();

		if (dwTimeStart == 0)
			dwTimeStart = dwTimeCur;

		t = (dwTimeCur - dwTimeStart) / 1000.0f;
	}

	// Animate the cube
	XMStoreFloat4x4(&_world, XMMatrixRotationZ(t) *
								XMMatrixTranslation(-2.0f, 0.0f, 5.0f)); //t value increases - makes it spin round

	XMStoreFloat4x4(&_world2, XMMatrixRotationZ(t) * 
								XMMatrixScaling(0.5f, 0.5f, 0.5f)* 
								XMMatrixTranslation(2.0f, 0.0f, 0.0f) * 
								XMMatrixRotationZ(t * 2.0f));

	//Pryamid
	XMStoreFloat4x4(&_world3, XMMatrixRotationZ(t) *
								XMMatrixScaling(0.45f, 0.45f, 0.45f) *
								XMMatrixRotationZ(t * 2.0f) * //can take a translation and rotation from previous object to rotate around it
								XMMatrixTranslation(-2.0f, 0.0f, 5.0f) *
								XMMatrixRotationZ(t * 5.0f) *
								XMMatrixTranslation(-2.0f, 0.0f, 5.0f) *
								XMMatrixRotationZ(t * 2.0f));

	

	/*XMStoreFloat4x4(&_world5, XMMatrixScaling(0.5f, 0.5f, 0.5f) *
								XMMatrixTranslation(-0.7f, -1.5f, 1.85f)*
								XMMatrixScaling(4.0f, 4.0f, 4.0f));*/

							
	if (GetAsyncKeyState(VK_UP))
	{
		D3D11_RASTERIZER_DESC wfdesc;
		ZeroMemory(&wfdesc, sizeof(D3D11_RASTERIZER_DESC));
		wfdesc.FillMode = D3D11_FILL_WIREFRAME;
		wfdesc.CullMode = D3D11_CULL_NONE;
		_pImmediateContext->RSSetState(_wireFrame);
	}
	else if (GetAsyncKeyState(VK_DOWN))
	{
		D3D11_RASTERIZER_DESC wfdesc;
		ZeroMemory(&wfdesc, sizeof(D3D11_RASTERIZER_DESC));
		wfdesc.FillMode = D3D11_FILL_SOLID;
		wfdesc.CullMode = D3D11_CULL_NONE;
		_pImmediateContext->RSSetState(_solidState);
	}


	//if (!carIsMoving)
	//{

	if (carSpeed <= 3.0f)
	{
		if (GetAsyncKeyState('W'))
		{
			zPosCar = zPosCar + carSpeed;
			carSpeed += 0.02f;
			//carIsMoving = true;
		}

		if (GetAsyncKeyState('S'))
		{
			zPosCar = zPosCar - carSpeed;
			carSpeed += 0.05f;
			//carIsMoving = true;
		}

		if (GetAsyncKeyState('A'))
		{
			xPosCar = xPosCar - carSpeed;
			carSpeed += 0.02f;
			xRotation = xRotation - 0.001;
			//carIsMoving = true;
		}

		if (GetAsyncKeyState('D'))
		{
			xPosCar = xPosCar + carSpeed;
			carSpeed += 0.02f;
			xRotation = xRotation + 0.001;
			//carIsMoving = true;
		}

		carSpeed = 2.0f;
		/*if (GetAsyncKeyState('W') || GetAsyncKeyState('S') || GetAsyncKeyState('A') || GetAsyncKeyState('D'))
		{

		}*/
	}

	if (xRotation >= 360 || xRotation <= -360)
	{
		xRotation = 0;
	}


		//If statement for when car collides with pickup
		//destroy pickup
		//if boost < 75, add to boost (max 100) (25 each)

		

		/*if (!GetAsyncKeyState('W') || GetAsyncKeyState('S') || GetAsyncKeyState('A') || GetAsyncKeyState('D'))
		{
			carSpeed = 2.0f;
		}*/

		/*else
		{
			carIsMoving = false;
		}
	/}*/

	
	//if (GetAsyncKeyState(VK_SHIFT))
	//{
	//	if (boost <= 100 && carSpeed <= 3.0f) //pickup collides with car, add to boost
	//	{
	//		carSpeed = carSpeed + 0.01f;
	//		boost = boost - 5.0f;
	//	}
	//	else
	//	{
	//		carSpeed = 2.0f;
	//	}
	//}

	//Car
	XMStoreFloat4x4(&_worldCar, XMMatrixScaling(5.0f, 5.0f, 5.0f) *
								XMMatrixTranslation(xPosCar, yPosCar, zPosCar) *
								XMMatrixRotationY(xRotation) *
								XMMatrixScaling(0.005f, 0.005f, 0.005f));

	_camera.Update();

	if (daytime)
	{
		timeOfDay++;
		timeOfNight = 0;
	}
	else if (!daytime)
	{
		timeOfNight++;
		timeOfDay = 0;
	}

	if (timeOfDay >= 1600 && timeOfDay <= 2799 || timeOfNight >= 3200 && timeOfNight <= 4799) //800, 1399,  1600, 2399
	{
		lightDirection = XMFLOAT3(0.25f, 0.5f, -1.0f);
		diffuseMaterial = XMFLOAT4(0.7f, 0.7f, 1.0f, 1.0f);
		diffuseLight = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

		ambientMaterial = XMFLOAT4(0.2f, 0.7f, 1.0f, 0.2f);
		ambientLight = XMFLOAT4(0.2f, 0.2f, 0.2f, 0.2f);
	}

	if (timeOfDay >= 3200 && timeOfDay <= 4799 || timeOfNight >= 1600 && timeOfNight <= 2799) //1600, 2399,   800, 1399
	{
		lightDirection = XMFLOAT3(0.25f, 0.5f, -1.0f);
		diffuseMaterial = XMFLOAT4(0.4f, 0.4f, 1.0f, 1.0f);
		diffuseLight = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

		ambientMaterial = XMFLOAT4(0.2f, 0.7f, 1.0f, 0.2f);
		ambientLight = XMFLOAT4(0.2f, 0.2f, 0.2f, 0.2f);
	}

	if (timeOfDay >= 4800) //2400
	{
		daytime = false;

		lightDirection = XMFLOAT3(0.25f, 0.5f, -1.0f);
		diffuseMaterial = XMFLOAT4(0.1f, 0.1f, 1.0f, 1.0f);
		diffuseLight = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

		ambientMaterial = XMFLOAT4(0.2f, 0.7f, 1.0f, 0.2f);
		ambientLight = XMFLOAT4(0.2f, 0.2f, 0.2f, 0.2f);
	}
	
	if (timeOfNight >= 4800) //2400
	{
		daytime = true;

		lightDirection = XMFLOAT3(0.25f, 0.5f, -1.0f);
		diffuseMaterial = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		diffuseLight = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

		ambientMaterial = XMFLOAT4(0.2f, 0.7f, 1.0f, 0.2f);
		ambientLight = XMFLOAT4(0.2f, 0.2f, 0.2f, 0.2f);
	}
}

void Application::Draw()
{
    // Clear the back buffer
    float ClearColor[4] = {0.0f, 0.125f, 0.3f, 1.0f}; // red,green,blue,alpha
    _pImmediateContext->ClearRenderTargetView(_pRenderTargetView, ClearColor);

	_pImmediateContext->ClearDepthStencilView(_depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);	

	XMMATRIX world = XMLoadFloat4x4(&_world);
	XMMATRIX view = XMLoadFloat4x4(&_camera.GetView());
	XMMATRIX projection = XMLoadFloat4x4(&_camera.GetProjection());

    // Update variables
    ConstantBuffer cb;
	cb.mWorld = XMMatrixTranspose(world);
	cb.mView = XMMatrixTranspose(view);
	cb.mProjection = XMMatrixTranspose(projection);

	cb.DiffuseLight = XMFLOAT4(diffuseLight);
	cb.DiffuseMtrl = XMFLOAT4(diffuseMaterial);
	cb.LightVecW = XMFLOAT3(lightDirection);
	
	cb.AmbientMtrl = XMFLOAT4(ambientMaterial);
	cb.AmbientLight = XMFLOAT4(ambientLight);

	cb.SpecularLight = XMFLOAT4(specularLight);
	cb.SpecularMtrl = XMFLOAT4(specularMtrl);
	cb.SpecularPower = (specularPower);
	cb.EyePosW = XMFLOAT3(eyePosW);

	_pImmediateContext->UpdateSubresource(_pConstantBuffer, 0, nullptr, &cb, 0, 0); //constant buffer is being updated, reads from shaders, passes data into the buffer

	// Set vertex buffer
	UINT stride = sizeof(SimpleVertex);
	UINT offset = 0;

	// "fine-tune" the blending equation
	float blendFactor[] = { 0.75f, 0.75f, 0.75f, 1.0f };

	// Set the default blend state (no blending) for opaque objects
	_pImmediateContext->OMSetBlendState(0, 0, 0xffffffff);

	_pImmediateContext->IASetVertexBuffers(0, 1, &_pVertexBuffer, &stride, &offset);

	// Set index buffer
	_pImmediateContext->IASetIndexBuffer(_pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);

	_pImmediateContext->PSSetShaderResources(0, 1, &_pTextureRV); //binds to pipeline

    // Renders a cube
	_pImmediateContext->VSSetShader(_pVertexShader, nullptr, 0);
	_pImmediateContext->VSSetConstantBuffers(0, 1, &_pConstantBuffer);
	_pImmediateContext->PSSetConstantBuffers(0, 1, &_pConstantBuffer);
	_pImmediateContext->PSSetShader(_pPixelShader, nullptr, 0);
	_pImmediateContext->DrawIndexed(36, 0, 0); //CHANGE NUMBER OF INDICIES   

	
	/*world = XMLoadFloat4x4(&_world5);
	cb.mWorld = XMMatrixTranspose(world);
	_pImmediateContext->UpdateSubresource(_pConstantBuffer, 0, nullptr, &cb, 0, 0);
	_pImmediateContext->DrawIndexed(36, 0, 0);*/
	
	_pImmediateContext->PSSetShaderResources(0, 1, &_pTextureCar); 

	//Renders the Car
	world = XMLoadFloat4x4(&_worldCar);
	cb.mWorld = XMMatrixTranspose(world);
	_pImmediateContext->UpdateSubresource(_pConstantBuffer, 0, nullptr, &cb, 0, 0);

	unsigned int stride2 = objMeshData.VBStride;
	unsigned int offset2 = 0;

	_pImmediateContext->IASetVertexBuffers(0, 1, &objMeshData.VertexBuffer, &stride2, &offset2);
	_pImmediateContext->IASetIndexBuffer(objMeshData.IndexBuffer, DXGI_FORMAT_R16_UINT, 0);
	_pImmediateContext->DrawIndexed(objMeshData.IndexCount, 0, 0);

	_pImmediateContext->PSSetShaderResources(0, 1, &_pTexturePowerup); 

	//Renders the Powerups
	world = XMLoadFloat4x4(&_worldPowerup);
	cb.mWorld = XMMatrixTranspose(world);
	_pImmediateContext->UpdateSubresource(_pConstantBuffer, 0, nullptr, &cb, 0, 0);

	unsigned int stride3 = objMeshDataSphere.VBStride;
	unsigned int offset3 = 0;

	_pImmediateContext->IASetVertexBuffers(0, 1, &objMeshDataSphere.VertexBuffer, &stride3, &offset3);
	_pImmediateContext->IASetIndexBuffer(objMeshDataSphere.IndexBuffer, DXGI_FORMAT_R16_UINT, 0);
	_pImmediateContext->DrawIndexed(objMeshDataSphere.IndexCount, 0, 0);

	world = XMLoadFloat4x4(&_worldPowerup2);
	cb.mWorld = XMMatrixTranspose(world);
	_pImmediateContext->UpdateSubresource(_pConstantBuffer, 0, nullptr, &cb, 0, 0);

	_pImmediateContext->IASetVertexBuffers(0, 1, &objMeshDataSphere.VertexBuffer, &stride3, &offset3);
	_pImmediateContext->IASetIndexBuffer(objMeshDataSphere.IndexBuffer, DXGI_FORMAT_R16_UINT, 0);
	_pImmediateContext->DrawIndexed(objMeshDataSphere.IndexCount, 0, 0);

	world = XMLoadFloat4x4(&_worldPowerup3);
	cb.mWorld = XMMatrixTranspose(world);
	_pImmediateContext->UpdateSubresource(_pConstantBuffer, 0, nullptr, &cb, 0, 0);

	_pImmediateContext->IASetVertexBuffers(0, 1, &objMeshDataSphere.VertexBuffer, &stride3, &offset3);
	_pImmediateContext->IASetIndexBuffer(objMeshDataSphere.IndexBuffer, DXGI_FORMAT_R16_UINT, 0);
	_pImmediateContext->DrawIndexed(objMeshDataSphere.IndexCount, 0, 0);


	////Renders second cube
	//world = XMLoadFloat4x4(&_world2); //converts float to mxmatrix
	//cb.mWorld = XMMatrixTranspose(world);//passes it into the constant buffer
	//_pImmediateContext->UpdateSubresource(_pConstantBuffer, 0, nullptr, &cb, 0, 0); //changes whats in the constant buffer
	//_pImmediateContext->DrawIndexed(36, 0, 0); //CHANGE NUMBER OF INDICIES 

	_pImmediateContext->PSSetShaderResources(0, 1, &_pTextureGrass); //binds to pipeline

   //Renders a grid
	_pImmediateContext->IASetVertexBuffers(0, 1, &_pGridVertexBuffer, &stride, &offset);

	_pImmediateContext->IASetIndexBuffer(_pGridIndexBuffer, DXGI_FORMAT_R16_UINT, 0);

	world = XMLoadFloat4x4(&_world4);
	cb.mWorld = XMMatrixTranspose(world);
	_pImmediateContext->UpdateSubresource(_pConstantBuffer, 0, nullptr, &cb, 0, 0);
	_pImmediateContext->DrawIndexed(54, 0, 0);

	// Set the blend state for transparent objects
	_pImmediateContext->OMSetBlendState(Transparency, blendFactor, 0xffffffff);

	_pImmediateContext->PSSetShaderResources(0, 1, &_pTextureBlue); //binds to pipeline

	 //Renders a pyramid
	_pImmediateContext->IASetVertexBuffers(0, 1, &_pPyramidVertexBuffer, &stride, &offset);

	_pImmediateContext->IASetIndexBuffer(_pPyramidIndexBuffer, DXGI_FORMAT_R16_UINT, 0);

	world = XMLoadFloat4x4(&_world3); 
	cb.mWorld = XMMatrixTranspose(world);
	_pImmediateContext->UpdateSubresource(_pConstantBuffer, 0, nullptr, &cb, 0, 0); 
	_pImmediateContext->DrawIndexed(18, 0, 0); 

    // Present our back buffer to our front buffer
    _pSwapChain->Present(0, 0);
}