#include "BlasVector.h"
#include "Blas.h"

namespace Raytracing
{

	BlasVector::BlasVector()
	{
	}

	void BlasVector::Update()
	{
		m_refBlas.resize(0);
		m_instanceDesc.resize(0);
	}

	void BlasVector::Add(std::weak_ptr<Blas> arg_refBlas, const DirectX::XMMATRIX& arg_worldMat, int arg_instanceIndex, bool arg_isAddBlas)
	{

		/*===== Tlasに登録するために配列に追加する =====*/

		//参照を追加して。
		int hitGroupSize = static_cast<int>(m_refBlas.size());
		if (arg_isAddBlas) {
			m_refBlas.emplace_back(arg_refBlas);
		}
		else {
			--hitGroupSize;
		}


		D3D12_RAYTRACING_INSTANCE_DESC instanceDesc;

		//行列を設定。
		XMStoreFloat3x4(
			reinterpret_cast<DirectX::XMFLOAT3X4*>(&instanceDesc.Transform),
			arg_worldMat);

		//インスタンスの詳細を設定。
		instanceDesc.InstanceID = arg_instanceIndex;			//レイトレで行う処理のフラグをここで設定する。マテリアル側で設定してもよい。
		instanceDesc.InstanceMask = 0xFF;
		instanceDesc.InstanceContributionToHitGroupIndex = hitGroupSize;
		instanceDesc.Flags = D3D12_RAYTRACING_INSTANCE_FLAG_NONE;
		instanceDesc.AccelerationStructure = m_refBlas.back().lock()->GetBlasBuffer()->GetGPUVirtualAddress();

		//インスタンスを追加。
		m_instanceDesc.emplace_back(instanceDesc);

	}

	int BlasVector::GetBlasRefCount()
	{
		return static_cast<int>(m_refBlas.size());
	}

	int BlasVector::GetInstanceCount()
	{
		return static_cast<int>(m_instanceDesc.size());
	}

	uint8_t* BlasVector::WriteShaderRecord(uint8_t* arg_dest, UINT arg_recordSize, Microsoft::WRL::ComPtr<ID3D12StateObject>& arg_stateObject, LPCWSTR arg_hitGroup)
	{

		/*===== シェーダーレコードを書き込む =====*/

		for (auto& index : m_refBlas) {
			arg_dest = index.lock()->WriteShaderRecord(arg_dest, arg_recordSize, arg_stateObject, arg_hitGroup);
		}

		return arg_dest;

	}

}
