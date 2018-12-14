//--------------------------------------------------------------------------------------
// File: DX11 Framework.fx
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------

Texture2D txDiffuse : register(t0);
SamplerState samLinear : register(s0);

//--------------------------------------------------------------------------------------
// Constant Buffer Variables
//--------------------------------------------------------------------------------------
cbuffer ConstantBuffer : register( b0 )
{
	matrix World;
	matrix View;
	matrix Projection;
	//float time;
	float4 DiffuseMtrl;
	float4 DiffuseLight;
	float3 LightVecW;
	float pad1;
	float4 AmbientLight;
	float4 AmbientMtrl;
	float4 SpecularMtrl;
	float4 SpecularLight;
	float SpecularPower;
	float3 EyePosW; 	// Camera position in world space

}

//--------------------------------------------------------------------------------------
struct VS_OUTPUT
{
    float4 Pos : SV_POSITION;
    float4 Color : COLOR0;
	float3 NormalW : NORMAL;
	float3 PosW : POSITION;
	float2 Tex : TEXCOORD0;
};

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
//VS_OUTPUT VS( float4 Pos : POSITION, float4 Color : COLOR ) //can only access vertex that we pass in
//{
//    VS_OUTPUT output = (VS_OUTPUT)0;
//	//Pos.xy += 0.5f * sin(Pos.x) * sin(3.0f * gTime);
//	//Pos.z *= 0.6f + 0.4f * sin(2.0f * gTime);
//    output.Pos = mul( Pos, World );
//    output.Pos = mul( output.Pos, View );
//    output.Pos = mul( output.Pos, Projection );
//    output.Color = Color;
//	//float time;
//    return output;
//}

VS_OUTPUT VS(float4 Pos : POSITION, float3 NormalL : NORMAL, float2 Tex : TEXCOORD0)
{
	VS_OUTPUT output = (VS_OUTPUT)0;

	output.Pos = mul(Pos, World);
	output.PosW = output.Pos; //LEAVE HERE

	output.Pos = mul(output.Pos, View);
	output.Pos = mul(output.Pos, Projection);

	// Convert from local space to world space 
	// W component of vector is 0 as vectors cannot be translated
	output.NormalW = mul(float4(NormalL, 0.0f), World).xyz;
	
	output.Tex = Tex;

	return output;
}

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS( VS_OUTPUT input ) : SV_Target //runs for every single pixel
{
	float4 textureColor = txDiffuse.Sample(samLinear, input.Tex);

	//return float4(input.Tex.x, input.Tex.y, 0.0f, 0.0f);
	float3 toEye = normalize(EyePosW - input.PosW.xyz);

	float3 normalW = normalize(input.NormalW);
	float3 lightVec = normalize(LightVecW);
	float3 r = reflect(-lightVec, normalW);

	// Determine how much (if any) specular light makes it into the eye.
	float specularAmount = pow(max(dot(r, toEye), 0.0f), SpecularPower);

	//Diffuse lighting
	float diffuseAmount = max(dot(lightVec, normalW), 0.0f);
	float3 diffuse = diffuseAmount * (DiffuseMtrl * DiffuseLight).rgb;

	float3 ambient = AmbientMtrl * AmbientLight;

	float3 specular = specularAmount * (SpecularMtrl * SpecularLight).rgb;

	//float4 finalColor = float4 (1.0f, 0.0f, 0.0f, 1.0f);

	input.Color.rgb = ((textureColor.rgb * (ambient + diffuse)) + specular);
	//input.Color.rgb = diffuse + ambient + specular;
	input.Color.a = DiffuseMtrl.a;

    return input.Color; //return float4(1.0f, 0.0f, 0.0f, 1.0f);
}
