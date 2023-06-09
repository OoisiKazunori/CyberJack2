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

	void BlasVector::Add(std::weak_ptr<Blas> arg_refBlas, const DirectX::XMMATRIX& arg_worldMat)
	{

		/*===== Tlasに登録するために配列に追加する =====*/

		//参照を追加して。
		m_refBlas.emplace_back(arg_refBlas);

		D3D12_RAYTRACING_INSTANCE_DESC instanceDesc;

		//行列を設定。
		XMStoreFloat3x4(
			reinterpret_cast<DirectX::XMFLOAT3X4*>(&instanceDesc.Transform),
			arg_worldMat);

		//インスタンスの詳細を設定。
		instanceDesc.InstanceID = 0;			//レイトレで行う処理のフラグをここで設定する。マテリアル側で設定してもよい。
		instanceDesc.InstanceMask = 0xFF;
		instanceDesc.InstanceContributionToHitGroupIndex = static_cast<int>(m_instanceDesc.size());
		instanceDesc.Flags = D3D12_RAYTRACING_INSTANCE_FLAG_NONE;
		instanceDesc.AccelerationStructure = arg_refBlas.lock()->GetBlasBuffer()->GetGPUVirtualAddress();

		//インスタンスを追加。
		m_instanceDesc.emplace_back(instanceDesc);

	}

	int BlasVector::GetBlasRefCount()
	{
		return static_cast<int>(m_refBlas.size());
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
