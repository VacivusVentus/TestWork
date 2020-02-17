struct IntData
{
	float4 pos : SV_POSITION;
	float3 vcolor : BASECOLOR;
};

float4 main(IntData inp) : SV_TARGET
{
	return float4(inp.vcolor, 1.0);
}