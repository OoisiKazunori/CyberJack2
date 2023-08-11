
//入力情報
RWTexture2D<float4> BackBuffer : register(u0);

//入力情報
RWTexture2D<float4> Raytracing : register(u1);

//出力先UAV  
RWTexture2D<float4> OutputImg : register(u2);

[numthreads(16, 16, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    
    //背景色だったらレイトレの結果を描画
    bool isBackGroundColor = 0.8f <= BackBuffer[DTid.xy].r && BackBuffer[DTid.xy].g <= 0.01f && BackBuffer[DTid.xy].b <= 0.01f;
    //レイトレの色の赤成分が強かったら一旦敵を描画する。
    bool isEnemyColor = 0.3f < Raytracing[DTid.xy].x && Raytracing[DTid.xy].y < 0.5f && Raytracing[DTid.xy].z < 0.5f;
    if (isBackGroundColor || isEnemyColor)
    {
    
        OutputImg[DTid.xy] = Raytracing[DTid.xy];
        
    }
    else
    {
        
        OutputImg[DTid.xy] = BackBuffer[DTid.xy];
    
    }
    
}