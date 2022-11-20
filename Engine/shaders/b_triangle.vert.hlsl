struct VSOutput
{
	float4 Position : SV_Position;
};

VSOutput main(uint VertexIndex : SV_VertexID)
{
    const float3 positions[3] = {
		float3( 1.0f, 1.0f, 0.0f),
		float3(-1.0f, 1.0f, 0.0f),
		float3( 0.0f,-1.0f, 0.0f)
    };

	VSOutput output = (VSOutput)0;
	output.Position = float4(positions[VertexIndex], 1.0);
	return output;
}