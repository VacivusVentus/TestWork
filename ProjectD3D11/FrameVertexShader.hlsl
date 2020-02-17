struct OutputData
{
	float4 pos : SV_POSITION;
	float2 coord : COORD;
	float2 actcoord : ACTCOORD;
};

cbuffer ScreenSetting : register(b0)
{
	float4 mainvalues;
};

struct EL 
{
	uint4 elpos[5];
};

cbuffer ScrollBar : register(b1) 
{
	EL el;
};

OutputData main(uint uv : SV_VertexID)
{
	OutputData output;
	const float2 pos2[] = {
	float2(-1.0, -1.0),
	float2(1.0, -1.0),
	float2(1.0, 1.0),
	float2(-1.0, 1.0),
	};


	uint id = uint(uv) / 6;
	uint4 vp4 = el.elpos[id / 4];
	uint vpl[] = {vp4.x, vp4.y ,vp4.z ,vp4.w };
	uint idimg = vpl[id % 4];

	float xb = 1.0 / 14.0 * float(idimg % 14);
	float xe = 1.0 / 14.0 + xb;
	float yb = 1.0 / 4.0 * float(idimg / 14);
	float ye = 1.0 / 4.0 + yb;

	float2 lcoord[] = {

	float2(xb, 1.0 - yb),
	float2(xe, 1.0 - yb),
	float2(xe, 1.0 - ye),
	float2(xb, 1.0 - ye),
	};
	uint lpos[] = { 0, 2, 1, 0, 3, 2 };
	float mwh = min(mainvalues[0], mainvalues[1]);

	float fx = -0.525 + 0.105 + 0.21 * float(id % 5);
	float fy = -0.62 + 0.15 + 0.31 * float(id  / 5);


	float2 pp = pos2[lpos[uv % 6]];
	float2 buildpos = float2(pp.x * 0.1, pp.y * 0.15) + float2(fx, fy);
	output.actcoord = float2(buildpos.x + mainvalues[2], buildpos.y + mainvalues[3]);

	output.coord = lcoord[lpos[uv % 6]];
	float mx = mwh / mainvalues[0];
	float my = mwh / mainvalues[1];
	output.pos = float4((buildpos.x + mainvalues[2]) * mx, (buildpos.y + mainvalues[3]) * my, 1.0, 1.0);
	
	return output;
}