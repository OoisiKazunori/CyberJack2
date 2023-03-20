#include "WirteCpuLineData.h"
#include"../KazLibrary/Helper/KazHelper.h"

WirteCpuLineData::WirteCpuLineData()
{

	std::array<std::vector<char>, DATA_MAX>lObjName;
	for (int i = 0; i < DATA_MAX; ++i)
	{
		lObjName[i] = KazHelper::CovertStringToChar("Line" + std::to_string(i));
	}

	stageParamLoader.LoadFile(KazFilePathName::StageParamPath + "CpuLineData.json");
	if (false)
	{
		for (int i = 0; i < DATA_MAX; ++i)
		{
			rapidjson::Value object(rapidjson::kObjectType);

			//Box毎のメンバ変数を追加
			std::vector<rapidjson::Value> startPosArray;
			std::vector<rapidjson::Value> endPosArray;

			for (int member = 0; member < LINE_MAX_NUM; ++member)
			{
				startPosArray.push_back(rapidjson::Value(rapidjson::kArrayType));
				endPosArray.push_back(rapidjson::Value(rapidjson::kArrayType));
				for (int axisIndex = 0; axisIndex < 3; ++axisIndex)
				{
					startPosArray[member].PushBack(rapidjson::Value(0.0f), stageParamLoader.doc.GetAllocator());
					endPosArray[member].PushBack(rapidjson::Value(0.0f), stageParamLoader.doc.GetAllocator());
				}
			}

			//Boxオブジェクトにデータを追加
			object.AddMember("StartPos1", startPosArray[0], stageParamLoader.doc.GetAllocator());
			object.AddMember("EndPos1", endPosArray[0], stageParamLoader.doc.GetAllocator());
			object.AddMember("StartPos2", startPosArray[1], stageParamLoader.doc.GetAllocator());
			object.AddMember("EndPos2", endPosArray[1], stageParamLoader.doc.GetAllocator());
			object.AddMember("StartPos3", startPosArray[2], stageParamLoader.doc.GetAllocator());
			object.AddMember("EndPos3", endPosArray[2], stageParamLoader.doc.GetAllocator());

			stageParamLoader.doc.AddMember(rapidjson::GenericStringRef<char>(lObjName[i].data()), object, stageParamLoader.doc.GetAllocator());
		}
		stageParamLoader.ExportFile(KazFilePathName::StageParamPath + "CpuLineData.json");
	}

	importFlag = false;
}

void WirteCpuLineData::Write()
{
	for (int dataIndex = 0; dataIndex < DATA_MAX; ++dataIndex)
	{
		const std::string name = "Line" + std::to_string(dataIndex);
		for (int i = 0; i < LINE_MAX_NUM; ++i)
		{
			const std::string startPosString = "StartPos" + std::to_string(i + 1);
			const std::string endPosString = "EndPos" + std::to_string(i + 1);
			for (int axisIndex = 0; axisIndex < 3; ++axisIndex)
			{
				switch (axisIndex)
				{
				case 0:
					stageParamLoader.doc[name.c_str()][startPosString.c_str()][axisIndex].SetFloat(data[dataIndex].startPosData[i]->x);
					stageParamLoader.doc[name.c_str()][endPosString.c_str()][axisIndex].SetFloat(data[dataIndex].endPosData[i]->x);
					break;
				case 1:
					stageParamLoader.doc[name.c_str()][startPosString.c_str()][axisIndex].SetFloat(data[dataIndex].startPosData[i]->y);
					stageParamLoader.doc[name.c_str()][endPosString.c_str()][axisIndex].SetFloat(data[dataIndex].endPosData[i]->y);
					break;
				case 2:
					stageParamLoader.doc[name.c_str()][startPosString.c_str()][axisIndex].SetFloat(data[dataIndex].startPosData[i]->z);
					stageParamLoader.doc[name.c_str()][endPosString.c_str()][axisIndex].SetFloat(data[dataIndex].endPosData[i]->z);
					break;
				default:
					break;
				}
			}
		}
	}
	stageParamLoader.ExportFile(KazFilePathName::StageParamPath + "CpuLineData.json");
}

void WirteCpuLineData::Load()
{
	for (int dataIndex = 0; dataIndex < DATA_MAX; ++dataIndex)
	{
		std::string name = "Line" + std::to_string(dataIndex);

		for (int lineNum = 0; lineNum < LINE_MAX_NUM; ++lineNum)
		{
			std::string startPosString = "StartPos" + std::to_string(lineNum + 1);
			std::string endPosString = "EndPos" + std::to_string(lineNum + 1);
			for (int axisIndex = 0; axisIndex < 3; ++axisIndex)
			{
				switch (axisIndex)
				{
				case 0:
					data[dataIndex].startPosData[lineNum]->x = stageParamLoader.doc[name.c_str()][startPosString.c_str()][axisIndex].GetFloat();
					data[dataIndex].endPosData[lineNum]->x = stageParamLoader.doc[name.c_str()][endPosString.c_str()][axisIndex].GetFloat();
					break;
				case 1:
					data[dataIndex].startPosData[lineNum]->y = stageParamLoader.doc[name.c_str()][startPosString.c_str()][axisIndex].GetFloat();
					data[dataIndex].endPosData[lineNum]->y = stageParamLoader.doc[name.c_str()][endPosString.c_str()][axisIndex].GetFloat();
					break;
				case 2:
					data[dataIndex].startPosData[lineNum]->z = stageParamLoader.doc[name.c_str()][startPosString.c_str()][axisIndex].GetFloat();
					data[dataIndex].endPosData[lineNum]->z = stageParamLoader.doc[name.c_str()][endPosString.c_str()][axisIndex].GetFloat();
					break;
				default:
					break;
				}
			}
		}
	}
}

void WirteCpuLineData::UpdataData(int ID, int LINE_NUM, KazMath::Vec3<float> *START_POS, KazMath::Vec3<float> *END_POS)
{
	data[ID].startPosData[LINE_NUM] = START_POS;
	data[ID].endPosData[LINE_NUM] = END_POS;
}

