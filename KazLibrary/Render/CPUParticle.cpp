#include "CPUParticle.h"
#include "CPUParticle.h"

CPUParticle::CPUParticle()
{
	//パーティクルのシステム全体の初期化処理

	//パーティクルのクリア処理
	for (int i = 0; i < EMITTER_MAX_NUM; ++i)
	{
		m_emitterTableArray[i] = INVAILD_NUM;
	}
	for (int i = 0; i < m_particleTransformDataArray.size(); ++i)
	{
		m_particleTransformDataArray[i].m_isEnableFlag = false;
	}
	for (int i = 0; i < m_particleHeaderArray.size(); ++i)
	{
		m_particleHeaderArray[i].Release();
	}

}

void CPUParticle::GenerateEffect(int arg_particleNum)
{
	const int emitterID = m_emitterHandle.GetHandle();

	//エミッター生成処理---------------------------------------
	//パーティクルの範囲を決める
	//m_emitterHeaderArray[emitterID].m_particleBinarySize = { 0,arg_particleNum };
	//使用するパーティクルのインデックスをもらう(被り防止処理含み)
	int count = 0;
	for (int i = 0; i < m_particleHeaderArray.size(); ++i)
	{
		if (m_particleHeaderArray[i].m_emitterID != INVAILD_NUM)
		{
			continue;
		}
		m_particleHeaderArray[i].m_emitterID = emitterID;
		if (1 <= KazMath::Rand<int>(2, 0))
		{
			m_particleHeaderArray[i].m_isAliveFlag = true;
		}
		else
		{
			m_particleHeaderArray[i].m_isAliveFlag = false;
		}
		m_particleHeaderArray[i].m_depth = KazMath::Rand<float>(1.0f, 0.0f);

		//未使用のパーティクルから消費する
		for (int particleIndex = 0; particleIndex < m_particleTransformDataArray.size(); ++particleIndex)
		{
			if (m_particleTransformDataArray[particleIndex].m_isEnableFlag)
			{
				continue;
			}
			m_particleTransformDataArray[particleIndex].m_isEnableFlag = true;
			m_particleHeaderArray[i].m_particleIndex = particleIndex;
			break;
		}
		++count;

		if (arg_particleNum <= count)
		{
			break;
		}
	}

	bool debug = false;
	//エミッター生成処理---------------------------------------
}

void CPUParticle::ReleaseEffect(int arg_emitterID)
{
	//m_emitterHeaderArray[arg_emitterID].m_particleBinarySize = {};
	//エミッターのパーティクル範囲から使用しているパーティクルインデックスを解放
	//DataSize size = m_emitterHeaderArray[arg_emitterID].m_particleBinarySize;
	for (int i = 0; i < m_particleHeaderArray.size(); ++i)
	{
		if (m_particleHeaderArray[i].m_emitterID != arg_emitterID)
		{
			continue;
		}
		int particleIndex = m_particleHeaderArray[i].m_particleIndex;
		m_particleTransformDataArray[particleIndex].m_isEnableFlag = false;
		m_particleHeaderArray[i].Release();
	}

	m_emitterHandle.DeleteHandle(arg_emitterID);
}

void CPUParticle::UpdateEmitter()
{
}

void CPUParticle::UpdateEmitterAndParticleOnGPU()
{
	DataSize enableSize;
	enableSize.m_head = 0;
	enableSize.m_size = 0;
	//更新
	for (int particleIndex = 0; particleIndex < m_particleHeaderArray.size(); ++particleIndex)
	{
		if (!m_particleHeaderArray[particleIndex].m_isAliveFlag)
		{
			continue;
		}
		//パーティクルの更新処理
		int id = m_particleHeaderArray[particleIndex].m_particleIndex;
		m_particleTransformDataArray[id].m_transform.pos = { 100.0f,100.0f,100.0f };
		enableSize.m_size = particleIndex;
	}

	//生死ソートをかける
	QuickSortAliveFlag(m_particleHeaderArray, enableSize.m_head, enableSize.m_size);

	DataSize aliveRange, deadRange;
	aliveRange.m_head = 0;
	//生存
	for (int i = 0; i < m_particleHeaderArray.size(); ++i)
	{
		if (!m_particleHeaderArray[i].m_isAliveFlag)
		{
			aliveRange.m_size = i - 1;
			deadRange.m_head = i;
			break;
		}
	}
	//死亡
	for (int i = deadRange.m_head; i < m_particleHeaderArray.size(); ++i)
	{
		if (m_particleHeaderArray[i].m_emitterID == INVAILD_NUM)
		{
			deadRange.m_size = i - 1;
			break;
		}
	}

	//エミッターIDのソートをかける
	//生存でのソート
	QuickSortEmitterID(m_particleHeaderArray, aliveRange.m_head, aliveRange.m_size);
	//死亡でのソート
	QuickSortEmitterID(m_particleHeaderArray, deadRange.m_head, deadRange.m_size);


	std::vector<DataSize>emitterDataSize;
	int prevHandle = -1;
	for (int i = 0; i < m_particleHeaderArray.size(); ++i)
	{
		if (m_particleHeaderArray[i].m_emitterID == INVAILD_NUM)
		{
			break;
		}
		if (m_particleHeaderArray[i].m_emitterID != prevHandle)
		{
			emitterDataSize.emplace_back();
			emitterDataSize.back().m_head = i;
			prevHandle = m_particleHeaderArray[i].m_emitterID;
		}
		emitterDataSize.back().m_size = i;
	}

	//深度ソートをかける
	for (int i = 0; i < emitterDataSize.size(); ++i)
	{
		//エミッター単位でZソートをかける
		QuickSortDepth(m_particleHeaderArray, emitterDataSize[i].m_head, emitterDataSize[i].m_size);
	}

	bool debug = false;

	/*
	Begin Update
	前フレームと発生予定のパーティクル数を見て総数を計算する
	*/

	/*
	Fill Unused Index
	新規エミッターが追加されたら、Part新しく割り当てる領域に初期化をかける(INVAILD_NUMで埋める)
	領域は末尾から追加していく -> emplance_backすると言う事
	*/

	/*
	Spawn Particles
	新しく発生するパーティクル数の決定　→　ParticleHeaderに末尾から追加する。
	EmitterRangeの初期化
	(どうやってHeadの位置が分かるんだ)
	*/

	/*
	Initialize Particles

	Dispatch(エミッターの数,1,1)
	*/

	/*
	Update Particle
	1スレッドで1パーティクルの更新
	ParticleHeaderを参照して更新をかける事で生存しているパーティクルのみ処理できる
	UberShader(複数の機能を一つのシェーダーに纏める)的に全アイテムの実行

	沢山の関数を用意して実行している...Itemの種類も有限みたいだ。用意された関数分しか出来ないだろう。
	移動量の初期化も行っていたりするのか
	*/

	/*
	Bitonic sort
	パーティクルをAlive,EmitterID,Depthの順でソートをかける
	*/

	/*
	Range Particle
	エミッター単位で生きているパーティクルの範囲と死んでいるパーティクルの範囲を決める
	*/

	/*
	Terminate Particles
	死んだパーティクルのIndexの返却
	Particle Index Listに使われてる所に書き込む
	*/

	/*
	Build Emitter Draw Arg
	EmitterRangeからパーティクルの数を求めてEmitterDataを更新
	*/

	/*
	Build Primitive
	パーティクル情報から頂点を生成する。
	1スレッド1パーティクル
	*/
}
