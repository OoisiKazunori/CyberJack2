#pragma once
#include"../KazLibrary/Helper/KazBufferHelper.h"
#include"../KazLibrary/Helper/Compute.h"
#include"../KazLibrary/Render/DrawingByRasterize.h"
#include"../KazLibrary/Buffer/ShaderRandomTable.h"

class EnemyDeadParticle
{
public:
	EnemyDeadParticle(const KazBufferHelper::BufferData& arg_meshEmitterBuffer, const KazMath::Vec3<float>& arg_pos);
	void InitCompute();
	void UpdateCompute(DrawingByRasterize& arg_rasterize);

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
		UINT m_emitterTimer;
	};
	struct InitCommonData
	{
		DirectX::XMFLOAT3 pos;
		float m_particleNum;;
	};
	struct Particle
	{
		DirectX::XMFLOAT3 m_pos;
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
