#include "object3d.hlsli"

struct VertexShaderInput {
	float32_t4 position : POSITION0;
    float32_t2 texcord : TEXCOORD0;
    float32_t3 normal : NORMAL0;
};

VertexShaderOutput main(VertexShaderInput input) {
	VertexShaderOutput output;
    output.position = input.position;
    output.texcord = input.texcord;
    output.normal = normalize(mul(input.normal, (float32_t3x3)WorldInverseTranspose));
    output.worldPosition = mul(input.position, World).xyz;
	return output;
}