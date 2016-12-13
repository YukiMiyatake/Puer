
//--------------------------------------------------------------------------------------
// Constant Buffer Variables
//--------------------------------------------------------------------------------------
cbuffer ConstantBuffer : register(b0)
{
	matrix World;
	matrix View;
	matrix Projection;
	float4 Diffuse;
	float4 vLightDir[2];
	float4 vLightColor[2];
	float4 vOutputColor;
}


//--------------------------------------------------------------------------------------
struct VS_INPUT
{
	float4 Pos			: POSITION;
	float3 Norm			: NORMAL;
};

struct PS_INPUT
{
	float4 Pos			: SV_POSITION;
	float4 WorldPos		: TEXCOORD0;
	float3 WorldNorm	: TEXCOORD1;
	float4 Diffuse		: TEXCOORD2;
};

struct PS_OUTPUT
{
	// デバッグのため入れ替え
	float4 Position		: SV_Target0;
	float4 NormalDepth	: SV_Target1;
	float4 DiffuseAlbedo: SV_Target2;
};

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
PS_INPUT VS(VS_INPUT input)
{
	PS_INPUT output = (PS_INPUT)0;

	float4 worldPos = mul(input.Pos, World);
	float4 viewPos = mul(worldPos, View);
	float4 projPos = mul(viewPos, Projection);

	float3 worldNormal = mul(input.Norm, (float3x3)World);

	output.Pos = projPos;
	output.WorldNorm = worldNormal;
	output.WorldPos = worldPos;
	output.Diffuse = float4(1.0, 1.0, 1.0, 1.0);

	return output;
}


//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
PS_OUTPUT PS(PS_INPUT input)
{
	PS_OUTPUT output = (PS_OUTPUT)0;

	float depth = input.WorldPos.z / input.WorldPos.w;

	// -1~1までの範囲を0~1に変換
	float3 normal = normalize(input.WorldNorm) * 0.5f + 0.5f;

	output.Position = input.WorldPos;
	output.NormalDepth = float4(normal, depth);
	output.DiffuseAlbedo = input.Diffuse;

	return output;

}

