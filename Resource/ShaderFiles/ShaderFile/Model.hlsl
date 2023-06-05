cbuffer MatBuffer : register(b0)
{
    matrix mat; //3D変換行列
}

//uv、法線対応---------------------------------------
struct PosUvNormalOutput
{
    float4 svpos : SV_POSITION; //システム用頂点座標
    float3 normal : NORMAL; //法線ベクトル
    float2 uv : TEXCOORD;
};

PosUvNormalOutput VSPosNormalUvmain(float4 pos : POSITION,float3 normal : NORMAL,float2 uv:TEXCOORD)
{
    PosUvNormalOutput op;
    op.svpos = mul(mat,pos);
    op.uv = uv;
    op.normal = normal;
    return op;
}

float4 PSPosNormalUvmain(PosUvNormalOutput input) : SV_TARGET
{
	float4 texColor = float4(input.uv.x,input.uv.y,1,1);
	return float4(texColor.rgb, 1.0f);
}
//uv、法線対応---------------------------------------