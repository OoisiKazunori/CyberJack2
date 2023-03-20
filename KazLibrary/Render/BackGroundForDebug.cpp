#include"../Render/BackGroundForDebug.h"

BackGroundForDebug::BackGroundForDebug(int CAMERA_INDEX) :color(41, 163, 231, 255), cameraId(CAMERA_INDEX)
{
	for (int i = 0; i < 100; i++)
	{
		horizontalLinePos[0][i].x = 900.0f;
		horizontalLinePos[0][i].y = -10.0f;
		horizontalLinePos[0][i].z = -1000.0f + i * 20.0f;
		horizontalLinePos[1][i].x = -3000.0f;
		horizontalLinePos[1][i].y = -10.0f;
		horizontalLinePos[1][i].z = -1000.0f + i * 20.0f;
	}

	for (int i = 0; i < 100; i++)
	{
		verticalLinePos[0][i].x = -1000.0f + i * 20.0f;
		verticalLinePos[0][i].y = -10.0f;
		verticalLinePos[0][i].z = 1000.0f;
		verticalLinePos[1][i].x = -1000.0f + i * 20.0f;
		verticalLinePos[1][i].y = -10.0f;
		verticalLinePos[1][i].z = -2000.0f;
	}

	//235,5,111
}

void BackGroundForDebug::Draw()
{
	PipeLineNames pipeline = PIPELINE_NAME_LINE;
	PIXBeginEvent(DirectX12CmdList::Instance()->cmdList.Get(), 0, L"Draw BackGround");
#pragma region äÓèÄê¸
	for (int yoko = 0; yoko < 100; yoko++)
	{
		horizontalLine[yoko].data.startPos = { horizontalLinePos[0][yoko].x, horizontalLinePos[0][yoko].y, horizontalLinePos[0][yoko].z };
		horizontalLine[yoko].data.endPos = { horizontalLinePos[1][yoko].x, horizontalLinePos[1][yoko].y, horizontalLinePos[1][yoko].z };
		horizontalLine[yoko].data.colorData = color;
		horizontalLine[yoko].data.pipelineName = pipeline;
		horizontalLine[yoko].data.cameraIndex = cameraId;

		horizontalLine[yoko].Draw();
	}

	for (int tate = 0; tate < 100; tate++)
	{
		verticalLine[tate].data.startPos = { verticalLinePos[0][tate].x,  verticalLinePos[0][tate].y,  verticalLinePos[0][tate].z };
		verticalLine[tate].data.endPos = { verticalLinePos[1][tate].x,verticalLinePos[1][tate].y, verticalLinePos[1][tate].z };
		verticalLine[tate].data.colorData = color;
		verticalLine[tate].data.pipelineName = pipeline;
		verticalLine[tate].data.cameraIndex = cameraId;

		verticalLine[tate].Draw();
	}
#pragma endregion
	PIXEndEvent(DirectX12CmdList::Instance()->cmdList.Get());
	xLine.data.startPos = { 0,0,0 };
	xLine.data.endPos = { 30,0,0 };
	xLine.data.colorData = { 255,0,0,255 };
	xLine.data.pipelineName = pipeline;
	xLine.Draw();

	yLine.data.startPos = { 0,0,0 };
	yLine.data.endPos = { 0,30,0 };
	yLine.data.colorData = { 0,255,0,255 };
	yLine.data.pipelineName = pipeline;
	yLine.Draw();

	zLine.data.startPos = { 0,0,0 };
	zLine.data.endPos = { 0,0,30 };
	zLine.data.colorData = { 0,0,255,255 };
	zLine.data.pipelineName = pipeline;
	zLine.Draw();
}
