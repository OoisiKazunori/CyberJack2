#include "DebugCamera.h"
#include"../KazLibrary/Input/KeyBoradInputManager.h"
#include"../KazLibrary/Input/ControllerInputManager.h"

DebugCamera::DebugCamera() :m_distance(20.0f), m_matRot(DirectX::XMMatrixIdentity()), m_target(0.0f, 1.0f, 0.0f)
{
}

void DebugCamera::Update()
{
	Camera(
		KeyBoradInputManager::Instance()->MouseInputState(MOUSE_INPUT_MIDDLE),
		KeyBoradInputManager::Instance()->MouseInputState(MOUSE_INPUT_LEFT),
		KeyBoradInputManager::Instance()->MouseInputState(MOUSE_INPUT_RIGHT),
		KeyBoradInputManager::Instance()->GetMouseVel()
	);
}

void DebugCamera::Camera(bool arg_zoomFlag, bool arg_targetPosFlag, bool arg_rotationFlag, const KazMath::Vec3<float>& arg_mouseVel)
{
	bool dirty = false;
	float angleX = 0;
	float angleY = 0;

	const KazMath::Vec2<float> SCALE = { 0.01f,0.01f };


	// マウスの左ボタンが押されていたらカメラを回転させる
	if (arg_rotationFlag)
	{
		float dy = arg_mouseVel.x * SCALE.x;
		float dx = arg_mouseVel.y * SCALE.y;
		angleX = -dx * KazMath::PI_2F;
		angleY = -dy * KazMath::PI_2F;
		dirty = true;
	}

	// マウスの中ボタンが押されていたらカメラを並行移動させる
	if (arg_zoomFlag)
	{
		float dx = arg_mouseVel.x / 100.0f;
		float dy = arg_mouseVel.y / 100.0f;

		DirectX::XMVECTOR move = { -dx, +dy, 0, 0 };
		move = DirectX::XMVector3Transform(move, m_matRot);

		MoveVector(move);
		dirty = true;
	}

	// ホイール入力で距離を変更
	if (arg_mouseVel.z != 0)
	{
		m_distance -= arg_mouseVel.z / 100.0f;
		m_distance = max(m_distance, 1.0f);
		dirty = true;
	}

	if (dirty)
	{
		// 追加回転分の回転行列を生成
		DirectX::XMMATRIX matRotNew = DirectX::XMMatrixIdentity();
		matRotNew *= DirectX::XMMatrixRotationX(-angleX);
		matRotNew *= DirectX::XMMatrixRotationY(-angleY);
		// 累積の回転行列を合成
		// ※回転行列を累積していくと、誤差でスケーリングがかかる危険がある為
		// クォータニオンを使用する方が望ましい
		m_matRot = matRotNew * m_matRot;

		// 注視点から視点へのベクトルと、上方向ベクトル
		DirectX::XMVECTOR vTargetEye = { 0.0f, 0.0f, -m_distance, 1.0f };
		DirectX::XMVECTOR vUp = { 0.0f, 1.0f, 0.0f, 0.0f };

		// ベクトルを回転
		vTargetEye = DirectX::XMVector3Transform(vTargetEye, m_matRot);
		vUp = DirectX::XMVector3Transform(vUp, m_matRot);

		// 注視点からずらした位置に視点座標を決定
		const DirectX::XMFLOAT3& target = m_target.ConvertXMFLOAT3();
		m_eye = { target.x + vTargetEye.m128_f32[0], target.y + vTargetEye.m128_f32[1], target.z + vTargetEye.m128_f32[2] };
		m_up = { vUp.m128_f32[0], vUp.m128_f32[1], vUp.m128_f32[2] };
	}

	CameraMgr::Instance()->Camera(m_eye, m_target, m_up);
};