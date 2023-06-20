#pragma once
#include"../KazLibrary/Math/KazMath.h"

///<summary>
///�o�C�i���[�ɂǂꂭ�炢�͈͕̔��m�ۂ�����
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
///���̎����̗�������ׂ̃A���S���Y��
///https://www.docswell.com/s/CAPCOM_RandD/Z7DPJK-2022-07-15-133910#p25
///</summary>
class CPUParticle
{
public:
	CPUParticle();

	///<summary>
	///�G�t�F�N�g��������(CPU)
	///</summary>
	void GenerateEffect();

	///<summary>
	///�G�~�b�^�[�X�V(CPU)
	///</summary>
	void UpdateEmitter();

	///<summary>
	///GPU�ł̃G�~�b�^�[�ƃp�[�e�B�N���̍X�V(GPU)
	///</summary>
	void UpdateEmitterAndParticleOnGPU();

private:
	static const int EMITTER_MAX_NUM = 3;
	static const int PARTICLE_MAX_NUM = 3;

	//CPU����GPU�֏������܂��o�b�t�@---------------------------------------

	std::array<int, EMITTER_MAX_NUM>m_emitterTableArray;
	std::array<EmitterHeaderData, EMITTER_MAX_NUM>m_emitterHeaderArray;

	//CPU����GPU�֏������܂��o�b�t�@---------------------------------------

	//GPU���݂̂ŏ������܂��o�b�t�@---------------------------------------

	std::array<std::vector<int>, EMITTER_MAX_NUM>m_particleIndexListArray;
	std::vector<std::vector<ParticleHeaderData>>m_particleHeaderArray;

	//GPU���݂̂ŏ������܂��o�b�t�@---------------------------------------


	//emitterBinary��particleBinary�͔��f���ނ����B�ǂ�����đg�ݍ��ނ񂾂낤�B
	//vector�͋֎~�B�V�F�[�_�[�ł͉ϒ��ł͖�������(�������������Ă�)

};

