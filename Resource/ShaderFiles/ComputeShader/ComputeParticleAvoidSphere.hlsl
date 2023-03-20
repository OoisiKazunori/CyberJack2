#include"../ShaderHeader/KazMathHeader.hlsli"
#include"../ShaderHeader/MeshCollision.hlsli"
#include"../ShaderHeader/GPUParticle.hlsli"


//パーティクル当たり判定
RWStructuredBuffer<ParticleHitBoxData> hitBoxData : register(u0);
//当たったインデックス
RWStructuredBuffer<MeshSphereHitData> hitIndexData : register(u1);
//出力
RWStructuredBuffer<ParticleHitData> inputGPUParticleData : register(u2);

RWStructuredBuffer<float3> larpPosData : register(u3);

cbuffer RootConstants : register(b0)
{
    uint hitMaxNum;
};

bool BoolUint3(uint3 A,uint3 B)
{
    return A.x == B.x && A.y == B.y && A.z == B.z;
}

//周りの当たり判定ともとる
bool CheckLinkHitBox(uint3 BASE_HIT_INDEX,uint3 CHECK_HIT_INDEX)
{
    bool linkHitFlagArray[7];
    linkHitFlagArray[0] = BoolUint3(CHECK_HIT_INDEX, BASE_HIT_INDEX);                    //真ん中
    linkHitFlagArray[1] = BoolUint3(CHECK_HIT_INDEX, BASE_HIT_INDEX + uint3(0,1,0));  //上
    linkHitFlagArray[2] = BoolUint3(CHECK_HIT_INDEX, BASE_HIT_INDEX + uint3(0,-1,0)); //下
    linkHitFlagArray[3] = BoolUint3(CHECK_HIT_INDEX, BASE_HIT_INDEX + uint3(-1,0,0)); //左
    linkHitFlagArray[4] = BoolUint3(CHECK_HIT_INDEX, BASE_HIT_INDEX + uint3(1,0,0));  //右
    linkHitFlagArray[5] = BoolUint3(CHECK_HIT_INDEX, BASE_HIT_INDEX + uint3(0,0,1));  //前
    linkHitFlagArray[6] = BoolUint3(CHECK_HIT_INDEX, BASE_HIT_INDEX + uint3(0,0,-1)); //後

    for(int i = 0;i < 7; ++i)
    {
        if(linkHitFlagArray[i])
        {
            return true;
        }
    }
    return false;
}

//メッシュパーティクルの球判定を取ったインデックスを参考にパーティクルの挙動を決める
[numthreads(1024, 1, 1)]
void CSmain(uint3 groupId : SV_GroupID, uint groupIndex : SV_GroupIndex,uint3 groupThreadID : SV_GroupThreadID)
{
    uint index = ThreadGroupIndex(groupId,groupIndex,groupThreadID,1024);

    ParticleHitData particleData;
    particleData.pos = hitBoxData[index].pos;
    particleData.color = hitBoxData[index].color;
    particleData.id = hitBoxData[index].meshID;


    particleData.hitFlag = inputGPUParticleData[index].hitFlag;
    particleData.hitTimer = inputGPUParticleData[index].hitTimer;

    float larpVel = 0.01f;
    float3 basePos = hitBoxData[index].pos;

    //同じインデックスの場合、パーティクルを動かす処理を追加する
    for(int i = 0;i < hitMaxNum; ++i)
    {
        //メッシュIDの確認
        if(particleData.id != hitIndexData[i].meshID)
        {
            continue;
        }
        //当たり判定+リンク付け込みの処理を取る
        if(CheckLinkHitBox(hitIndexData[i].id,hitBoxData[index].id))
        {    
            float3 posHitBoxVec = hitIndexData[i].circlePos - hitIndexData[i].meshPos;
            float3 particleHitPos = hitBoxData[index].pos - hitIndexData[i].meshPos;
            float3 posParticleVec = posHitBoxVec - particleHitPos;

            //なす角の判断
            float cos = dot(posParticleVec,posHitBoxVec) / (length(posParticleVec) * length(posHitBoxVec));
            float sin = acos(cos);
    
            int angle = RadianToAngle(sin) - 90;
            float rate = angle / 90.0f;

            posParticleVec.x = 0.0f;
            float3 vel = normalize(posParticleVec) * rate;
            float3 mulVel = float3(0.0f,15.5f,15.5f);
            larpVel = 0.1f;
            basePos = hitBoxData[index].pos + vel * mulVel;
            
            //パーティクル情報の描画,当たったかどうかも表示する
            //particleData.color = float4(0.89, 0.50, 0.07,1);
            particleData.hitFlag = 1;
            particleData.hitTimer = 0;
            break;
        }
    }
    //行列計算ーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーー
    
    larpPosData[index] = Larp(basePos,larpPosData[index],larpVel);

    //あたり判定の光らせ
    if(particleData.hitFlag)
    {
        particleData.color = float4(0.0, 0.8, 0.8,1.0);
        particleData.hitTimer += 1;
        if(1 <= particleData.hitTimer)
        {
            particleData.hitFlag = 0;
            particleData.hitTimer = 0;
        }
    }
    
    particleData.pos = larpPosData[index];
    inputGPUParticleData[index] = particleData;
}
