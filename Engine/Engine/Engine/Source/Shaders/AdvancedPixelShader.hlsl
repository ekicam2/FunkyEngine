// Per-pixel color data passed through the pixel shader.
Texture2D  Texture1  : register (t0);
TextureCube SkyTexture : register(t1);

SamplerState MySampler : register (s0);

struct PixelShaderInput
{
	float4 pos : SV_POSITION;
	float3 worldPos : COLOR0;
	float3 color : COLOR1;
	float3 lookat : COLOR2;
	float3 normal : NORMAL0;
	float2 texcoord : UV0;
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

// A pass-through function for the (interpolated) color data.
float4 main(PixelShaderInput input) : SV_TARGET0
{
	float3 OutColor = Texture1.Sample(MySampler, input.texcoord).xyz;
	
	float ambientIntensity = 0.45;
	float3 ambientColor = ambientIntensity * SkyTexture.Sample(MySampler, (reflect(normalize(input.lookat), normalize(input.normal)))).xyzw;

	float3 norm = normalize(input.normal);

	float3 toLight = normalize(PointLight.Position.xyz - input.worldPos);
	float diffuseIntensity = max(dot(toLight, norm), 0.0);
	float3 diffuseColor = diffuseIntensity * PointLight.Color.xyz;

	float3 reflectedToFrag = reflect(-toLight, norm);
	float specVal = pow(max(dot(input.lookat, reflectedToFrag), 0.0), 32.);
	float3 specular = specVal * PointLight.Color.xyz * 0.5;

	return float4((diffuseColor + specular) * ambientColor * OutColor, 1.0);
}
