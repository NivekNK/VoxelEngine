struct VSInput
{
    [[vk::location(0)]] float3 Color : COLOR0;
};

struct VSOutput
{
    [[vk::location(0)]] float4 Color : SV_Target;
};

VSOutput main(VSInput input)
{
	VSOutput output = (VSOutput)0;
	output.Color = float4(input.Color, 1.0f);
	return output;
}