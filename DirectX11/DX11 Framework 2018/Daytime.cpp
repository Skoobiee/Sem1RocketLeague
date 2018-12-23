//#include "Daytime.h"
//
//
//
//Daytime::Daytime()
//{
//
//}
//
//Daytime::~Daytime()
//{
//
//}
//
//void Daytime::Update()
//{
//	if (daytime)
//	{
//		timeOfDay++;
//		timeOfNight = 0;
//	}
//	else if (!daytime)
//	{
//		timeOfNight++;
//		timeOfDay = 0;
//	}
//
//	if (timeOfDay >= 100)
//	{
//		lightDirection = XMFLOAT3(0.25f, 0.5f, -1.0f);
//		diffuseMaterial = XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);
//		diffuseLight = XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f);
//
//		ambientMaterial = XMFLOAT4(0.2f, 0.7f, 1.0f, 0.2f);
//		ambientLight = XMFLOAT4(0.2f, 0.2f, 0.2f, 0.2f);
//
//		daytime = false;
//	}
//
//	if (timeOfNight >= 100)
//	{
//		lightDirection = XMFLOAT3(0.25f, 0.5f, -1.0f);
//		diffuseMaterial = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
//		diffuseLight = XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f);
//
//		ambientMaterial = XMFLOAT4(0.2f, 0.7f, 1.0f, 0.2f);
//		ambientLight = XMFLOAT4(0.2f, 0.2f, 0.2f, 0.2f);
//
//		daytime = true;
//	}
//}