#pragma once
#include <Helper/Compute.h>

namespace PostEffect {

	//実行することでGBufferからシーン情報と明るさ情報を持ってきてレンズフレアをかけてくれるクラス。
	class LensFlare {

	private:

		/*===== 変数 =====*/



	public:

		/*===== 関数 =====*/

		//コンストラクタ
		LensFlare();

		//初期化処理
		void Setting();

	};

}