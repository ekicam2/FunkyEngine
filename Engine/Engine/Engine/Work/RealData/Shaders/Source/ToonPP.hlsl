#include "PPVS.hlsl"

//////////////////////////////////////////////////////////////////////////////
// Post process implementation

Texture2D SceneColor : register(t0);
Texture2D SceneDepth : register(t1);

SamplerState SampleType;

cbuffer PostProcessBuffer : register(b0)
{
	float2 ScreenSize;
};

float Sobel(Texture2D inTX, SamplerState samplerType, float2 uv) 
{
	float4 hr = float4(0, 0, 0, 0);
	float4 vt = float4(0, 0, 0, 0);

	float2 offset = float2( 1.0/1016.0, 1.0/492.0);

	hr += inTX.Sample(samplerType, uv + (float2(-1.0,  1.0) * offset)) *  1.0;
	hr += inTX.Sample(samplerType, uv + (float2(-1.0,  0.0) * offset)) *  2.0;
	hr += inTX.Sample(samplerType, uv + (float2(-1.0, -1.0) * offset)) *  1.0;

	hr += inTX.Sample(samplerType, uv + (float2( 0.0,  1.0) * offset)) *  0.0;
	hr += inTX.Sample(samplerType, uv + (float2( 0.0,  0.0) * offset)) *  0.0;
	hr += inTX.Sample(samplerType, uv + (float2( 0.0, -1.0) * offset)) *  0.0;

	hr += inTX.Sample(samplerType, uv + (float2( 1.0,  1.0) * offset)) * -1.0;
	hr += inTX.Sample(samplerType, uv + (float2( 1.0,  0.0) * offset)) * -2.0;
	hr += inTX.Sample(samplerType, uv + (float2( 1.0, -1.0) * offset)) * -1.0;
	
	///////////////////////////////////////////////////////////////////////

	vt += inTX.Sample(samplerType, uv + (float2(-1.0,  1.0) * offset)) * -1.0;
	vt += inTX.Sample(samplerType, uv + (float2(-1.0,  0.0) * offset)) *  0.0;
	vt += inTX.Sample(samplerType, uv + (float2(-1.0, -1.0) * offset)) *  1.0;
	
	vt += inTX.Sample(samplerType, uv + (float2( 0.0,  1.0) * offset)) * -2.0;
	vt += inTX.Sample(samplerType, uv + (float2( 0.0,  0.0) * offset)) *  0.0;
	vt += inTX.Sample(samplerType, uv + (float2( 0.0, -1.0) * offset)) *  2.0;
	
	vt += inTX.Sample(samplerType, uv + (float2( 1.0,  1.0) * offset)) * -1.0;
	vt += inTX.Sample(samplerType, uv + (float2( 1.0,  0.0) * offset)) *  0.0;
	vt += inTX.Sample(samplerType, uv + (float2( 1.0, -1.0) * offset)) *  1.0;

	float intensity = 0.8;

	hr *= intensity;
	vt *= intensity;

	return dot(hr, hr) + dot(vt, vt);//sqrt(dot(hr, hr) + dot(vt, vt));
}

float4 PSMain(VSOutPSIn Input) : SV_TARGET0
{
	//if(Input.uv.x < 0.25) return float4(1.0, 1.0, 0.0, 0.0);
	//if(Input.uv.y < 0.25) return float4(0.0, 1.0, 0.0, 0.0);

	float2 SamplerUV = float2(Input.uv.x, 1.0 - Input.uv.y);
	float4 InScene = SceneColor.Sample(SampleType, SamplerUV);

	float sobel = Sobel(SceneColor, SampleType, SamplerUV);
	sobel = floor(sobel);
	InScene = InScene * clamp((1.0 - sobel), 0.0, 1.0);
	return InScene;
}