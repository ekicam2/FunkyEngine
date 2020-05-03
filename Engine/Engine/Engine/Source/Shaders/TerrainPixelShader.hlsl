// Per-pixel color data passed through the pixel shader.
struct PixelShaderInput
{
	float4 pos : SV_POSITION;
	float3 color : COLOR1;
};

struct Light
{
	float4 Color;
	float3 Position;
};

cbuffer Lights : register(b0)
{
	Light PointLight;
}

float4 main(PixelShaderInput input) : SV_TARGET0
{
	return float4(input.color, 1.0f);
}
