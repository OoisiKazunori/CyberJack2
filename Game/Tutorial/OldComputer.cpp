#include "OldComputer.h"
#include"../KazLibrary/Helper/ResourceFilePass.h"
#include"../KazLibrary/Imgui/MyImgui.h"

OldComputer::OldComputer()
{
	render.data.handle = ObjResourceMgr::Instance()->LoadModel(KazFilePathName::StagePath + "PC/" + "PC_Model.obj");
	render.data.pipelineName = PIPELINE_NAME_PCMONITOR_MULTITEX;


	dataHandle = render.CreateConstBuffer(sizeof(Data), typeid(Data).name(), GRAPHICS_RANGE_TYPE_CBV_VIEW, GRAPHICS_PRAMTYPE_DATA2);
}

void OldComputer::Init(RESOURCE_HANDLE MOVIE_HANDLE)
{
	//ƒ}ƒXƒN‰æ‘œ
	render.data.addHandle.handle[0] = TextureResourceMgr::Instance()->LoadGraph(KazFilePathName::StagePath + "PC/" + "PC_Mask.png");
	render.data.addHandle.paramType[0] = GRAPHICS_PRAMTYPE_TEX2;
	//“®‰æ‰æ‘œ
	render.data.addHandle.handle[1] = MOVIE_HANDLE;
	render.data.addHandle.paramType[1] = GRAPHICS_PRAMTYPE_TEX3;

	render.data.transform.pos = { 0.0f,-76.0f,80.0f };
	render.data.transform.scale = { 70.0f,70.0f,49.0f };
	render.data.transform.rotation.y = 180.0f;
}

void OldComputer::Update()
{
	/*ImGui::Begin("PC");
	KazImGuiHelper::InputTransform3D("Transform", &render.data.transform);
	ImGui::End();*/


	render.TransData(&data, dataHandle, typeid(Data).name());

}

void OldComputer::Draw()
{
	render.Draw();
}

void OldComputer::SetMonitorTexture(RESOURCE_HANDLE MOVIE_HANDLE)
{
	render.data.addHandle.handle[1] = MOVIE_HANDLE;
	render.data.addHandle.paramType[1] = GRAPHICS_PRAMTYPE_TEX3;
}

void OldComputer::SetTransform(const KazMath::Transform3D &TRANSFORM)
{
	render.data.transform = TRANSFORM;
}
