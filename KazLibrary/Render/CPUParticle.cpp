#include "CPUParticle.h"
#include "CPUParticle.h"

CPUParticle::CPUParticle()
{
	//�p�[�e�B�N���̃V�X�e���S�̂̏���������

	//�p�[�e�B�N���̃N���A����
	for (int i = 0; i < EMITTER_MAX_NUM; ++i)
	{
		m_emitterTableArray[i] = -1;
	}

}

void CPUParticle::GenerateEffect()
{
	//�G�~�b�^�[����

}

void CPUParticle::UpdateEmitter()
{

}

void CPUParticle::UpdateEmitterAndParticleOnGPU()
{
	/*
	Begin Update
	�O�t���[���Ɣ����\��̃p�[�e�B�N���������đ������v�Z����
	*/

	/*
	Fill Unused Index
	�V�K�G�~�b�^�[���ǉ����ꂽ��APart�V�������蓖�Ă�̈�ɏ�������������(-1�Ŗ��߂�)
	�̈�͖�������ǉ����Ă��� -> emplance_back����ƌ�����
	*/

	/*
	Spawn Particles
	�V������������p�[�e�B�N�����̌���@���@ParticleHeader�ɖ�������ǉ�����B
	EmitterRange�̏�����
	(�ǂ������Head�̈ʒu���������)
	*/

	/*
	Initialize Particles

	Dispatch(�G�~�b�^�[�̐�,1,1)
	*/

	/*
	Update Particle
	1�X���b�h��1�p�[�e�B�N���̍X�V
	ParticleHeader���Q�Ƃ��čX�V�������鎖�Ő������Ă���p�[�e�B�N���̂ݏ����ł���
	UberShader(�����̋@�\����̃V�F�[�_�[�ɓZ�߂�)�I�ɑS�A�C�e���̎��s

	��R�̊֐���p�ӂ��Ď��s���Ă���...Item�̎�ނ��L���݂������B�p�ӂ��ꂽ�֐��������o���Ȃ����낤�B
	�ړ��ʂ̏��������s���Ă����肷��̂�
	*/

	/*
	Bitonic sort
	�p�[�e�B�N����Alive,EmitterID,Depth�̏��Ń\�[�g��������
	*/

	/*
	Range Particle
	�G�~�b�^�[�P�ʂŐ����Ă���p�[�e�B�N���͈̔͂Ǝ���ł���p�[�e�B�N���͈̔͂����߂�
	*/

	/*
	Terminate Particles
	���񂾃p�[�e�B�N����Index�̕ԋp
	Particle Index List�Ɏg���Ă鏊�ɏ�������
	*/

	/*
	Build Emitter Draw Arg
	EmitterRange����p�[�e�B�N���̐������߂�EmitterData���X�V
	*/

	/*
	Build Primitive
	�p�[�e�B�N����񂩂璸�_�𐶐�����B
	1�X���b�h1�p�[�e�B�N��
	*/

}
