struct VSOutput
{
    [[vk::location(0)]] float4 Color : SV_Target;
};

VSOutput main()
{
	VSOutput output = (VSOutput)0;
	output.Color = float4(1.0f, 1.0f, 1.0f, 1.0f);
	return output;
}