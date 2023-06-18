#include "HitGroupMgr.h"
#include "HitGroup.h"
#include <assert.h>

namespace Raytracing {

	void HitGroupMgr::Setting()
	{

		/*===== ヒットグループ設定 =====*/

		//ヒットグループ名のコンテナを設定。
		hitGroupNames.emplace_back(L"DEFAULT");

		//ヒットグループを生成。
		HitGroupInitData initData;

		//第一要素
		initData.m_CH = { L"mainCHS",true };
		initData.m_AH = { L"mainAnyHit",true };
		initData.m_IS = { L"",false };
		initData.m_SRVcount = 3;
		initData.m_UAVcount = 0;
		initData.m_CBVcount = 0;
		m_hitGroup.push_back(std::make_shared<HitGroup>());
		m_hitGroup[DEF]->Generate(initData, 1, hitGroupNames[DEF]);

	}

	const LPCWSTR& HitGroupMgr::GetCH(HITGROUP_ID arg_index)
	{
		return m_hitGroup[arg_index]->GetCH();
	}

	bool HitGroupMgr::GetCHFlag(HITGROUP_ID arg_index)
	{
		return m_hitGroup[arg_index]->GetCHFlag();
	}

	const LPCWSTR& HitGroupMgr::GetAH(HITGROUP_ID arg_index)
	{
		return m_hitGroup[arg_index]->GetAH();
	}

	bool HitGroupMgr::GetAHFlag(HITGROUP_ID arg_index)
	{
		return m_hitGroup[arg_index]->GetAHFlag();
	}

	const LPCWSTR& HitGroupMgr::GetIS(HITGROUP_ID arg_index)
	{
		return m_hitGroup[arg_index]->GetIS();
	}

	bool HitGroupMgr::GetISFlag(HITGROUP_ID arg_index)
	{
		return m_hitGroup[arg_index]->GetISFlag();
	}

	int HitGroupMgr::GetSRVCount(HITGROUP_ID arg_index)
	{
		return m_hitGroup[arg_index]->GetSRVCount();
	}

	int HitGroupMgr::GetCBVCount(HITGROUP_ID arg_index)
	{
		return m_hitGroup[arg_index]->GetCBVCount();
	}

	int HitGroupMgr::GetUAVCount(HITGROUP_ID arg_index)
	{
		return m_hitGroup[arg_index]->GetUAVCount();
	}

	const std::shared_ptr<RayRootsignature> HitGroupMgr::GetLocalRootSig(HITGROUP_ID arg_index)
	{
		return m_hitGroup[arg_index]->GetLoacalRootSig();
	}

	int HitGroupMgr::GetHitGroupID(const LPCWSTR& arg_hitGroupName)
	{

		/*===== ヒットグループ名からヒットグループIDを取得する。 =====*/

		int hitGroupID = -1;

		for (int index = 0; index < MAX_HIT_GROUP; ++index) {

			//同じ名前だったら。
			if (arg_hitGroupName != hitGroupNames[index]) continue;

			//IDを保存。
			hitGroupID = index;

		}

		//-1だったら名前の存在しないヒットグループをセットしようとしているのでアサートを出す。
		if (hitGroupID == -1) {

			//存在しないヒットグループをセットしようとしている。
			assert(0);

		}

		return hitGroupID;

	}

	int HitGroupMgr::GetHitGroupSRVCount(HITGROUP_ID arg_hitGroupID)
	{

		/*===== ヒットグループのSRVの数を取得 =====*/

		return m_hitGroup[arg_hitGroupID]->GetSRVCount();

	}

	int HitGroupMgr::GetHitGroupUAVCount(HITGROUP_ID arg_hitGroupID)
	{

		/*===== ヒットグループのUAVの数を取得 =====*/

		return m_hitGroup[arg_hitGroupID]->GetUAVCount();

	}

}