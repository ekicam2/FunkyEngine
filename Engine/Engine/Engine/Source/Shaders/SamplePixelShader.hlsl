// Per-pixel color data passed through the pixel shader.
Texture2D  ShadowMap  : register (t0);
SamplerState MySampler : register (s0);

SamplerComparisonState cmpSampler
{
	Filter = COMPARISON_MIN_MAG_MIP_LINEAR;
	AddressU = WRAP;
	AddressV = WRAP;
	ComparisonFunc = ALWAYS;
};

struct PixelShaderInput
{
	float4 pos : SV_POSITION;
	float4 shadow_pos : POSITION0;
	float3 color : COLOR0;
};

struct Light
{
	float4 Position;
	float4 Direction;
	float4 Color;
	float Intensity;
};

cbuffer Lights : register(b0)
{
	Light DirLight;
	Light Lights[5];
}

// A pass-through function for the (interpolated) color data.
float4 main(PixelShaderInput input) : SV_TARGET0
{
	
	float3 fragment_position_shadow_camera;
	fragment_position_shadow_camera.x = input.shadow_pos.x / input.shadow_pos.w / 2.0f + 0.5f;
	fragment_position_shadow_camera.y = -input.shadow_pos.y / input.shadow_pos.w / 2.0f + 0.5f;
	fragment_position_shadow_camera.z = input.shadow_pos.z / input.shadow_pos.w;

	float depth = ShadowMap.Sample(MySampler, fragment_position_shadow_camera.xy).r;
	// return float4(depth, depth, depth, 1.0f);

	float4 color = float4(input.color * 0.25f, 1.0f);

	if (
		(saturate(fragment_position_shadow_camera.x) == fragment_position_shadow_camera.x)
		&& 
		(saturate(fragment_position_shadow_camera.y) == fragment_position_shadow_camera.y)
		&& 
		(saturate(fragment_position_shadow_camera.z) == fragment_position_shadow_camera.z)
		)
	{
		color = float4(input.color, 1.0f);

		fragment_position_shadow_camera.z -= 0.001f;

		if (fragment_position_shadow_camera.z > depth)
		{
			color *= 0.25f;
		}
	}

	return color;
}
