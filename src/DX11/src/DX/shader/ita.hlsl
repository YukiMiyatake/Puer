
Texture2DArray<float4> tex : register(t0); 

/*
Texture2D tex0				: register(t0);
Texture2D tex1				: register(t1);
Texture2D tex2				: register(t2);
Texture2D tex3				: register(t3);
Texture2D tex4				: register(t4);
Texture2D tex5				: register(t5);
Texture2D tex6				: register(t6);
Texture2D tex7				: register(t7);
//*/

SamplerState ss
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Wrap;
	AddressV = Wrap;
};


struct VSInput
{
	float4 Position		: POSITION;
	float2 TexCoord		: TEXCOORD0;
};


struct PSInput
{
	float4 Position		: SV_POSITION;
	float3 TexCoord		: TEXCOORD0;
//	float3 Color		: COLOR0;
};









PSInput VS(const VSInput input)
{
	PSInput output = (PSInput)0;

	output.Position = input.Position;
	output.TexCoord = float3(input.TexCoord, 0.f);
	//output.tex = tex0;
//	output.Color = float3(1,0,0);
	return output;
}

[maxvertexcount(12)]
void GS4(triangle PSInput input[3], inout TriangleStream<PSInput> OutputStream)
{
	PSInput output = (PSInput)0;
	float4 pos[3];


	for (uint i = 0; i<3; i++)
	{
		pos[i] = input[i].Position * float4(0.5, 0.5, 1, 1);

		output.Position =  pos[i] + float4(-0.5, +0.5, 0, 0);
		output.TexCoord = float3(input[i].TexCoord.xy, 0);
//		output.Color = float3(0, 1, 0);

		OutputStream.Append(output);
	}
	OutputStream.RestartStrip();


	for (uint i = 0; i<3; i++)
	{
		output.Position = pos[i] + float4(+0.5, +0.5, 0, 0);
		output.TexCoord = float3(input[i].TexCoord.xy, 1);
//		output.Color = float3(0, 0, 1);

		OutputStream.Append(output);
	}
	OutputStream.RestartStrip();

	for (uint i = 0; i<3; i++)
	{
		output.Position = pos[i] + float4(-0.5, -0.5, 0, 0);
		output.TexCoord = float3(input[i].TexCoord.xy, 2);
		//		output.Color = float3(0, 1, 0); 

		OutputStream.Append(output);
	}
	OutputStream.RestartStrip();


	for (uint i = 0; i<3; i++)
	{
		output.Position = pos[i] + float4(+0.5, -0.5, 0, 0);
		output.TexCoord = float3(input[i].TexCoord.xy, 3);
		//		output.Color = float3(0, 0, 1);

		OutputStream.Append(output);
	}

	OutputStream.RestartStrip();

}


float4 PS(PSInput input) : SV_Target
{
//	return 	float4(input.Color,0);

	return 	tex.Sample(ss, input.TexCoord);
}



