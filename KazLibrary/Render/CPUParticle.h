#pragma once
#include"../KazLibrary/Math/KazMath.h"

///<summary>
///バイナリーにどれくらいの範囲分確保したか
///</summary>
struct DataSize
{
	int m_head, m_size;
};

///<summary>
///
///</summary>
struct EmitterHeaderData
{
	DataSize m_emitterBinarySize, m_particleBinarySize;
};

struct ParticleHeaderData
{
	bool m_isAliveFlag;
	int m_emitterID;
	int m_particleIndex;
	float m_depth;
};

///<summary>
///
///</summary>
struct EmitterBinaryData
{

};

///<summary>
///
///</summary>
struct ParticleBinaryData
{

};

///<summary>
///この資料の理解する為のアルゴリズム
///https://www.docswell.com/s/CAPCOM_RandD/Z7DPJK-2022-07-15-133910#p25
///</summary>
class CPUParticle
{
public:
	CPUParticle();

	///<summary>
	///エフェクト生成処理(CPU)
	///</summary>
	void GenerateEffect();

	///<summary>
	///エミッター更新(CPU)
	///</summary>
	void UpdateEmitter();

	///<summary>
	///GPUでのエミッターとパーティクルの更新(GPU)
	///</summary>
	void UpdateEmitterAndParticleOnGPU();

private:
	static const int EMITTER_MAX_NUM = 3;
	static const int PARTICLE_MAX_NUM = 3;

	//CPUからGPUへ書き込まれるバッファ---------------------------------------

	std::array<int, EMITTER_MAX_NUM>m_emitterTableArray;
	std::array<EmitterHeaderData, EMITTER_MAX_NUM>m_emitterHeaderArray;

	//CPUからGPUへ書き込まれるバッファ---------------------------------------

	//GPU内のみで書き込まれるバッファ---------------------------------------

	std::array<std::vector<int>, EMITTER_MAX_NUM>m_particleIndexListArray;
	std::vector<std::vector<ParticleHeaderData>>m_particleHeaderArray;

	//GPU内のみで書き込まれるバッファ---------------------------------------


	//emitterBinaryとparticleBinaryは反映がむずい。どうやって組み込むんだろう。
	//vectorは禁止。シェーダーでは可変長では無いため(そう見せかけてる)

};

