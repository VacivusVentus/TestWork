Texture2D tex : register(t0);
SamplerState sampl : register(s0);


struct InputData
{
	float4 pos : SV_POSITION;
	float2 coord : COORD;
	float2 actcoord : ACTCOORD;
};

float4 main(InputData inp) : SV_TARGET
{
	float4 color = tex.Sample(sampl, inp.coord);
	color.w *= 1.0 - float(abs(inp.actcoord.x) > 0.325 || abs(inp.actcoord.y) > 0.27);
	return color;
}