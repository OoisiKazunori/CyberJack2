#include "RayRootSignature.h"
#include "DirectXCommon/DirectX12Device.h"

namespace Raytracing {

	void RayRootsignature::AddRootparam(D3D12_DESCRIPTOR_RANGE_TYPE arg_type, UINT arg_shaderRegister, UINT arg_registerSpace)
	{

		/*===== ルートパラメーター追加処理 =====*/


		if (arg_type == D3D12_DESCRIPTOR_RANGE_TYPE_SRV) {

			m_descRange[m_rootparamCount] = CD3DX12_DESCRIPTOR_RANGE(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, arg_shaderRegister, arg_registerSpace);
			m_rootparam[m_rootparamCount].InitAsDescriptorTable(1, &m_descRange[m_rootparamCount]);
			m_rootparam[m_rootparamCount].Descriptor.ShaderRegister = arg_shaderRegister;
			m_rootparam[m_rootparamCount].Descriptor.RegisterSpace = arg_registerSpace;
			m_rootparam[m_rootparamCount].DescriptorTable.NumDescriptorRanges = 1;

		}
		else if (arg_type == D3D12_DESCRIPTOR_RANGE_TYPE_CBV) {

			m_rootparam[m_rootparamCount].InitAsConstantBufferView(arg_shaderRegister, arg_registerSpace);
			m_rootparam[m_rootparamCount].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;

		}
		else if (arg_type == D3D12_DESCRIPTOR_RANGE_TYPE_UAV) {

			m_descRange[m_rootparamCount] = CD3DX12_DESCRIPTOR_RANGE(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, arg_shaderRegister, arg_registerSpace);
			m_rootparam[m_rootparamCount].InitAsDescriptorTable(1, &m_descRange[m_rootparamCount]);
			m_rootparam[m_rootparamCount].Descriptor.ShaderRegister = arg_shaderRegister;
			m_rootparam[m_rootparamCount].Descriptor.RegisterSpace = arg_registerSpace;
			m_rootparam[m_rootparamCount].DescriptorTable.NumDescriptorRanges = 1;

		}



		//ルートパラメーターの数を更新
		++m_rootparamCount;

	}

	void RayRootsignature::AddStaticSampler(int arg_registerSpace)
	{
		/*===== スタティックサンプラー追加処理 =====*/

		CD3DX12_STATIC_SAMPLER_DESC buff;
		buff.Init(0,
			D3D12_FILTER_MIN_MAG_MIP_LINEAR
		);
		buff.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		buff.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		buff.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		buff.RegisterSpace = arg_registerSpace;
		buff.ShaderRegister = 0;
		m_sampler[m_samplerCount] = buff;

		++m_samplerCount;

	}

	void RayRootsignature::Build(bool arg_isLocal, const wchar_t* arg_name)
	{
		/*===== ルートシグネチャ生成処理 =====*/

		//ルートシグネチャ設定用構造体を設定。
		D3D12_ROOT_SIGNATURE_DESC rootSigDesc{};
		rootSigDesc.NumParameters = m_rootparamCount;
		rootSigDesc.pParameters = m_rootparam.data();
		rootSigDesc.NumStaticSamplers = m_samplerCount;
		rootSigDesc.pStaticSamplers = m_sampler.data();

		//ローカルルートシグネチャのフラグが立っていたら、ローカルルートシグネチャのフラグを設定する。
		if (arg_isLocal) {

			rootSigDesc.Flags |= D3D12_ROOT_SIGNATURE_FLAG_LOCAL_ROOT_SIGNATURE;

		}

		//ルートシグネチャ生成。
		Microsoft::WRL::ComPtr<ID3DBlob> blob, errBlob;
		D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, &blob, &errBlob);
		if (errBlob.Get() != nullptr) {
			std::string a = static_cast<char*>(errBlob->GetBufferPointer());
			_RPTF0(_CRT_WARN, a.c_str());
		}
		DirectX12Device::Instance()->dev->CreateRootSignature(0, blob->GetBufferPointer(), blob->GetBufferSize(),
			IID_PPV_ARGS(&m_rootsignature));

		//名前を設定
		if (arg_name != nullptr) {
			m_rootsignature->SetName(arg_name);
		}

	}

}