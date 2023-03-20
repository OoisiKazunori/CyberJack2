#include "SecondStage.h"
#include"../KazLibrary/Imgui/MyImgui.h"

SecondStage::SecondStage()
{
	stageParamLoader.LoadFile(KazFilePathName::StageParamPath + "StageParamData_SecondStage.json");

	for (int i = 0; i < line.size(); ++i)
	{
		line[i].data.colorData = { 255,255,255,255 };
		line[i].data.pipelineName = PIPELINE_NAME_FOG_LINE;

		fogHandle[i].handle = line[i].CreateConstBuffer(sizeof(FogData), typeid(FogData).name(), GRAPHICS_RANGE_TYPE_CBV_VIEW, GRAPHICS_PRAMTYPE_DATA);
		fogHandle[i].fogColor.fogdata = { 1.0f,1.0f,1.0f,0.0006f };

		FogData tmpColor = fogHandle[i].fogColor;
		line[i].TransData(&tmpColor, fogHandle[i].handle, typeid(tmpColor).name());
	}

	if (false)
	{
		std::array <std::array<char, 7>, 50>data;
		for (int i = 0; i < line.size(); ++i)
		{
			data[i][0] = 'L';
			data[i][1] = 'i';
			data[i][2] = 'n';
			data[i][3] = 'e';
			if (i < 10)
			{
				data[i][4] = std::to_string(i)[0];
				data[i][5] = '\0';
			}
			else
			{
				data[i][4] = std::to_string(i)[0];
				data[i][5] = std::to_string(i)[1];
				data[i][6] = '\0';
			}
		}

		for (int i = 0; i < line.size(); ++i)
		{
			DirectX::XMVECTOR startPos = line[i].data.startPos.ConvertXMVECTOR();
			DirectX::XMVECTOR endPos = line[i].data.endPos.ConvertXMVECTOR();

			//Line毎のメンバ変数を追加
			rapidjson::Value startPosArray(rapidjson::kArrayType);
			rapidjson::Value endPosArray(rapidjson::kArrayType);
			for (int axisIndex = 0; axisIndex < 3; ++axisIndex)
			{
				startPosArray.PushBack(rapidjson::Value(startPos.m128_f32[axisIndex]), stageParamLoader.doc.GetAllocator());
				endPosArray.PushBack(rapidjson::Value(endPos.m128_f32[axisIndex]), stageParamLoader.doc.GetAllocator());
			}

			//Lineオブジェクトにデータを追加
			rapidjson::Value object(rapidjson::kObjectType);
			object.AddMember("StartPos", startPosArray, stageParamLoader.doc.GetAllocator());
			object.AddMember("EndPos", endPosArray, stageParamLoader.doc.GetAllocator());
			stageParamLoader.doc.AddMember(rapidjson::GenericStringRef<char>(data[i].data()), object, stageParamLoader.doc.GetAllocator());
		}
		stageParamLoader.ExportFile(KazFilePathName::StageParamPath + "StageParamData_SecondStage.json");
	}

	for (int i = 0; i < JSON_ARRAY_NUM; ++i)
	{
		std::string name = "Line" + std::to_string(i);
		for (int axisIndex = 0; axisIndex < 3; ++axisIndex)
		{
			int x = 0;
			int y = 1;
			int z = 2;
			line[i].data.startPos.x = stageParamLoader.doc[name.c_str()]["StartPos"][x].GetFloat();
			line[i].data.endPos.x = stageParamLoader.doc[name.c_str()]["EndPos"][x].GetFloat();

			line[i].data.startPos.y = stageParamLoader.doc[name.c_str()]["StartPos"][y].GetFloat();
			line[i].data.endPos.y = stageParamLoader.doc[name.c_str()]["EndPos"][y].GetFloat();

			line[i].data.startPos.z = stageParamLoader.doc[name.c_str()]["StartPos"][z].GetFloat();
			line[i].data.endPos.z = stageParamLoader.doc[name.c_str()]["EndPos"][z].GetFloat();
		}
	}


	int index = 0;
	int zIndex = 0;
	for (int i = 25; i < line.size(); i++)
	{
		float z = static_cast<float>(-100 + zIndex * 50);
		switch (index)
		{
		case 0:
			line[i].data.startPos = { -150.0f,-100.0f,z };
			line[i].data.endPos = { 150.0f,-100.0f,z };
			++index;
			break;

		case 1:
			line[i].data.startPos = { -150.0f,100.0f,z };
			line[i].data.endPos = { 150.0f,100.0f,z };
			++index;
			break;

		case 2:
			line[i].data.startPos = { -150.0f,100.0f,z };
			line[i].data.endPos = { -150.0f,-100.0f,z };
			++index;
			break;

		case 3:
			line[i].data.startPos = { 150.0f,100.0f,z };
			line[i].data.endPos = { 150.0f,-100.0f,z };
			++zIndex;
			index = 0;
			break;
		default:
			break;
		}
	}
}

void SecondStage::Update()
{
#pragma region ImGui
	bool exportFlag = false;
	bool importFlag = false;

	/*ImGui::Begin("Stage");
	for (int i = 0; i < JSON_ARRAY_NUM; ++i)
	{
		string name = "Line" + std::to_string(i);
		if (ImGui::TreeNode(name.c_str()))
		{
			ImGui::InputFloat("StartPosX", &line[i].data.startPos.x);
			ImGui::InputFloat("StartPosY", &line[i].data.startPos.y);
			ImGui::InputFloat("StartPosZ", &line[i].data.startPos.z);
			ImGui::InputFloat("EndPosX", &line[i].data.endPos.x);
			ImGui::InputFloat("EndPosY", &line[i].data.endPos.y);
			ImGui::InputFloat("EndPosZ", &line[i].data.endPos.z);
			ImGui::TreePop();
		}
	}

	if (ImGui::Button("Import"))
	{
		importFlag = true;
	}
	ImGui::SameLine();
	if (ImGui::Button("Export"))
	{
		exportFlag = true;
	}
	ImGui::End();*/

	//ファイル読み込み
	if (importFlag)
	{
		for (int i = 0; i < JSON_ARRAY_NUM; ++i)
		{
			std::string name = "Line" + std::to_string(i);
			
			int x = 0;
			int y = 0;
			int z = 0;
			line[i].data.startPos.x = stageParamLoader.doc[name.c_str()]["StartPos"][x].GetFloat();
			line[i].data.endPos.x = stageParamLoader.doc[name.c_str()]["EndPos"][x].GetFloat();

			line[i].data.startPos.y = stageParamLoader.doc[name.c_str()]["StartPos"][y].GetFloat();
			line[i].data.endPos.y = stageParamLoader.doc[name.c_str()]["EndPos"][y].GetFloat();

			line[i].data.startPos.z = stageParamLoader.doc[name.c_str()]["StartPos"][z].GetFloat();
			line[i].data.endPos.z = stageParamLoader.doc[name.c_str()]["EndPos"][z].GetFloat();
		}
	}

	//ファイル書き込み
	if (exportFlag)
	{
		for (int i = 0; i < JSON_ARRAY_NUM; ++i)
		{
			std::string name = "Line" + std::to_string(i);
			for (int axisIndex = 0; axisIndex < 3; ++axisIndex)
			{
				stageParamLoader.doc[name.c_str()]["StartPos"][axisIndex].SetFloat(line[i].data.startPos.ConvertXMVECTOR().m128_f32[axisIndex]);
				stageParamLoader.doc[name.c_str()]["EndPos"][axisIndex].SetFloat(line[i].data.endPos.ConvertXMVECTOR().m128_f32[axisIndex]);
			}
		}
		stageParamLoader.ExportFile(KazFilePathName::StageParamPath + "StageParamData_SecondStage.json");
	}


#pragma endregion
	for (int i = 0; i < 4; i++)
	{
		line[i].data.startPos.z += -objectSpeed;
		line[i].data.endPos.z += -objectSpeed;
		if (line[i].data.startPos.z <= -150.0f)
		{
			line[i].data.startPos.z = 1400.0f;
			line[i].data.endPos.z = 1400.0f;
		}
	}

	for (int i = 25; i < line.size(); i++)
	{
		line[i].data.startPos.z += -objectSpeed;
		line[i].data.endPos.z += -objectSpeed;
		if (line[i].data.startPos.z <= -150.0f)
		{
			line[i].data.startPos.z = 1400.0f;
			line[i].data.endPos.z = 1400.0f;
		}
	}
}

void SecondStage::Draw()
{
	for (int i = 0; i < line.size(); i++)
	{
		line[i].data.cameraIndex = cameraIndex;
	}
	for (int i = 0; i < line.size(); ++i)
	{
		line[i].Draw();
	}
}