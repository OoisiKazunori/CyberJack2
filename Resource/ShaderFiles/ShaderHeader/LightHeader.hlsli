struct LightVSOutput 
{
    float4 svpos:SV_POSITION;	//システム用頂点座標
    float3 normal:NORMAL;		//法線ベクトル
    float3 worldPos:TEXCOORD;   //何故TEXCOORD...?
};

cbuffer light : register(b1)
{
    // ディレクションライト用のデータ
    float3 dirDirection;    // ライトの方向
    float3 dirColor;        // ライトのカラー

    // 定数バッファーにポイントライト用の変数を追加
    float3 ptPosition;      // ポイントライトの位置
    float3 ptColor;         // ポイントライトのカラー
    float ptRange;          // ポイントライトの影響範囲

    float3 spPosition;      // スポットライトの位置
    float3 spColor;         // スポットライトのカラー
    float spRange;          // スポットライトの射出範囲
    float3 spDirection;     // スポットライトの射出方向
    float spAngle;          // スポットライトの射出角度

    float3 eyePos;          // 視点の位置

    float3 ambientLight;    // アンビエントライト
};

//struct DirectionalLight
//{
//    float3 direction;   // ライトの方向
//    float4 color;       // ライトの色
//};
//
//// ライト用の定数バッファー
//cbuffer LightCb : register(b1)
//{
//    DirectionalLight directionalLight[4];
//    float3 eyePos;          // カメラの視点
//    float specPow;          // スペキュラの絞り
//    float3 ambientLight;    // 環境光
//};


float3 CalcLambertDiffuse(float3 lightDirection, float3 lightColor, float3 normal)
{
    // ピクセルの法線とライトの方向の内積を計算する
    float t = dot(normal, lightDirection) * -1.0f;

    // 内積の値を0以上の値にする
    t = max(0.0f, t);

    // 拡散反射光を計算する
    return lightColor * t;
}

float3 CalcPhongSpecular(float3 lightDirection, float3 lightColor, float3 worldPos, float3 normal)
{
    // 反射ベクトルを求める
    float3 refVec = reflect(lightDirection, normal);

    // 光が当たったサーフェイスから視点に伸びるベクトルを求める
    float3 toEye = eyePos - worldPos;
    toEye = normalize(toEye);

    // 鏡面反射の強さを求める
    float t = dot(refVec, toEye);

    // 鏡面反射の強さを0以上の数値にする
    t = max(0.0f, t);

    // 鏡面反射の強さを絞る
    t = pow(t, 5.0f);

    // 鏡面反射光を求める
    return lightColor * t;
}


Texture2D<float4> mainRenderTargetTexture : register(t0); // メインレンダリングターゲットのテクスチャ
sampler Sampler : register(s0);
