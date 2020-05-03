cbuffer PerObjectConstant : register(b0)
{
	// REMEMBER IN HLSL matrices are column major
	matrix mvp;
	matrix model;
};

struct VertexInput
{
	float3 vPosition : POSITION;
	float3 vColor : COLOR;
	float3 vNormal : NORMAL;
 	float2 vUv : UV;
};

struct VSOutPSIn
{
	float4 vPos : SV_POSITION;
	float4 vPosWS : POSITION_WS;
	float4 vNormalWS : NORMAL_WS;
};

VSOutPSIn VSMain(VertexInput Input)
{
	VSOutPSIn Out;
	
	Out.vPos = mul(mvp, float4(Input.vPosition, 1.0));
	Out.vPosWS = mul(model, float4(Input.vPosition, 1.0));
	Out.vNormalWS = mul(model, float4(Input.vNormal, 0.0));

	return Out;
}

//////////////////////////////////////////////////////////////////////////////


cbuffer PerViewObjectConstant : register(b0)
{
	// REMEMBER IN HLSL matrices are column major
	float4 cameraPos;
	float4 cameraForward;
	matrix vp;
};

struct Light
{

};

#define TOON_SHADING 0
#define COLOR 0

float4 PSMain(VSOutPSIn Input) : SV_TARGET0
{
	// lightning from right to left
	float4 lightDirection = normalize(float4(1.0, 1.0, 0.0, 0.0));

	//float4 pixelToCamera = (cameraPos - Input.vPosWS);
	//float4 pixelToLight = normalize(lightPositionWS - Input.vPosWS);

	float4 normalDecoded = normalize(Input.vNormalWS);
	normalDecoded.g *= -1.0;

	float cosLightNormal = dot(normalize(-lightDirection), normalDecoded);

#if !TOON_SHADING
	float lightningMethod = 0.6f + cosLightNormal;
#else
	float d = cosLightNormal;
	float lightningMethod  = 
		(d > 0.95 ? 1.0 :
			(d > 0.60 ? 0.8 :
				(d > 0.40 ? 0.55 : 
					(d > 0.12 ? 0.4 : 0.0)
				)
			)
		);
#endif

#if COLOR
	return lerp(float4(1.0, 0.0, 1.0, 0.0), float4(1.0, 1.0, 0.0, 0.0), lightningMethod);
#else
	return lerp(float4(0.0, 0.0, 0.0, 0.0), float4(1.0, 1.0, 1.0, 0.0), lightningMethod);
#endif
	//float hf = lightningMethod;//max(min((Input.vPosWS.y * 0.35), 1.0), 0.0);
	//return float4(hf,hf,hf,hf);
}
