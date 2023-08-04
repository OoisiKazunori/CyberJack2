#pragma once
#include"../KazLibrary/Math/KazMath.h"
#include"../KazLibrary/Camera/CameraMgr.h"

class DebugCamera
{
public:
	DebugCamera();
	void Update();

private:
	KazMath::Vec3<float>m_eye, m_target, m_up;
	DirectX::XMMATRIX m_matRot;

	float m_distance;

	void Camera(bool arg_zoomFlag, bool arg_targetPosFlag, bool arg_rotationFlag, const KazMath::Vec3<float>& arg_mouseVel);

	void MoveVector(const DirectX::XMFLOAT3& move)
	{
		// 視点と注視点座標を移動し、反映
		DirectX::XMFLOAT3 eye_moved = m_eye.ConvertXMFLOAT3();
		DirectX::XMFLOAT3 target_moved = m_target.ConvertXMFLOAT3();

		eye_moved.x += move.x;
		eye_moved.y += move.y;
		eye_moved.z += move.z;

		target_moved.x += move.x;
		target_moved.y += move.y;
		target_moved.z += move.z;

		m_eye = KazMath::Vec3<float>(eye_moved.x, eye_moved.y, eye_moved.z);
		m_target = KazMath::Vec3<float>(target_moved.x, target_moved.y, target_moved.z);
	}

	void MoveVector(const DirectX::XMVECTOR& move)
	{
		// 視点と注視点座標を移動し、反映
		DirectX::XMFLOAT3 eye_moved = m_eye.ConvertXMFLOAT3();
		DirectX::XMFLOAT3 target_moved = m_target.ConvertXMFLOAT3();

		eye_moved.x += move.m128_f32[0];
		eye_moved.y += move.m128_f32[1];
		eye_moved.z += move.m128_f32[2];

		target_moved.x += move.m128_f32[0];
		target_moved.y += move.m128_f32[1];
		target_moved.z += move.m128_f32[2];

		m_eye = KazMath::Vec3<float>(eye_moved.x, eye_moved.y, eye_moved.z);
		m_target = KazMath::Vec3<float>(target_moved.x, target_moved.y, target_moved.z);
	}

};

