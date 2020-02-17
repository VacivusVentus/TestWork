struct OutData 
{ 
	float4 pos : SV_POSITION;
	float3 vcolor : BASECOLOR;
};

cbuffer MainBuffer : register(b0)
{
	float4 screen;
	uint4 activeEl;
};

OutData main(uint uv : SV_VertexID)
{
	OutData od;
	float mwh = min(screen[0], screen[1]);

	float mx = mwh / screen[0];
	float my = mwh / screen[1];
	const float2 triangleVer[] = 
	{
		float2(-0.2, 0.35),
		float2(0.0, 0.42),
		float2(0.2, 0.35),

		float2(-0.2, -0.35),
		float2(0.2, -0.35),
		float2(0.0, -0.42),

		float2(-0.35, -0.2),
		float2(-0.42, 0.0),
		float2(-0.35, 0.2),

		float2(0.35, -0.2),
		float2(0.35, 0.2),
		float2(0.42, 0.0),
	};
	float2 v = triangleVer[uv];
	uint sel[] = { activeEl.x, activeEl.y ,activeEl.z ,activeEl.w };
	od.pos = float4(v.x * mx, v.y * my, 0.0, 1.0);
	float3 lcolor[] = {
		float3(0.2, 0.2, 0.2),
		float3(clamp(0.5 + v.x, 0.2, 0.8), clamp(0.5 + v.y, 0.2, 0.8), clamp(0.5 + v.y * v.x, 0.2, 0.8)),
	};
	od.vcolor = lcolor[sel[uv / 3]];
	return od;
}