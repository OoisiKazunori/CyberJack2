#pragma once
#include"../Buffer/DrawFuncData.h"
#include"../KazLibrary/Camera/CameraMgr.h"


namespace DrawFunc
{
	static CoordinateSpaceMatData GetMatrixBufferData(const KazMath::Transform3D& arg_transform)
	{
		CoordinateSpaceMatData transData(arg_transform.GetMat(), CameraMgr::Instance()->GetViewMatrix(), CameraMgr::Instance()->GetPerspectiveMatProjection());
		transData.m_world = arg_transform.GetMat();
		transData.m_projective = CameraMgr::Instance()->GetPerspectiveMatProjection();
		transData.m_view = CameraMgr::Instance()->GetViewMatrix();

		DirectX::XMVECTOR pos, scale, rotaQ;
		DirectX::XMMatrixDecompose(&pos, &scale, &rotaQ, arg_transform.GetMat());
		DirectX::XMMATRIX rotaMat = DirectX::XMMatrixRotationQuaternion(rotaQ);
		transData.m_rotaion = rotaMat;
		return transData;
	}

	enum RayTracingType
	{
		NONE,		//�������Ȃ�
		REFRACTION,	//���C�g�����ŃI�u�W�F�N�g�����܂�����
		REFLECTION	//���C�g�����ŃI�u�W�F�N�g�𔽎˂�����
	};

	static void DrawTextureIn2D(DrawFuncData::DrawCallData& arg_callData, const KazMath::Transform2D& arg_transform, const KazBufferHelper::BufferData& arg_texture)
	{
		//�s����
		static DirectX::XMMATRIX mat = arg_transform.GetMat() * CameraMgr::Instance()->GetOrthographicMatProjection();
		arg_callData.extraBufferArray[0].bufferWrapper->TransData(&mat, sizeof(DirectX::XMMATRIX));
		//�e�N�X�`�����
		arg_callData.extraBufferArray[1] = arg_texture;
		arg_callData.extraBufferArray[1].rangeType = GRAPHICS_RANGE_TYPE_SRV_DESC;
		arg_callData.extraBufferArray[1].rootParamType = GRAPHICS_PRAMTYPE_DATA;
	}

	static void DrawModelInRaytracing(DrawFuncData::DrawCallData& arg_callData, const KazMath::Transform3D& arg_transform, RayTracingType arg_type)
	{
		//�s����
		static CoordinateSpaceMatData transData(arg_transform.GetMat(), CameraMgr::Instance()->GetViewMatrix(), CameraMgr::Instance()->GetPerspectiveMatProjection());
		transData.m_world = arg_transform.GetMat();
		transData.m_projective = CameraMgr::Instance()->GetPerspectiveMatProjection();
		transData.m_view = CameraMgr::Instance()->GetViewMatrix();

		DirectX::XMVECTOR pos, scale, rotaQ;
		DirectX::XMMatrixDecompose(&pos, &scale, &rotaQ, arg_transform.GetMat());
		DirectX::XMMATRIX rotaMat = DirectX::XMMatrixRotationQuaternion(rotaQ);
		transData.m_rotaion = rotaMat;

		arg_callData.extraBufferArray[0].bufferWrapper->TransData(&transData, sizeof(CoordinateSpaceMatData));
		//ID
		UINT num = static_cast<UINT>(arg_type);
		arg_callData.extraBufferArray[1].bufferWrapper->TransData(&num, sizeof(UINT));
	}

	static void DrawModel(DrawFuncData::DrawCallData& arg_callData, const KazMath::Transform3D& arg_transform, const KazMath::Color& arg_color)
	{
		//�s����
		static CoordinateSpaceMatData transData(arg_transform.GetMat(), CameraMgr::Instance()->GetViewMatrix(), CameraMgr::Instance()->GetPerspectiveMatProjection());
		transData.m_world = arg_transform.GetMat();
		transData.m_projective = CameraMgr::Instance()->GetPerspectiveMatProjection();
		transData.m_view = CameraMgr::Instance()->GetViewMatrix();

		DirectX::XMVECTOR pos, scale, rotaQ;
		DirectX::XMMatrixDecompose(&pos, &scale, &rotaQ, arg_transform.GetMat());
		DirectX::XMMATRIX rotaMat = DirectX::XMMatrixRotationQuaternion(rotaQ);
		transData.m_rotaion = rotaMat;

		arg_callData.extraBufferArray[0].bufferWrapper->TransData(&transData, sizeof(CoordinateSpaceMatData));
		arg_callData.extraBufferArray[1].bufferWrapper->TransData(&arg_color.ConvertColorRateToXMFLOAT4(), sizeof(DirectX::XMFLOAT4));
	}

	static void DrawModelLight(DrawFuncData::DrawCallData& arg_callData, const KazMath::Transform3D& arg_transform,KazMath::Vec3<float> &lightDir, const KazMath::Color& arg_color)
	{
		//�s����
		static CoordinateSpaceMatData transData(arg_transform.GetMat(), CameraMgr::Instance()->GetViewMatrix(), CameraMgr::Instance()->GetPerspectiveMatProjection());
		transData.m_world = arg_transform.GetMat();
		transData.m_projective = CameraMgr::Instance()->GetPerspectiveMatProjection();
		transData.m_view = CameraMgr::Instance()->GetViewMatrix();

		DirectX::XMVECTOR pos, scale, rotaQ;
		DirectX::XMMatrixDecompose(&pos, &scale, &rotaQ, arg_transform.GetMat());
		DirectX::XMMATRIX rotaMat = DirectX::XMMatrixRotationQuaternion(rotaQ);
		transData.m_rotaion = rotaMat;

		arg_callData.extraBufferArray[0].bufferWrapper->TransData(&transData, sizeof(CoordinateSpaceMatData));
		arg_callData.extraBufferArray[1].bufferWrapper->TransData(&lightDir, sizeof(DirectX::XMFLOAT3));
		arg_callData.extraBufferArray[2].bufferWrapper->TransData(&arg_color.ConvertColorRateToXMFLOAT4(), sizeof(DirectX::XMFLOAT4));
	}

	static void DrawLine(DrawFuncData::DrawCallData& arg_callData, const KazMath::Vec3<float>& arg_startPos, const KazMath::Vec3<float>& arg_endPos, const KazMath::Color& arg_color)
	{
		std::array<DirectX::XMFLOAT3, 2>vertArray;
		vertArray[0] = arg_startPos.ConvertXMFLOAT3();
		vertArray[1] = arg_endPos.ConvertXMFLOAT3();
		//���_���𓮂����Đ��̈ʒu�����߂�
		VertexBufferMgr::Instance()->GetBuffer(arg_callData.m_modelVertDataHandle).vertBuffer[0]->bufferWrapper->TransData(vertArray.data(), KazBufferHelper::GetBufferSize<int>(vertArray.size(), sizeof(DirectX::XMFLOAT3)));

		DirectX::XMMATRIX mat(KazMath::Transform3D().GetMat() * CameraMgr::Instance()->GetViewMatrix() * CameraMgr::Instance()->GetPerspectiveMatProjection());
		//�s����
		arg_callData.extraBufferArray[0].bufferWrapper->TransData(&mat, sizeof(DirectX::XMMATRIX));
		arg_callData.extraBufferArray[1].bufferWrapper->TransData(&arg_color.ConvertColorRateToXMFLOAT4(), sizeof(DirectX::XMFLOAT4));
	}
}