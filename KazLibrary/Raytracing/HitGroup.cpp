#include "HitGroup.h"
#include "RayRootSignature.h"

namespace Raytracing {

	HitGroup::HitGroup()
	{
	}

	void HitGroup::Generate(const HitGroupInitData& arg_inputData, int arg_registerSpace, const LPCWSTR& arg_hitGroupName)
	{


		/*===== 生成処理 =====*/

		//入力されたデータの値を保存する。
		m_CH = arg_inputData.m_CH;
		m_AH = arg_inputData.m_AH;
		m_IS = arg_inputData.m_IS;
		m_SRVcount = arg_inputData.m_SRVcount;
		m_CBVcount = arg_inputData.m_CBVcount;
		m_UAVcount = arg_inputData.m_UAVcount;
		m_registerSpace = arg_registerSpace;
		m_hitGroupName = arg_hitGroupName;

		//ローカルルートシグネチャを生成する。
		m_localRootSig = std::make_shared<RayRootsignature>();

		//SRVについて追加。
		for (int index = 0; index < m_SRVcount; ++index) {

			m_localRootSig->AddRootparam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, index, m_registerSpace);

		}
		//UAVについて追加。
		for (int index = 0; index < m_UAVcount; ++index) {

			m_localRootSig->AddRootparam(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, index, m_registerSpace);

		}
		//CBVについて追加。
		for (int index = 0; index < m_CBVcount; ++index) {

			m_localRootSig->AddRootparam(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, index, m_registerSpace);

		}
		//サンプラーを追加。
		m_localRootSig->AddStaticSampler(m_registerSpace);

		//ローカルルートシグネチャを生成。
		m_localRootSig->Build(true, m_hitGroupName);


	}

}
