
Texture2D PositionSmp : register(t0);
Texture2D NormalDepthSmp : register(t1);
Texture2D DiffuseAlbedoSmp : register(t2);
SamplerState samLinear : register(s0);


cbuffer ConstantBuffer : register(b0)
{
	matrix World;
	matrix View;
	matrix Projection;
	float4 vLightDir[2];
	float4 vLightColor[2];
	float4 vOutputColor;
}


/*
cbuffer cbNeverChanges : register(b0)
{
float3 CameraPosition;
};

cbuffer cbLight : register(b1)
{
float3 LightPosition[2];
float3 LightColor[2];

};
*/
struct LightVSInput
{
	float4 Position : POSITION;
	float2 TexCoord : TEXCOORD0;
};

/////////////////////////////////////////////////////////////////////////
// LightVSOutput structure
/////////////////////////////////////////////////////////////////////////
struct LightVSOutput
{
	float4 Position : SV_POSITION;
	float2 TexCoord : TEXCOORD0;
};

//-----------------------------------------------------------------------
// Name : LightVSFunc()
// Desc : 遅延レンダリング - ライティングパス頂点処理
//-----------------------------------------------------------------------
LightVSOutput VS(const LightVSInput input)
{
	LightVSOutput output = (LightVSOutput)0;

	output.Position = input.Position;
	output.TexCoord = input.TexCoord;

	return output;
}

//------------------------------------------------------------------------
// Name : LightPSFunc()
// Desc : 遅延レンダリング - ライティングパスピクセル処理
//------------------------------------------------------------------------
float4 PS(const LightVSOutput input) : SV_Target
{
	/*
	float4 finalColor = 0;
	for (int i = 0; i<2; i++)
	{
	finalColor += saturate(dot((float3)vLightDir[i],input.Norm) * vLightColor[i]);
	}
	finalColor.a = 1;
	return finalColor;
	*/

	//	return DiffuseAlbedoSmp.Sample(samLinear, input.TexCoord);

	float3 WorldPosition = PositionSmp.Sample(samLinear, input.TexCoord).rgb;
	float3 WorldNormal = float3(0, 0, 0);
	float  Depth = 0;
	{
		float4 normalDepth = NormalDepthSmp.Sample(samLinear, input.TexCoord);
		WorldNormal = normalDepth.rgb * 2.0f - 1.0f;
		Depth = normalDepth.a;

	}

	float4 Color = float4(0, 0, 0, 0);
	//float3 EyeDir = normalize(CameraPosition - WorldPosition);
	float4 Diffuse = DiffuseAlbedoSmp.Sample(samLinear, input.TexCoord);

	float4 finalColor = 0;
	for (int i = 0; i < 2; i++)
	{
		//	float3 L = normalize(vLightDir[i]).xyz;
		//		finalColor += float4( vLightDir[i] * vLightColor[i] * max(dot(L, WorldNormal.xyz),0) );
		finalColor += (float4(dot(vLightDir[i].xyz, WorldNormal.xyz) * vLightColor[i].xyz , 0) * Diffuse);
		//finalColor += saturate(dot((float3)vLightDir[i], WorldNormal) * vLightColor[i]);
		/*
		float3 L = normalize(vLightDir[i].xyz);
		float  attenution = 1.0f / length(vLightDir[i]);
		Color.rgb += attenution * vLightColor[i] * Diffuse.rgb * max(dot(L, WorldNormal), 0);
		Color.a = 1.0f;
		*/
	}
	finalColor.a = 1.0;
	return finalColor;
	//	return float4(0.4f, 0.2f, 0.3f, 1.0f);
	return Color;
}

//--------------------------------------------------------------------------------------
// PSSolid - render a solid color
//--------------------------------------------------------------------------------------
float4 PSSolid(LightVSInput input) : SV_Target
{
	return vOutputColor;
}
