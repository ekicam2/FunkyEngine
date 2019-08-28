// Per-pixel color data passed through the pixel shader.
TextureCube SkyTexture : register(t0);
SamplerState MySampler : register (s0);

struct PixelShaderInput
{
	float4 pos : SV_POSITION;
	float3 color : COLOR0;
	float3 texcoord : UV0;
};

float4 main(PixelShaderInput input) : SV_TARGET0
{
	float3 OutColor = SkyTexture.Sample(MySampler, input.texcoord).xyz;

	return SkyTexture.Sample(MySampler, input.texcoord);// float4(OutColor, 1.0f);
}
