#pragma once
#include"../Buffer/DrawFuncData.h"
#include"../KazLibrary/Camera/CameraMgr.h"


namespace DrawFunc
{
	enum RayTracingType
	{
		NONE,		//何もしない
		REFRACTION,	//レイトレ側でオブジェクトを屈折させる
		REFLECTION	//レイトレ側でオブジェクトを反射させる
	};

	static void DrawTextureIn2D(DrawFuncData::DrawCallData &arg_callData, const KazMath::Transform2D &arg_transform, const KazBufferHelper::BufferData &arg_texture)
	{
		//行列情報
		static DirectX::XMMATRIX mat = arg_transform.GetMat() * CameraMgr::Instance()->GetOrthographicMatProjection();
		arg_callData.extraBufferArray[0].bufferWrapper->TransData(&mat, sizeof(DirectX::XMMATRIX));
		//テクスチャ情報
		arg_callData.extraBufferArray[1] = arg_texture;
		arg_callData.extraBufferArray[1].rangeType = GRAPHICS_RANGE_TYPE_SRV_DESC;
		arg_callData.extraBufferArray[1].rootParamType = GRAPHICS_PRAMTYPE_DATA;
	}

	static void DrawModelInRaytracing(DrawFuncData::DrawCallData &arg_callData, const KazMath::Transform3D &arg_transform, RayTracingType arg_type)
	{
		//行列情報
		static CoordinateSpaceMatData transData(arg_transform.GetMat(), CameraMgr::Instance()->GetViewMatrix(), CameraMgr::Instance()->GetPerspectiveMatProjection());
		arg_callData.extraBufferArray[0].bufferWrapper->TransData(&transData, sizeof(CoordinateSpaceMatData));
		//ID
		UINT num = static_cast<UINT>(arg_type);
		arg_callData.extraBufferArray[1].bufferWrapper->TransData(&num, sizeof(UINT));
	}
}