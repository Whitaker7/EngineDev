#include "mvp.hlsli"

struct VSIn
{
	float3 pos : POSITION;
	float3 normal : NORMAL;
	float2 uv : TEXCOORD;
};

struct VSOut
{
	float4 pos : SV_POSTITION;
	float4 normal : NORMAL;
	float4 color : COLOR;
};

VSOut main(VSIn input)
{
	VSOut output;

	output.pos = mul(float4(input.pos, 1.0f), modeling);
	output.pos = mul(output.pos, view);
	output.pos = mul(output.pos, projection);

	output.normal = mul(float4(input.normal, 0.0f), modeling);

	output.color = float4((output.normal.xyz + 1.0f) * 0.5f, 1.0f) * 0.75f;

	return output;
}