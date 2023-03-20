#include"../ShaderHeader/FbxShaderHeader.hlsli"

VSOutput VSmain(VSInput input)
{
	//スキニング計算
    SkinOutPut skinned = ComputeSkin(input);
	//法線にワールド行列によるスケーリング
    float3 wnormal = normalize(mul(world, float4(skinned.normal, 0))).xyz;
    
    VSOutput op;
    //ワールド法線を次のステージに渡す
    op.svpos = mul(world, skinned.pos);
    op.svpos = mul(view, op.svpos);
    op.svpos = mul(viewproj, op.svpos);

    op.normal = wnormal.xyz;
	op.uv = input.uv;
	return op;
}
