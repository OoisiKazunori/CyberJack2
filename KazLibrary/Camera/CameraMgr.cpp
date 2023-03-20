#include "CameraMgr.h"
#include"../Math/KazMath.h"

CameraMgr::CameraMgr() : orthographicMatProjectionDirtyFlag(orthographicMatProjection)
{

	//2D座標変換
	orthographicMatProjection = DirectX::XMMatrixOrthographicOffCenterLH(
		0.0f,
		(float)WIN_X,
		(float)WIN_Y,
		0.0f,
		0.0f,
		1.0f
	);

	for (int i = 0; i < CAMERA_ARRAY_NUM; i++)
	{
		viewArray[i] = DirectX::XMMatrixIdentity();
		viewDirtyFlagArray[i] = std::make_unique<DirtySet>(viewArray[i]);
		billBoardArray[i] = DirectX::XMMatrixIdentity();
		billBoardDirtyFlagArray[i] = std::make_unique<DirtySet>(billBoardArray[i]);
		perspectiveMatArray[i] =
			DirectX::XMMatrixPerspectiveFovLH(
				DirectX::XMConvertToRadians(60.0f),
				static_cast<float>(WIN_X) / static_cast<float>(WIN_Y),
				0.1f,
				700.0f
			);
		perspectiveProjDirtyFlag[i] = std::make_unique<DirtySet>(perspectiveMatArray[i]);
	}
}

void CameraMgr::CameraSetting(float VIEWING_ANGLE, float FAR_SIDE, int CAMERA_INDEX)
{
	perspectiveMatArray[CAMERA_INDEX] =
		DirectX::XMMatrixPerspectiveFovLH(
			DirectX::XMConvertToRadians(VIEWING_ANGLE),
			static_cast<float>(WIN_X) / static_cast<float>(WIN_Y),
			0.1f,
			FAR_SIDE
		);
}

void CameraMgr::Camera(const KazMath::Vec3<float> &EYE_POS, const KazMath::Vec3<float> &TARGET_POS, const KazMath::Vec3<float> &UP, int CAMERA_INDEX)
{
	DirectX::XMFLOAT3 eye = { EYE_POS.x,EYE_POS.y,EYE_POS.z };
	DirectX::XMFLOAT3 target = { TARGET_POS.x,TARGET_POS.y ,TARGET_POS.z };
	DirectX::XMFLOAT3 up = { UP.x,UP.y,UP.z };
	CameraAxis cameraAxis;


	bool eyeAll0 = (eye.x == 0 && eye.y == 0 && eye.z == 0);
	//bool targetAll0 = (target.x == 0 && target.y == 0 && target.z == 0);
	bool eyeEqualTarget = (eye.x == target.x && eye.y == target.y && eye.z == target.z);

	if (eyeAll0 || eyeEqualTarget)
	{
		return;
	}

#pragma region ビュー行列
	DirectX::XMMATRIX matView;
	// 視点座標
	DirectX::XMVECTOR eyePosition = XMLoadFloat3(&eye);
	// 注視点座標
	DirectX::XMVECTOR targetPosition = XMLoadFloat3(&target);
	// （仮の）上方向
	DirectX::XMVECTOR upVector = XMLoadFloat3(&up);

	// カメラZ軸（視線方向）
	cameraAxis.z = DirectX::XMVectorSubtract(targetPosition, eyePosition);
	// 0ベクトルだと向きが定まらないので除外
	assert(!DirectX::XMVector3Equal(cameraAxis.z, DirectX::XMVectorZero()));
	assert(!DirectX::XMVector3IsInfinite(cameraAxis.z));
	assert(!DirectX::XMVector3Equal(upVector, DirectX::XMVectorZero()));
	assert(!DirectX::XMVector3IsInfinite(upVector));
	// ベクトルを正規化
	cameraAxis.z = DirectX::XMVector3Normalize(cameraAxis.z);

	// カメラのX軸（右方向）
	// X軸は上方向→Z軸の外積で求まる
	cameraAxis.x = DirectX::XMVector3Cross(upVector, cameraAxis.z);
	// ベクトルを正規化
	cameraAxis.x = DirectX::XMVector3Normalize(cameraAxis.x);

	// カメラのY軸（上方向）
	// Y軸はZ軸→X軸の外積で求まる
	cameraAxis.y = DirectX::XMVector3Cross(cameraAxis.z, cameraAxis.x);

	cameraAxis.upVec = upVector;


	// カメラ回転行列
	DirectX::XMMATRIX matCameraRot;
	// カメラ座標系→ワールド座標系の変換行列
	matCameraRot.r[0] = cameraAxis.x;
	matCameraRot.r[1] = cameraAxis.y;
	matCameraRot.r[2] = cameraAxis.z;
	matCameraRot.r[3] = DirectX::XMVectorSet(0, 0, 0, 1);


	// 転置により逆行列（逆回転）を計算
	matView = XMMatrixTranspose(matCameraRot);

	// 視点座標に-1を掛けた座標
	DirectX::XMVECTOR reverseEyePosition = DirectX::XMVectorNegate(eyePosition);
	// カメラの位置からワールド原点へのベクトル（カメラ座標系）
	DirectX::XMVECTOR tX = DirectX::XMVector3Dot(cameraAxis.x, reverseEyePosition);	// X成分
	DirectX::XMVECTOR tY = DirectX::XMVector3Dot(cameraAxis.y, reverseEyePosition);	// Y成分
	DirectX::XMVECTOR tZ = DirectX::XMVector3Dot(cameraAxis.z, reverseEyePosition);	// Z成分
	const int x = 0;
	const int y = 1;
	const int z = 2;
	// 一つのベクトルにまとめる
	DirectX::XMVECTOR translation = { tX.m128_f32[x], tY.m128_f32[y], tZ.m128_f32[z],1.0f };
	// ビュー行列に平行移動成分を設定
	matView.r[3] = translation;
#pragma endregion

	//全方位ビルボード
	DirectX::XMMATRIX matBillboard;
	matBillboard.r[0] = cameraAxis.x;
	matBillboard.r[1] = cameraAxis.y;
	matBillboard.r[2] = cameraAxis.z;
	matBillboard.r[3] = DirectX::XMVectorSet(0, 0, 0, 1);

	billBoardArray[CAMERA_INDEX] = matBillboard;
#pragma region Y軸ビルボード行列
	// カメラX軸、Y軸、Z軸
	DirectX::XMVECTOR billCameraAxisX, billCameraAxisY, billCameraAxisZ;

	// X軸は共通
	billCameraAxisX = cameraAxis.x;
	// Y軸はワールド座標系のY軸
	billCameraAxisY = DirectX::XMVector3Normalize(cameraAxis.upVec);
	// Z軸はX軸→Y軸の外積で求まる
	billCameraAxisZ = DirectX::XMVector3Cross(billCameraAxisX, billCameraAxisY);

	// Y軸回りビルボード行列
	matBillboard.r[0] = billCameraAxisX;
	matBillboard.r[1] = billCameraAxisY;
	matBillboard.r[2] = billCameraAxisZ;
	matBillboard.r[3] = DirectX::XMVectorSet(0, 0, 0, 1);

	yBillBoardArray[CAMERA_INDEX] = matBillboard;
#pragma endregion

	viewArray[CAMERA_INDEX] = matView;
	//view = matView;
	//billBoard = matBillboard;
}

DirectX::XMMATRIX CameraMgr::CreateCamera(const KazMath::Vec3<float> &EYE_POS, const KazMath::Vec3<float> &TARGET_POS, const KazMath::Vec3<float> &UP)
{
	DirectX::XMFLOAT3 eye = { EYE_POS.x,EYE_POS.y,EYE_POS.z };
	DirectX::XMFLOAT3 target = { TARGET_POS.x, TARGET_POS.y, TARGET_POS.z };
	DirectX::XMFLOAT3 up = { UP.x,UP.y,UP.z };
	CameraAxis cameraAxis;


	bool eyeAll0 = (eye.x == 0 && eye.y == 0 && eye.z == 0);
	//bool targetAll0 = (target.x == 0 && target.y == 0 && target.z == 0);
	bool eyeEqualTarget = (eye.x == target.x && eye.y == target.y && eye.z == target.z);

	if (eyeAll0 || eyeEqualTarget)
	{
		return DirectX::XMMATRIX();
	}

#pragma region ビュー行列
	DirectX::XMMATRIX matView;
	// 視点座標
	DirectX::XMVECTOR eyePosition = XMLoadFloat3(&eye);
	// 注視点座標
	DirectX::XMVECTOR targetPosition = XMLoadFloat3(&target);
	// （仮の）上方向
	DirectX::XMVECTOR upVector = XMLoadFloat3(&up);

	// カメラZ軸（視線方向）
	cameraAxis.z = DirectX::XMVectorSubtract(targetPosition, eyePosition);
	// 0ベクトルだと向きが定まらないので除外
	assert(!DirectX::XMVector3Equal(cameraAxis.z, DirectX::XMVectorZero()));
	assert(!DirectX::XMVector3IsInfinite(cameraAxis.z));
	assert(!DirectX::XMVector3Equal(upVector, DirectX::XMVectorZero()));
	assert(!DirectX::XMVector3IsInfinite(upVector));
	// ベクトルを正規化
	cameraAxis.z = DirectX::XMVector3Normalize(cameraAxis.z);

	// カメラのX軸（右方向）
	// X軸は上方向→Z軸の外積で求まる
	cameraAxis.x = DirectX::XMVector3Cross(upVector, cameraAxis.z);
	// ベクトルを正規化
	cameraAxis.x = DirectX::XMVector3Normalize(cameraAxis.x);

	// カメラのY軸（上方向）
	// Y軸はZ軸→X軸の外積で求まる
	cameraAxis.y = DirectX::XMVector3Cross(cameraAxis.z, cameraAxis.x);

	cameraAxis.upVec = upVector;


	// カメラ回転行列
	DirectX::XMMATRIX matCameraRot;
	// カメラ座標系→ワールド座標系の変換行列
	matCameraRot.r[0] = cameraAxis.x;
	matCameraRot.r[1] = cameraAxis.y;
	matCameraRot.r[2] = cameraAxis.z;
	matCameraRot.r[3] = DirectX::XMVectorSet(0, 0, 0, 1);


	// 転置により逆行列（逆回転）を計算
	matView = XMMatrixTranspose(matCameraRot);

	// 視点座標に-1を掛けた座標
	DirectX::XMVECTOR reverseEyePosition = DirectX::XMVectorNegate(eyePosition);
	// カメラの位置からワールド原点へのベクトル（カメラ座標系）
	DirectX::XMVECTOR tX = DirectX::XMVector3Dot(cameraAxis.x, reverseEyePosition);	// X成分
	DirectX::XMVECTOR tY = DirectX::XMVector3Dot(cameraAxis.y, reverseEyePosition);	// Y成分
	DirectX::XMVECTOR tZ = DirectX::XMVector3Dot(cameraAxis.z, reverseEyePosition);	// Z成分
	const int x = 0;
	const int y = 1;
	const int z = 2;
	// 一つのベクトルにまとめる
	DirectX::XMVECTOR translation = DirectX::XMVectorSet(tX.m128_f32[x], tY.m128_f32[y], tZ.m128_f32[z], 1.0f);
	// ビュー行列に平行移動成分を設定
	matView.r[3] = translation;
#pragma endregion

	return matView;
}

DirectX::XMMATRIX CameraMgr::GetViewMatrix(int CAMERA_INDEX)
{
	return viewArray[CAMERA_INDEX];
}

DirectX::XMMATRIX *CameraMgr::GetViewMatrixPointer(int CAMERA_INDEX)
{
	return &viewArray[CAMERA_INDEX];
}

DirectX::XMMATRIX CameraMgr::GetMatBillBoard(int CAMERA_INDEX)
{
	return billBoardArray[CAMERA_INDEX];
}

DirectX::XMMATRIX *CameraMgr::GetMatBillBoardPointer(int CAMERA_INDEX)
{
	return &billBoardArray[CAMERA_INDEX];
}

DirectX::XMMATRIX CameraMgr::GetPerspectiveMatProjection(int CAMERA_INDEX)
{
	return perspectiveMatArray[CAMERA_INDEX];
}

DirectX::XMMATRIX *CameraMgr::GetPerspectiveMatProjectionPointer(int CAMERA_INDEX)
{
	return &perspectiveMatArray[CAMERA_INDEX];
}

DirectX::XMMATRIX CameraMgr::GetOrthographicMatProjection()
{
	return orthographicMatProjection;
}

DirectX::XMMATRIX CameraMgr::GetPerspectiveMatProjectionAngle(float angle)
{
	return DirectX::XMMatrixPerspectiveFovLH(
		DirectX::XMConvertToRadians(angle),
		static_cast<float>(WIN_X) / static_cast<float>(WIN_Y),
		0.1f,
		100000
	);;
}

bool CameraMgr::ViewAndProjDirty(int CAMERA_INDEX)
{
	return viewDirtyFlagArray[CAMERA_INDEX]->Dirty() || perspectiveProjDirtyFlag[CAMERA_INDEX]->Dirty();
}

bool CameraMgr::BillboardDirty(int CAMERA_INDEX)
{
	return billBoardDirtyFlagArray[CAMERA_INDEX]->Dirty();
}

void CameraMgr::Record()
{
	for (int i = 0; i < CAMERA_ARRAY_NUM; ++i)
	{
		viewDirtyFlagArray[i]->Record();
		billBoardDirtyFlagArray[i]->Record();
		perspectiveProjDirtyFlag[i]->Record();
	}
}
