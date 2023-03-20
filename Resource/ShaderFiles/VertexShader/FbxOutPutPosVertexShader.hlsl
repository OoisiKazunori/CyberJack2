#include"../ShaderHeader/FbxShaderHeader.hlsli"
#include"../ShaderHeader/FogShaderHeader.hlsli"

FogOutPut VSmain(VSInput input)
{
	//スキニング計算
    SkinOutPut skinned = ComputeSkin(input);

    FogOutPut op;
    //ワールド法線を次のステージに渡す
    op.svpos = mul(world, skinned.pos);
    op.fogFactor = op.svpos.xyz;
    op.svpos = mul(view, op.svpos);
    op.svpos = mul(viewproj, op.svpos);

	return op;
}


