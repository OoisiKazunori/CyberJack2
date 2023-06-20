#include "CPUParticle.h"
#include "CPUParticle.h"

CPUParticle::CPUParticle()
{
	//パーティクルのシステム全体の初期化処理

	//パーティクルのクリア処理
	for (int i = 0; i < EMITTER_MAX_NUM; ++i)
	{
		m_emitterTableArray[i] = -1;
	}

}

void CPUParticle::GenerateEffect()
{
	//エミッター生成

}

void CPUParticle::UpdateEmitter()
{

}

void CPUParticle::UpdateEmitterAndParticleOnGPU()
{
	/*
	Begin Update
	前フレームと発生予定のパーティクル数を見て総数を計算する
	*/

	/*
	Fill Unused Index
	新規エミッターが追加されたら、Part新しく割り当てる領域に初期化をかける(-1で埋める)
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
