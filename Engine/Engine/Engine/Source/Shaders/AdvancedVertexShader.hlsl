cbuffer ModelViewProjectionConstantBuffer : register(b0)
{
	matrix mvp;
	matrix model;
	float3 lookat;
};

// Per-vertex data used as input to the vertex shader.
struct VertexShaderInput
{
	float3 pos : POSITION;
	float3 color : COLOR0;
	float3 normal : NORMAL0;
	float2 texcoord : UV0;
};

// Per-pixel color data passed through the pixel shader.
struct PixelShaderInput
{
	float4 pos : SV_POSITION;
	float3 worldPos : COLOR0;
	float3 color : COLOR1;
	float3 lookat : COLOR2;
	float3 normal : NORMAL0;
	float2 texcoord : UV0;
};

// Simple shader to do vertex processing on the GPU.
PixelShaderInput main(VertexShaderInput input)
{
	PixelShaderInput output;

	output.pos = mul(mvp, float4(input.pos, 1.0f));
	output.worldPos = mul(model, float4(input.pos, 1.0f));

	// Pass the color through without modification.
	output.color = input.color;
	output.lookat = lookat;
	output.texcoord = input.texcoord;
	output.normal = mul(model, float4(input.normal, 1.0)).xyz;

	return output;
}
