#include "ModelTool.h"
#include <iostream>
#include <filesystem>
#include <fstream>
#include"../KazLibrary/Imgui/MyImgui.h"

ModelTool::ModelTool(std::string arg_fileDir) :m_fileDir(arg_fileDir)
{
	//Load();
}

void ModelTool::Load()
{
	//指定のファイル階層に存在するファイル一覧検索
	std::vector<std::string> fileNameArray;
	std::vector<std::string> fileDirArray;
	LoadFileName(m_fileDir, fileNameArray, fileDirArray);

	//モデルの読み込み
	m_modelInfomationArray.clear();
	for (int i = 0; i < fileNameArray.size(); ++i)
	{
		m_modelInfomationArray.emplace_back(
			ModelData(fileNameArray[i], ModelLoader::Instance()->Load(m_fileDir + fileDirArray[i], fileNameArray[i]))
		);
	}
}

void ModelTool::LoadFileName(std::string arg_folderPath, std::vector<std::string>& arg_file_names, std::vector<std::string>& fileDir)
{
	//ファイル検索の参考資料
	//https://qiita.com/tes2840/items/8d295b1caaf10eaf33ad

	using namespace std::filesystem;
	directory_iterator iter(arg_folderPath), end;
	std::error_code err;

	for (; iter != end && !err; iter.increment(err))
	{
		const directory_entry entry = *iter;
		//フォルダだった場合は1階層下を潜る
		if (entry.is_directory())
		{
			fileDir.emplace_back(entry.path().filename().string() + "/");
			LoadFileName(entry.path().string(), arg_file_names, fileDir);
		}
		//ファイルならフォーマットを確認する
		else
		{
			//拡張子取得の参考資料
			//https://qiita.com/takano_tak/items/acf34b4a30cb974bab65
			std::string filename(entry.path().filename().string());
			int extI = static_cast<int>(filename.find_last_of("."));
			std::string extName = filename.substr(extI, filename.size() - extI);

			//拡張子がgltfもしくはglbの場合は読み込み対象なので保存する
			if (extName == ".gltf" || extName == ".glb")
			{
				//ファイル名取得
				arg_file_names.emplace_back(entry.path().filename().string());
				break;
			}
		}
	}

	/* エラー処理 */
	if (err)
	{
		std::cout << err.value() << std::endl;
		std::cout << err.message() << std::endl;
		assert(0);
	}
}

void ModelTool::Update()
{
	//モデルのロード
	if (KeyBoradInputManager::Instance()->InputTrigger(DIK_L))
	{
		Load();
	}
}

void ModelTool::Draw(DrawingByRasterize& render)
{
	ImGui::Begin("ModelTool");
	ImGui::BeginChild(ImGui::GetID((void*)0), ImVec2(250, 100), ImGuiWindowFlags_NoTitleBar);
	//モデル名表示
	for (int i = 0; i < m_modelInfomationArray.size(); ++i)
	{
		ImGui::RadioButton(m_modelInfomationArray[i].m_fileName.c_str(), &m_selectNum, i);
	}
	ImGui::EndChild();
	if (m_modelInfomationArray.size() != 0)
	{
		if(ImGui::TreeNode("Transform"))
		{
			//Transform初期化
			if (ImGui::Button("InitTransform"))
			{
				m_modelInfomationArray[m_selectNum].m_transform.pos = {};
				m_modelInfomationArray[m_selectNum].m_transform.scale = {};
				m_modelInfomationArray[m_selectNum].m_transform.rotation = {};
			}
			//回転処理
			KazImGuiHelper::InputTransform3D("Transform", &m_modelInfomationArray[m_selectNum].m_transform);
			ImGui::TreePop();
		}
	}
	else
	{
		ImGui::Text("You didn't Load Anything. Please Press L or check inside a file");
	}
	ImGui::End();


	//render.ObjectRender();
}
