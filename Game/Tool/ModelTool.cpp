#include "ModelTool.h"
#include <iostream>
#include <filesystem>
#include <fstream>
#include"../KazLibrary/Imgui/MyImgui.h"

ModelTool::ModelTool(std::string arg_fileDir) :m_fileDir(arg_fileDir)
{
	Load();

	for (int z = 0; z < m_gridCallDataX.size(); ++z)
	{
		m_gridCallDataX[z] = DrawFuncData::SetLine();
	}
	for (int y = 0; y < m_gridCallDataZ.size(); ++y)
	{
		m_gridCallDataZ[y] = DrawFuncData::SetLine();
	}
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
	if (ImGui::Button("Load Model"))
	{
		Load();
	}
	ImGui::BeginChild(ImGui::GetID((void*)0), ImVec2(250, 100), ImGuiWindowFlags_NoTitleBar);
	//モデル名表示
	for (int i = 0; i < m_modelInfomationArray.size(); ++i)
	{
		ImGui::RadioButton(m_modelInfomationArray[i].m_fileName.c_str(), &m_selectNum, i);
	}
	ImGui::EndChild();
	if (m_modelInfomationArray.size() != 0)
	{
		if (ImGui::TreeNode("Transform"))
		{
			//Transform初期化
			if (ImGui::Button("InitTransform"))
			{
				m_modelInfomationArray[m_selectNum].m_transform.pos = {};
				m_modelInfomationArray[m_selectNum].m_transform.scale = { 1.0f,1.0f,1.0f };
				m_modelInfomationArray[m_selectNum].m_transform.rotation = {};
			}
			//回転処理
			ImGui::DragFloat("Scale", &m_modelInfomationArray[m_selectNum].m_transform.scale.x);
			KazImGuiHelper::InputVec3("Rotation", &m_modelInfomationArray[m_selectNum].m_transform.rotation);
			m_modelInfomationArray[m_selectNum].m_transform.scale.y = m_modelInfomationArray[m_selectNum].m_transform.scale.x;
			m_modelInfomationArray[m_selectNum].m_transform.scale.z = m_modelInfomationArray[m_selectNum].m_transform.scale.x;
			ImGui::TreePop();
		}
	}
	else
	{
		ImGui::Text("You didn't Load Anything. Please press Load Model button or check inside a file");
	}
	KazImGuiHelper::InputVec3("DirectionalLight", &m_directionalLight);
	ImGui::End();

	//モデル描画
	DrawFunc::DrawModelLight(m_modelInfomationArray[m_selectNum].m_drawCall, m_modelInfomationArray[m_selectNum].m_transform, m_directionalLight, KazMath::Color(255, 255, 255, 255));
	DrawGrid(render);
	render.ObjectRender(m_modelInfomationArray[m_selectNum].m_drawCall);
}

void ModelTool::DrawGrid(DrawingByRasterize& render)
{
	const float height = -5.0f;
	const float range = 50.0f;
	const KazMath::Color lineColor(49, 187, 134, 255);

	//横の線を並べる
	for (int z = 0; z < m_gridCallDataX.size(); ++z)
	{
		float zLine = static_cast<float>(z) * 10.0f - (range);
		KazMath::Vec3<float>startPos(-range + 20.0f, height, zLine), endPos(range + 20.0f, height, zLine);
		DrawFunc::DrawLine(m_gridCallDataX[z], startPos, endPos, lineColor);
		render.ObjectRender(m_gridCallDataX[z]);
	}
	//縦の線を並べる
	for (int x = 0; x < m_gridCallDataZ.size(); ++x)
	{
		float xLine = static_cast<float>(x) * 10.0f - (range);
		KazMath::Vec3<float>startPos(xLine, height, -range), endPos(xLine, height, range + 20.0f);
		DrawFunc::DrawLine(m_gridCallDataZ[x], startPos, endPos, lineColor);
		render.ObjectRender(m_gridCallDataZ[x]);
	}
}
