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
		std::vector<KazMath::Vec3<float>>posArray(2);
		VertexGenerateData bufferData(posArray.data(), sizeof(DirectX::XMFLOAT3), posArray.size(), sizeof(DirectX::XMFLOAT3));
		m_gridCallDataX[z] = DrawFuncData::SetLine(VertexBufferMgr::Instance()->GenerateBuffer(bufferData, false));
	}
	for (int y = 0; y < m_gridCallDataZ.size(); ++y)
	{
		std::vector<KazMath::Vec3<float>>posArray(2);
		VertexGenerateData bufferData(posArray.data(), sizeof(DirectX::XMFLOAT3), posArray.size(), sizeof(DirectX::XMFLOAT3));
		m_gridCallDataZ[y] = DrawFuncData::SetLine(VertexBufferMgr::Instance()->GenerateBuffer(bufferData, false));
	}


	struct OutputData
	{
		DirectX::XMMATRIX worldMat;
		DirectX::XMFLOAT4 color;
	};
	m_meshParticle = KazBufferHelper::SetGPUBufferData(sizeof(OutputData) * 10000);
	m_particle = KazBufferHelper::SetUploadBufferData(sizeof(OutputData) * 10000);
	m_meshParticle.structureSize = sizeof(OutputData);
	m_meshParticle.elementNum = 10000;
	m_particle.structureSize = sizeof(OutputData);
	m_particle.elementNum = 10000;

	m_particleRender =
		DrawFuncData::SetExecuteIndirect(
			DrawFuncData::GetBasicInstanceShader2(),
			m_particle.bufferWrapper->GetBuffer()->GetGPUVirtualAddress(),
			10000
		);
	m_particleRender.extraBufferArray.emplace_back();
	m_particleRender.extraBufferArray.back().rangeType = GRAPHICS_RANGE_TYPE_UAV_VIEW;
	m_particleRender.extraBufferArray.back().rootParamType = GRAPHICS_PRAMTYPE_DATA;
	m_particleRender.renderTargetHandle = -1;

	//m_meshParticle.GenerateCounterBuffer();
	//m_meshParticle.CreateUAVView();

	m_counterBuffer = KazBufferHelper::SetUploadBufferData(sizeof(UINT));
	UINT num = 0;
	m_counterBuffer.bufferWrapper->TransData(&num, sizeof(UINT));
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
		m_modelAnimation.emplace_back();
		m_modelAnimationInRaytracing.emplace_back();
		if (m_modelInfomationArray.back().m_modelInfomation->skelton->animations.size())
		{
			m_modelAnimation.back() = std::make_shared<ModelAnimator>(m_modelInfomationArray.back().m_modelInfomation);
			m_modelAnimation.back()->Play("繧｢繝ｼ繝槭メ繝･繧｢Action", true);

			m_modelAnimationInRaytracing.back() = std::make_shared<AnimationInRaytracing>();

			//生成
			m_modelAnimationInRaytracing.back()->m_vertexBuffer =
				KazBufferHelper::SetGPUBufferData(
					sizeof(VertexBufferAnimationData) * static_cast<int>(m_modelInfomationArray.back().m_modelInfomation->modelData[0].vertexData.verticesArray.size())
				);
			m_modelAnimationInRaytracing.back()->m_indexBuffer =
				KazBufferHelper::SetGPUBufferData(
					sizeof(UINT) * static_cast<int>(m_modelInfomationArray.back().m_modelInfomation->modelData[0].vertexData.indexArray.size())
				);
			m_modelAnimationInRaytracing.back()->m_indexBuffer.elementNum = static_cast<int>(m_modelInfomationArray.back().m_modelInfomation->modelData[0].vertexData.indexArray.size());
			m_modelAnimationInRaytracing.back()->m_indexBuffer.structureSize = sizeof(UINT);

			m_modelAnimationInRaytracing.back()->m_vertexBuffer.elementNum = static_cast<int>(m_modelInfomationArray.back().m_modelInfomation->modelData[0].vertexData.verticesArray.size());
			m_modelAnimationInRaytracing.back()->m_vertexBuffer.structureSize = sizeof(VertexBufferAnimationData);

			//既存の頂点情報のコピー
			m_modelAnimationInRaytracing.back()->m_vertexBuffer.bufferWrapper->CopyBuffer(
				VertexBufferMgr::Instance()->GetVertexIndexBuffer(m_modelInfomationArray.back().m_modelInfomation->modelVertDataHandle).vertBuffer[0]->bufferWrapper->GetBuffer()
			);
			m_modelAnimationInRaytracing.back()->m_indexBuffer.bufferWrapper->CopyBuffer(
				VertexBufferMgr::Instance()->GetVertexIndexBuffer(m_modelInfomationArray.back().m_modelInfomation->modelVertDataHandle).indexBuffer[0]->bufferWrapper->GetBuffer()
			);
		}
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
	//m_meshParticle.counterWrapper->CopyBuffer(m_counterBuffer.bufferWrapper->GetBuffer());

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
	if (m_modelAnimation[m_selectNum])
	{
		m_modelAnimation[m_selectNum]->Update(1.0f);
		m_modelAnimationInRaytracing[m_selectNum]->Compute(
			*VertexBufferMgr::Instance()->GetVertexIndexBuffer(m_modelInfomationArray[m_selectNum].m_modelInfomation->modelVertDataHandle).vertBuffer[0],
			m_modelAnimation[m_selectNum]->GetBoneMatBuff(),
			m_modelInfomationArray[m_selectNum].m_transform.GetMat()
		);
	}
	//KazImGuiHelper::InputVec3("DirectionalLight", &m_directionalLight);
	ImGui::End();

	//モデル描画
	if (m_modelAnimation[m_selectNum])
	{
		DrawFunc::DrawModel(m_modelInfomationArray[m_selectNum].m_drawCall, m_modelInfomationArray[m_selectNum].m_transform, m_modelAnimation[m_selectNum]->GetBoneMatBuff(), KazMath::Color(255, 255, 255, 255));
	}
	else
	{
		DrawFunc::DrawModelLight(m_modelInfomationArray[m_selectNum].m_drawCall, m_modelInfomationArray[m_selectNum].m_transform, m_directionalLight, KazMath::Color(255, 255, 255, 255));
	}
	DrawGrid(render);
	render.ObjectRender(m_modelInfomationArray[m_selectNum].m_drawCall);

	struct OutputData
	{
		DirectX::XMMATRIX worldMat;
		DirectX::XMFLOAT4 color;
	};

	std::vector<OutputData>vec(10000);
	memcpy(vec.data(), m_particle.bufferWrapper->GetMapAddres(), sizeof(OutputData) * 10000);
	//memcpy(vec.data(), m_meshParticle.bufferWrapper->GetMapAddres(), 10000);

	render.ObjectRender(m_particleRender);
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
		std::vector<KazMath::Vec3<float>>posArray;
		posArray.emplace_back(startPos);
		posArray.emplace_back(endPos);
		DrawFunc::DrawLine(m_gridCallDataZ[z], posArray, m_gridCallDataZ[z].m_modelVertDataHandle, lineColor);
		render.ObjectRender(m_gridCallDataZ[z]);
	}
	//縦の線を並べる
	for (int x = 0; x < m_gridCallDataZ.size(); ++x)
	{
		float xLine = static_cast<float>(x) * 10.0f - (range);
		KazMath::Vec3<float>startPos(xLine, height, -range), endPos(xLine, height, range + 20.0f);

		std::vector<KazMath::Vec3<float>>posArray;
		posArray.emplace_back(startPos);
		posArray.emplace_back(endPos);
		DrawFunc::DrawLine(m_gridCallDataX[x], posArray, m_gridCallDataX[x].m_modelVertDataHandle, lineColor);
		render.ObjectRender(m_gridCallDataX[x]);
	}
}
