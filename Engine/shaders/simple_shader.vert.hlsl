struct VSInput
{
    [[vk::location(0)]] float2 Position : POSITION0;
};

struct VSOutput {
    float4 Position : SV_Position;
};

VSOutput main(VSInput input) {
	VSOutput output = (VSOutput)0;
	output.Position = float4(input.Position, 0.0f, 1.0f);
	return output;
}