struct VertexInput
{
	float3 vPosition : POSITION;
};

struct VSOutPSIn
{
	float4 vPos : SV_POSITION;
	float2 uv : TEXCOORD0;
};

VSOutPSIn VSMain(VertexInput Input)
{
	VSOutPSIn Out;

	Out.vPos = float4(Input.vPosition, 1.0);
	Out.uv = Input.vPosition.xy;
	//Out.uv.y = 1.0 - Out.uv.y;
	Out.uv = Out.uv * 0.5 +  0.5;

	return Out;
}

//////////////////////////////////////////////////////////////////////////////
// Post process implementation

Texture2D shaderTexture;
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
	float4 InScene = shaderTexture.Sample(SampleType, SamplerUV);

	float sobel = Sobel(shaderTexture, SampleType, SamplerUV);
	sobel = floor(sobel);
	InScene = InScene * clamp((1.0 - sobel), 0.0, 1.0);// * InScene;
	return InScene;//lerp(InScene, float4(0.0, 0.0, 0.0, 0.0), sobel * 0.1);
}