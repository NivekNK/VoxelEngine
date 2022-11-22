struct VSOutput {
    float4 Position : SV_Position;
    [[vk::location(0)]] float3 Color : COLOR0;
};

VSOutput main(uint VertexIndex : SV_VertexID) {
    //const array of positions for the triangle
	const float2 positions[3] = {
		float2( 0.0f,-0.5f),
		float2( 0.5f, 0.5f),
		float2(-0.5f, 0.5f)
    };

	//const array of colors for the triangle
	const float3 colors[3] = {
		float3(1.0f, 0.0f, 0.0f), //red
		float3(0.0f, 1.0f, 0.0f), //green
		float3(0.0f, 0.0f, 1.0f)  //blue
    };

	VSOutput output = (VSOutput)0;
	output.Position = float4(positions[VertexIndex], 0.0f, 1.0f);
	output.Color = colors[VertexIndex];
	return output;
}