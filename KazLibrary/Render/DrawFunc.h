#pragma once
#include"../Buffer/DrawFuncData.h"
#include"../KazLibrary/Camera/CameraMgr.h"

namespace DrawFunc
{
	static CoordinateSpaceMatData GetMatrixBufferData(KazMath::Transform3D& arg_transform)
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
		NONE,		//何もしない
		REFRACTION,	//レイトレ側でオブジェクトを屈折させる
		REFLECTION	//レイトレ側でオブジェクトを反射させる
	};

	struct TextureRender
	{
		DrawFuncData::DrawCallData m_drawCommand;
		KazBufferHelper::BufferData m_textureBuffer;
		KazMath::Transform2D m_transform;

		TextureRender(const std::string& arg_textureFilePass, bool arg_alphaFlag = false)
		{
			if (arg_alphaFlag)
			{
				m_drawCommand = DrawFuncData::SetSpriteAlphaData(DrawFuncData::GetSpriteAlphaShader());
			}
			else
			{
				m_drawCommand = DrawFuncData::SetTexPlaneData(DrawFuncData::GetSpriteShader());
			}
			m_textureBuffer = TextureResourceMgr::Instance()->LoadGraphBuffer(arg_textureFilePass);
			m_transform.scale =
			{
				static_cast<float>(m_textureBuffer.bufferWrapper->GetBuffer().Get()->GetDesc().Width),
				static_cast<float>(m_textureBuffer.bufferWrapper->GetBuffer().Get()->GetDesc().Height)
			};
		};
		TextureRender()
		{
			m_drawCommand = DrawFuncData::SetTexPlaneData(DrawFuncData::GetSpriteShader());
		};

		void operator=(const KazBufferHelper::BufferData& rhs)
		{
			m_textureBuffer = rhs;
			m_transform.scale =
			{
				static_cast<float>(m_textureBuffer.bufferWrapper->GetBuffer().Get()->GetDesc().Width),
				static_cast<float>(m_textureBuffer.bufferWrapper->GetBuffer().Get()->GetDesc().Height)
			};
		};
	};

	static void DrawTextureIn2D(DrawFuncData::DrawCallData& arg_callData, const KazMath::Transform2D& arg_transform, const KazBufferHelper::BufferData& arg_texture)
	{
		//行列情報
		DirectX::XMMATRIX mat = arg_transform.GetMat() * CameraMgr::Instance()->GetOrthographicMatProjection();
		arg_callData.extraBufferArray[0].bufferWrapper->TransData(&mat, sizeof(DirectX::XMMATRIX));
		//テクスチャ情報
		arg_callData.extraBufferArray[1] = arg_texture;
		arg_callData.extraBufferArray[1].rangeType = GRAPHICS_RANGE_TYPE_SRV_DESC;
		arg_callData.extraBufferArray[1].rootParamType = GRAPHICS_PRAMTYPE_DATA;
	}

	static void DrawTextureIn2D(DrawFuncData::DrawCallData& arg_callData, const KazMath::Transform2D& arg_transform, const KazBufferHelper::BufferData& arg_texture, const KazMath::Color& arg_color)
	{
		//行列情報
		DirectX::XMMATRIX mat = arg_transform.GetMat() * CameraMgr::Instance()->GetOrthographicMatProjection();
		arg_callData.extraBufferArray[0].bufferWrapper->TransData(&mat, sizeof(DirectX::XMMATRIX));
		//色
		arg_callData.extraBufferArray[1].bufferWrapper->TransData(&arg_color.ConvertColorRateToXMFLOAT4(), sizeof(DirectX::XMFLOAT4));
		//テクスチャ情報
		arg_callData.extraBufferArray[2] = arg_texture;
		arg_callData.extraBufferArray[2].rangeType = GRAPHICS_RANGE_TYPE_SRV_DESC;
		arg_callData.extraBufferArray[2].rootParamType = GRAPHICS_PRAMTYPE_DATA;
	}


	static void DrawModelInRaytracing(DrawFuncData::DrawCallData& arg_callData, KazMath::Transform3D& arg_transform, RayTracingType arg_type)
	{
		//行列情報
		static CoordinateSpaceMatData transData(arg_transform.GetMat(), CameraMgr::Instance()->GetViewMatrix(), CameraMgr::Instance()->GetPerspectiveMatProjection());
		transData.m_world = arg_transform.GetMat();
		transData.m_projective = CameraMgr::Instance()->GetPerspectiveMatProjection();
		transData.m_view = CameraMgr::Instance()->GetViewMatrix();

		//クォータニオンに値が入っている or クォータニオンが単位行列じゃなかったらクォータニオンで回転行列を求める。
		if (0 < fabs(arg_transform.quaternion.m128_f32[3])) {
			transData.m_rotaion = DirectX::XMMatrixRotationQuaternion(arg_transform.quaternion);
		}
		else {
			transData.m_rotaion = KazMath::CaluRotaMatrix(arg_transform.rotation);
		}

		arg_callData.extraBufferArray[0].bufferWrapper->TransData(&transData, sizeof(CoordinateSpaceMatData));
		//ID
		UINT num = static_cast<UINT>(arg_type);
		arg_callData.extraBufferArray[1].bufferWrapper->TransData(&num, sizeof(UINT));
	}

	static void Test(DrawFuncData::DrawCallData& arg_callData, KazMath::Transform3D& arg_transform, RayTracingType arg_type)
	{
		//行列情報
		static CoordinateSpaceMatData transData(arg_transform.GetMat(), CameraMgr::Instance()->GetViewMatrix(), CameraMgr::Instance()->GetPerspectiveMatProjection());
		transData.m_world = arg_transform.GetMat();
		transData.m_projective = CameraMgr::Instance()->GetPerspectiveMatProjection();
		transData.m_view = CameraMgr::Instance()->GetViewMatrix();

		//クォータニオンに値が入っている or クォータニオンが単位行列じゃなかったらクォータニオンで回転行列を求める。
		if (0 < fabs(arg_transform.quaternion.m128_f32[3])) {
			transData.m_rotaion = DirectX::XMMatrixRotationQuaternion(arg_transform.quaternion);
		}
		else {
			transData.m_rotaion = KazMath::CaluRotaMatrix(arg_transform.rotation);
		}

	}

	static void DrawModelInRaytracing(DrawFuncData::DrawCallData& arg_callData, KazMath::Transform3D& arg_transform, RayTracingType arg_type, const KazMath::Color& arg_emissiveColor, const KazMath::Color& arg_albedoColor = KazMath::Color(255, 255, 255, 255))
	{
		//行列情報
		static CoordinateSpaceMatData transData(arg_transform.GetMat(), CameraMgr::Instance()->GetViewMatrix(), CameraMgr::Instance()->GetPerspectiveMatProjection());
		transData.m_world = arg_transform.GetMat();
		transData.m_projective = CameraMgr::Instance()->GetPerspectiveMatProjection();
		transData.m_view = CameraMgr::Instance()->GetViewMatrix();

		//クォータニオンに値が入っている or クォータニオンが単位行列じゃなかったらクォータニオンで回転行列を求める。
		if (0 < fabs(arg_transform.quaternion.m128_f32[3])) {
			transData.m_rotaion = DirectX::XMMatrixRotationQuaternion(arg_transform.quaternion);
		}
		else {
			transData.m_rotaion = KazMath::CaluRotaMatrix(arg_transform.rotation);
		}

		arg_callData.extraBufferArray[0].bufferWrapper->TransData(&transData, sizeof(CoordinateSpaceMatData));
		//ID
		UINT num = static_cast<UINT>(arg_type);
		arg_callData.extraBufferArray[1].bufferWrapper->TransData(&num, sizeof(UINT));

		//AlbedoColor
		arg_callData.extraBufferArray[2].bufferWrapper->TransData(&arg_albedoColor.GetColorRate(), sizeof(DirectX::XMFLOAT4));

		//EmissiveColor
		arg_callData.extraBufferArray[3].bufferWrapper->TransData(&arg_emissiveColor.GetColorRate(), sizeof(DirectX::XMFLOAT4));
	}

	static void DrawModel(DrawFuncData::DrawCallData& arg_callData, KazMath::Transform3D& arg_transform, const KazMath::Color& arg_color = KazMath::Color(255, 255, 255, 255))
	{
		//行列情報
		CoordinateSpaceMatData transData(arg_transform.GetMat(), CameraMgr::Instance()->GetViewMatrix(), CameraMgr::Instance()->GetPerspectiveMatProjection());
		transData.m_world = arg_transform.GetMat();
		transData.m_projective = CameraMgr::Instance()->GetPerspectiveMatProjection();
		transData.m_view = CameraMgr::Instance()->GetViewMatrix();

		//クォータニオンに値が入っている or クォータニオンが単位行列じゃなかったらクォータニオンで回転行列を求める。
		if (0 < fabs(arg_transform.quaternion.m128_f32[3])) {
			transData.m_rotaion = DirectX::XMMatrixRotationQuaternion(arg_transform.quaternion);
		}
		else {
			transData.m_rotaion = KazMath::CaluRotaMatrix(arg_transform.rotation);
		}

		arg_callData.extraBufferArray[0].bufferWrapper->TransData(&transData, sizeof(CoordinateSpaceMatData));

		arg_callData.extraBufferArray[1].bufferWrapper->TransData(&arg_color.ConvertColorRateToXMFLOAT4(), sizeof(DirectX::XMFLOAT4));
	}

	static void DrawModel(DrawFuncData::DrawCallData& arg_callData, KazMath::Transform3D& arg_transform, const KazBufferHelper::BufferData& arg_boneBuffer, const KazMath::Color& arg_color = KazMath::Color(255, 255, 255, 255))
	{
		//行列情報
		CoordinateSpaceMatData transData(arg_transform.GetMat(), CameraMgr::Instance()->GetViewMatrix(), CameraMgr::Instance()->GetPerspectiveMatProjection());
		transData.m_world = arg_transform.GetMat();
		transData.m_projective = CameraMgr::Instance()->GetPerspectiveMatProjection();
		transData.m_view = CameraMgr::Instance()->GetViewMatrix();

		//クォータニオンに値が入っている or クォータニオンが単位行列じゃなかったらクォータニオンで回転行列を求める。
		if (0 < fabs(arg_transform.quaternion.m128_f32[3])) {
			transData.m_rotaion = DirectX::XMMatrixRotationQuaternion(arg_transform.quaternion);
		}
		else {
			transData.m_rotaion = KazMath::CaluRotaMatrix(arg_transform.rotation);
		}

		arg_callData.extraBufferArray[0].bufferWrapper->TransData(&transData, sizeof(CoordinateSpaceMatData));

		arg_callData.extraBufferArray[1].bufferWrapper->TransData(&arg_color.ConvertColorRateToXMFLOAT4(), sizeof(DirectX::XMFLOAT4));

		arg_callData.extraBufferArray[2].rangeType = GRAPHICS_RANGE_TYPE_CBV_VIEW;
		arg_callData.extraBufferArray[2].rootParamType = GRAPHICS_PRAMTYPE_DATA3;

		arg_callData.extraBufferArray[3] = arg_boneBuffer;
		arg_callData.extraBufferArray[3].rangeType = GRAPHICS_RANGE_TYPE_CBV_VIEW;
		arg_callData.extraBufferArray[3].rootParamType = GRAPHICS_PRAMTYPE_DATA4;
	}

	static void DrawLine(DrawFuncData::DrawCallData& arg_callData, std::vector<KazMath::Vec3<float>>arg_limitPosArray, RESOURCE_HANDLE arg_vertexHandle, const KazMath::Color& arg_color = KazMath::Color(255, 255, 255, 255))
	{
		VertexBufferMgr::Instance()->GetVertexBuffer(arg_vertexHandle).vertBuffer->bufferWrapper->TransData(
			arg_limitPosArray.data(),
			KazBufferHelper::GetBufferSize<UINT>(arg_limitPosArray.size(), sizeof(DirectX::XMFLOAT3))
		);

		//行列情報
		DirectX::XMMATRIX mat = CameraMgr::Instance()->GetViewMatrix() * CameraMgr::Instance()->GetPerspectiveMatProjection();
		arg_callData.extraBufferArray[0].bufferWrapper->TransData(&mat, sizeof(DirectX::XMMATRIX));
		arg_callData.extraBufferArray[1].bufferWrapper->TransData(&arg_color.ConvertColorRateToXMFLOAT4(), sizeof(DirectX::XMFLOAT4));
	}

	static void DrawModelLight(DrawFuncData::DrawCallData& arg_callData, KazMath::Transform3D& arg_transform, KazMath::Vec3<float>& lightDir, const KazMath::Color& arg_color)
	{
		//行列情報
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
}