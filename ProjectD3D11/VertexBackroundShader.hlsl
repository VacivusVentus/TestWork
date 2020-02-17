#define MAX_COORD 3.14
struct VertexPOutput
{
	float4 pos : SV_POSITION;
	float2 coord : COORD;
};

cbuffer ConstVertexForGeneratePixel : register(b0)
{
	float4 values;
};

VertexPOutput main(uint uv : SV_VertexID)
{
	const float2 pos2[] = {
	float2(-1.0, -1.0),
	float2(1.0, -1.0),
	float2(1.0, 1.0),
	float2(-1.0, 1.0),
	};
	float lx = values[0] / 600.0;
	float ly = values[1] / 600.0;
	float2 coordlst[] = {

	float2(-MAX_COORD * lx + values[2], -MAX_COORD * ly + values[2]),
	float2(MAX_COORD * lx + values[2], -MAX_COORD * ly + values[2]),
	float2(MAX_COORD * lx + values[2], MAX_COORD * ly + values[2]),
	float2(-MAX_COORD * lx + values[2], MAX_COORD * ly + values[2]),
	};
	VertexPOutput outvalue;
    uint lpos[] = {0, 2, 1, 0, 3, 2};
	outvalue.pos = float4(pos2[lpos[uv % 6]], 0.0, 1.0);
	outvalue.coord = float4(coordlst[lpos[uv % 6]], 0.0, 1.0);
	return outvalue;
}