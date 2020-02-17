struct PixelInput 
{
	float4 pos : SV_POSITION;
	float2 coord : COORD;
};

float4 main(PixelInput input) : SV_TARGET
{
	float x = cos(input.coord.x) * sin(input.coord.y);
	float y = cos(input.coord.y) * sin(input.coord.x);
	return float4(max(0.0, x * 0.5), max(0.0, y * 0.5), max(0.0, min(1.0, x + y) * 0.5),1.0);
}