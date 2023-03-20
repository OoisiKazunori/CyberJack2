#pragma once
#include"../DirectXCommon/Base.h"

/// <summary>
/// フォルダまでのパスを纏めました
/// リソースの読み込みでパスを書く場合は、読み込みたいファイルがあるフォルダを候補から選んで先頭に書いた後、ファイル名を書いてください
/// </summary>
namespace KazFilePathName
{
	static std::string RelativeResourcePath = "Resource/";

	//シェーダーパス-----------------------

	static std::string RelativeShaderPath = RelativeResourcePath + "ShaderFiles/";
	static std::string VertexShaderPath = RelativeShaderPath + "VertexShader/";
	static std::string PixelShaderPath = RelativeShaderPath + "PixelShader/";
	static std::string GeometryShaderPath = RelativeShaderPath + "GeometryShader/";
	static std::string ComputeShaderPath = RelativeShaderPath + "ComputeShader/";


	//画像パス-----------------------

	static std::string TestPath = RelativeResourcePath + "Test/";
	static std::string CursorPath = RelativeResourcePath + "Cursor/";
	static std::string HpPath = RelativeResourcePath + "Hp/";
	static std::string GoalPath = RelativeResourcePath + "Goal/";
	static std::string GameOverPath = RelativeResourcePath + "GameOver/";
	static std::string StageUIPath = RelativeResourcePath + "StageUI/";
	static std::string UIPath = RelativeResourcePath + "UI/";
	static std::string TitlePath = RelativeResourcePath + "Title/";
	static std::string SceneChangePath = RelativeResourcePath + "SceneChange/";
	static std::string TutorialPath = RelativeResourcePath + "Tutorial/";
	static std::string EnemyPath = RelativeResourcePath + "Enemy/";
	static std::string EffectPath = RelativeResourcePath + "Effect/";
	static std::string EffectFirePath = EffectPath + "Fire/";
	static std::string LinePath = RelativeResourcePath + "Line/";
	static std::string SoundPath = RelativeResourcePath + "Sound/";
	static std::string StagePath = RelativeResourcePath + "Stage/";
	static std::string PlayerPath = RelativeResourcePath + "Player/";


	//デバックパス-----------------------

	static std::string ParamPath = RelativeResourcePath + "Parameter/";
	static std::string StageParamPath = ParamPath + "Stage/";

}