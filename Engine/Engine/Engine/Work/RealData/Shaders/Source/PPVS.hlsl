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
	Out.uv = Out.uv * 0.5 +  0.5;
	//Out.uv.y = 1.0 - Out.uv.y;

	return Out;
}