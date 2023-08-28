#pragma once
#include"../KazLibrary/Helper/KazBufferHelper.h"
#include"../KazLibrary/Helper/Compute.h"
#include"../KazLibrary/Render/DrawingByRasterize.h"
#include"../KazLibrary/Buffer/ShaderRandomTable.h"

class EnemyDeadParticle
{
public:
	EnemyDeadParticle(const KazBufferHelper::BufferData& arg_meshEmitterBuffer, const KazMath::Vec3<float>& arg_pos);
	void InitCompute(const KazMath::Vec3<float>& arg_pos,int particleNum);
	void UpdateCompute(DrawingByRasterize& arg_rasterize);
	KazMath::Vec3<float>m_pos;
	KazMath::Color m_color;
	float m_vel;
private:
	int m_timer;
	ComputeShader m_init, m_update;
	KazBufferHelper::BufferData m_emitterBuffer, m_outputBuffer;
	KazBufferHelper::BufferData m_initCommonBuffer, m_updateCommonBuffer;

	struct UpdateCommonData
	{
		DirectX::XMMATRIX m_viewProjMat;
		DirectX::XMMATRIX m_billboard;
		DirectX::XMMATRIX m_scaleRotaMat;
		DirectX::XMFLOAT4 emittPosAndEmittTimer;
		DirectX::XMFLOAT4 color;
		float m_vel;
	};
	struct InitCommonData
	{
		DirectX::XMFLOAT3 pos;
		float m_particleNum;
	};
	struct Particle
	{
		DirectX::XMFLOAT3 m_pos;
		DirectX::XMFLOAT3 m_scale;
		DirectX::XMFLOAT3 m_localPos;
		int m_emittTimer;
		int m_timer;
		int m_maxTimer;
	};
	struct OutputData
	{
		DirectX::XMMATRIX m_worldMat;
		DirectX::XMFLOAT4 m_color;
	};

	static const int PARTICLE_MAX_NUM = 1024 * 1;

	DrawFuncData::DrawCallData m_executeIndirect;

};
