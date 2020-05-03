// Per-pixel color data passed through the pixel shader.
struct PixelShaderInput
{
	float4 pos : SV_POSITION;
	float4 depth_color : COLOR0;
};

// A pass-through function for the (interpolated) color data.
float4 main(PixelShaderInput input) : SV_TARGET0
{
	const float d = input.depth_color.z / input.depth_color.w;
	return float4(d, d, d, 1.0f);
}
