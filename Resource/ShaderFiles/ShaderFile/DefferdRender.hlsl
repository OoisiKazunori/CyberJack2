/*
単色表示の処理
*/

//共通処理---------------------------------------
cbuffer MatBuffer : register(b0)
{
    matrix mat; //3D変換行列
}
cbuffer ColorBuffer : register(b1)
{
    float4 color; //色
}
//共通処理---------------------------------------

//GBuffer
RWTexture2D<float4> albedoGBuffer : register(u0);
RWTexture2D<float4> normalGBuffer : register(u1);


//単色対応---------------------------------------
struct ColorOutput
{
    float4 svpos : SV_POSITION; //システム用頂点座標
    float4 color : COLOR;
};    

ColorOutput VSmain(float4 pos : POSITION)
{
    ColorOutput op;
    op.svpos = mul(mat,pos);
    op.color = color;
    return op;
}

float4 PSmain(ColorOutput input) : SV_TARGET
{    
    input.color.a = 1.0f;
    albedoGBuffer[input.svpos.xy] = input.color;
    return input.color;
}
//単色対応---------------------------------------



//色、法線対応---------------------------------------
struct ColorNormalOutput
{
    float4 svpos : SV_POSITION; //システム用頂点座標
    float4 color : COLOR;
    float3 normal : NORMAL; //法線ベクトル
};

ColorNormalOutput VSPosNormalmain(float4 pos : POSITION,float3 normal : NORMAL)
{
    ColorNormalOutput op;
    op.svpos = mul(mat,pos);
    op.color = color;
    op.normal = normal;
    return op;
}

float4 PSPosNormalmain(ColorNormalOutput input) : SV_TARGET
{
    float3 light = normalize(float3(1, -1, 1));
    float diffuse = saturate(dot(-light, input.normal));
    float brightness = diffuse + 0.3f;

    input.color.a = 1.0f;
    albedoGBuffer[input.svpos.xy] = input.color;
    normalGBuffer[input.svpos.xy] = float4(input.normal.x, input.normal.y, input.normal.z, 1);

    return input.color * float4(brightness, brightness, brightness, 1);
}
//色、法線対応---------------------------------------



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


Texture2D<float4>tex:register(t0);
SamplerState smp :register(s0);

cbuffer MaterialBuffer :register(b1)
{
    float3 ambient;
    float3 diffuse;
    float3 specular;
    float alpha;
}

float4 PSPosNormalUvmain(PosUvNormalOutput input) : SV_TARGET
{
	float3 light = normalize(float3(1,-1,1));			//右下奥向きのライト
	float light_diffuse = saturate(dot(-light, input.normal));//環境光...diffuseを[0,1]の範囲にClampする
	
	float3 shade_color;
	shade_color = ambient + 0.5;			  //アンビエント
	shade_color += diffuse * light_diffuse; //ディフューズ項

	float4 texColor = tex.Sample(smp, input.uv);
	return float4(texColor.rgb, alpha);
}
//uv、法線対応---------------------------------------