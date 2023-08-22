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
}

void ModelTool::Load()
{
	//�w��̃t�@�C���K�w�ɑ��݂���t�@�C���ꗗ����
	std::vector<std::string> fileNameArray;
	std::vector<std::string> fileDirArray;
	LoadFileName(m_fileDir, fileNameArray, fileDirArray);

	//���f���̓ǂݍ���
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
			m_modelAnimation.back()->Play("アーマチュアAction", true, false);

			m_modelAnimationInRaytracing.back() = std::make_shared<AnimationInRaytracing>();

			//����
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

			//�����̒��_���̃R�s�[
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
	//�t�@�C�������̎Q�l����
	//https://qiita.com/tes2840/items/8d295b1caaf10eaf33ad

	using namespace std::filesystem;
	directory_iterator iter(arg_folderPath), end;
	std::error_code err;

	for (; iter != end && !err; iter.increment(err))
	{
		const directory_entry entry = *iter;
		//�t�H���_�������ꍇ��1�K�w�������
		if (entry.is_directory())
		{
			fileDir.emplace_back(entry.path().filename().string() + "/");
			LoadFileName(entry.path().string(), arg_file_names, fileDir);
		}
		//�t�@�C���Ȃ�t�H�[�}�b�g���m�F����
		else
		{
			//�g���q�擾�̎Q�l����
			//https://qiita.com/takano_tak/items/acf34b4a30cb974bab65
			std::string filename(entry.path().filename().string());
			int extI = static_cast<int>(filename.find_last_of("."));
			std::string extName = filename.substr(extI, filename.size() - extI);

			//�g���q��gltf��������glb�̏ꍇ�͓ǂݍ��ݑΏۂȂ̂ŕۑ�����
			if (extName == ".gltf" || extName == ".glb")
			{
				//�t�@�C�����擾
				arg_file_names.emplace_back(entry.path().filename().string());
				break;
			}
		}
	}

	/* �G���[���� */
	if (err)
	{
		std::cout << err.value() << std::endl;
		std::cout << err.message() << std::endl;
		assert(0);
	}
}

void ModelTool::Update()
{
	//���f���̃��[�h
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
	//���f�����\��
	for (int i = 0; i < m_modelInfomationArray.size(); ++i)
	{
		ImGui::RadioButton(m_modelInfomationArray[i].m_fileName.c_str(), &m_selectNum, i);
	}
	ImGui::EndChild();
	if (m_modelInfomationArray.size() != 0)
	{
		if (ImGui::TreeNode("Transform"))
		{
			//Transform������
			if (ImGui::Button("InitTransform"))
			{
				m_modelInfomationArray[m_selectNum].m_transform.pos = {};
				m_modelInfomationArray[m_selectNum].m_transform.scale = { 1.0f,1.0f,1.0f };
				m_modelInfomationArray[m_selectNum].m_transform.rotation = {};
			}
			//��]����
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
			*VertexBufferMgr::Instance()->GetVertexIndexBuffer(m_modelInfomationArray[m_selectNum].m_modelInfomation->modelVertDataHandle).indexBuffer[0]
		);
	}
	//KazImGuiHelper::InputVec3("DirectionalLight", &m_directionalLight);
	ImGui::End();

	//���f���`��
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
}

void ModelTool::DrawGrid(DrawingByRasterize& render)
{
	const float height = -5.0f;
	const float range = 50.0f;
	const KazMath::Color lineColor(49, 187, 134, 255);

	//���̐�����ׂ�
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
	//�c�̐�����ׂ�
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
