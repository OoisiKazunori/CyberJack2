#pragma once
#include"../KazLibrary/Render/DrawFunc.h"
#include<filesystem>
#include"../KazLibrary/Loader/ModelLoader.h"
#include"../KazLibrary/Input/KeyBoradInputManager.h"
#include"../KazLibrary/Render/DrawingByRasterize.h"
#include"../KazLibrary/Helper/Select.h"

class ModelTool
{
public:
	ModelTool(std::string arg_fileDir);

	void Load();
	void LoadFileName(std::string arg_folderPath, std::vector<std::string>& arg_file_names, std::vector<std::string>& fileDir);
	void Update();
	void Draw(DrawingByRasterize& render);

private:
	struct ModelData
	{
		ModelData(std::string arg_fileName, std::shared_ptr<ModelInfomation>arg_modelInfomation) :m_fileName(arg_fileName), m_modelInfomation(arg_modelInfomation)
		{};
		std::string m_fileName;
		KazMath::Transform3D m_transform;
		std::shared_ptr<ModelInfomation>m_modelInfomation;
	};
	//ファイル検索をもとに取得したモデル情報
	std::vector<ModelData>m_modelInfomationArray;
	//確認用のモデルが入っているファイルのパス
	std::string m_fileDir;

	int m_selectNum;
};

