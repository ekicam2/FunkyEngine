// A constant buffer that stores the three basic column-major matrices for composing geometry.
cbuffer ModelViewProjectionConstantBuffer : register(b0)
{
	matrix mvp;
	matrix model;
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
	float4 depth_color : COLOR0;
};

// Simple shader to do vertex processing on the GPU.
PixelShaderInput main(VertexShaderInput input)
{
	PixelShaderInput output;

	output.pos = mul(mvp, float4(input.pos, 1.0f));

	// Pass the color through without modification.
	output.depth_color = output.pos;

	return output;
}
