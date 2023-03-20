#pragma once
#include"../DirectXCommon/Base.h"
#pragma warning(push)
#pragma warning(disable:4023)
#include"imgui.h"
#include"imgui_impl_win32.h"
#include"imgui_impl_dx12.h"
#include"imgui_internal.h"
#include"imstb_rectpack.h"
#include"imstb_textedit.h"
#include"imstb_truetype.h"
#include"imconfig.h"
#pragma warning(pop)
#include"../KazLibrary/Math/KazMath.h"

/// <summary>
/// ImGuiの為のデスクリプタヒープの生成とImGUiの初期化を行います
/// なお、ImGuiの関数を呼び出すためのhもまとめて書いています
/// </summary>
class MyImgui
{
public:
	MyImgui();
	~MyImgui();

	/// <summary>
	/// ImGuiの初期化とデスクリプタヒープの初期化
	/// </summary>
	/// <param name="hwnd">ウィンドウハンドル</param>
	void Create(HWND hwnd);

	/// <summary>
	/// ImGuiの描画開始
	/// </summary>
	void NewFlame();

	/// <summary>
	/// コマンドリストに積む
	/// </summary>
	void Set();

private:
	/// <summary>
	/// ヒープの生成
	/// </summary>
	/// <returns>デスクリプタヒープ</returns>
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> CreateDescriptorHeapForImgui();
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>heapForImgui;

};


namespace KazImGuiHelper
{
	inline void InputTransform3D(const std::string &NAME, KazMath::Transform3D *TRANSFORM)
	{
		static std::string lLabel = "";
		lLabel = NAME + "_PosX";
		ImGui::DragFloat(lLabel.c_str(), &TRANSFORM->pos.x);
		lLabel = NAME + "_PosY";
		ImGui::DragFloat(lLabel.c_str(), &TRANSFORM->pos.y);
		lLabel = NAME + "_PosZ";
		ImGui::DragFloat(lLabel.c_str(), &TRANSFORM->pos.z);

		lLabel = NAME + "_ScaleX";
		ImGui::DragFloat(lLabel.c_str(), &TRANSFORM->scale.x);
		lLabel = NAME + "_ScaleY";
		ImGui::DragFloat(lLabel.c_str(), &TRANSFORM->scale.y);
		lLabel = NAME + "_ScaleZ";
		ImGui::DragFloat(lLabel.c_str(), &TRANSFORM->scale.z);

		lLabel = NAME + "_RotationX";
		ImGui::DragFloat(lLabel.c_str(), &TRANSFORM->rotation.x);
		lLabel = NAME + "_RotationY";
		ImGui::DragFloat(lLabel.c_str(), &TRANSFORM->rotation.y);
		lLabel = NAME + "_RotationZ";
		ImGui::DragFloat(lLabel.c_str(), &TRANSFORM->rotation.z);
	};

	inline void InputTransform2D(const std::string &NAME, KazMath::Transform2D *TRANSFORM)
	{
		static std::string lLabel = "";
		lLabel = NAME + "_PosX";
		ImGui::DragFloat(lLabel.c_str(), &TRANSFORM->pos.x);
		lLabel = NAME + "_PosY";
		ImGui::DragFloat(lLabel.c_str(), &TRANSFORM->pos.y);

		lLabel = NAME + "_ScaleX";
		ImGui::DragFloat(lLabel.c_str(), &TRANSFORM->scale.x);
		lLabel = NAME + "_ScaleY";
		ImGui::DragFloat(lLabel.c_str(), &TRANSFORM->scale.y);

		lLabel = NAME + "_Rotation";
		ImGui::DragFloat(lLabel.c_str(), &TRANSFORM->rotation);
	};

	inline void InputVec4(const std::string &LABEL, KazMath::Vec4<int> *VEC_4)
	{
		static std::string label = "";
		label = LABEL + "X";
		ImGui::InputInt(label.c_str(), &VEC_4->x);
		label = LABEL + "Y";
		ImGui::InputInt(label.c_str(), &VEC_4->y);
		label = LABEL + "Z";
		ImGui::InputInt(label.c_str(), &VEC_4->z);
		label = LABEL + "W";
		ImGui::InputInt(label.c_str(), &VEC_4->a);
	};

	inline void InputVec4(const std::string &LABEL, KazMath::Vec4<float> *VEC_4)
	{
		static std::string label = "";
		label = LABEL + "X";
		ImGui::DragFloat(label.c_str(), &VEC_4->x);
		label = LABEL + "Y";
		ImGui::DragFloat(label.c_str(), &VEC_4->y);
		label = LABEL + "Z";
		ImGui::DragFloat(label.c_str(), &VEC_4->z);
		label = LABEL + "W";
		ImGui::DragFloat(label.c_str(), &VEC_4->a);
	};

	inline void InputVec3(const std::string &LABEL, KazMath::Vec3<float> *VEC_3)
	{
		static std::string lLabel = "";
		lLabel = LABEL + "X";
		ImGui::DragFloat(lLabel.c_str(), &VEC_3->x);
		lLabel = LABEL + "Y";
		ImGui::DragFloat(lLabel.c_str(), &VEC_3->y);
		lLabel = LABEL + "Z";
		ImGui::DragFloat(lLabel.c_str(), &VEC_3->z);
	};

	inline void InputVec3(const std::string &LABEL, KazMath::Vec3<int> *VEC_3)
	{
		static std::string lLabel = "";
		lLabel = LABEL + "X";
		ImGui::DragInt(lLabel.c_str(), &VEC_3->x);
		lLabel = LABEL + "Y";
		ImGui::DragInt(lLabel.c_str(), &VEC_3->y);
		lLabel = LABEL + "Z";
		ImGui::DragInt(lLabel.c_str(), &VEC_3->z);
	};

	inline void InputVec2(const std::string &LABEL, KazMath::Vec2<float> *VEC_2)
	{
		static std::string lLabel = "";
		lLabel = LABEL + "X";
		ImGui::DragFloat(lLabel.c_str(), &VEC_2->x);
		lLabel = LABEL + "Y";
		ImGui::DragFloat(lLabel.c_str(), &VEC_2->y);
	};

	inline void DrawVec3(const std::string &LABEL, const KazMath::Vec3<float> VEC_3)
	{
		static std::string lLabel = "";
		lLabel = LABEL + "X" + ":" + std::to_string(VEC_3.x);
		ImGui::Text(lLabel.c_str());
		lLabel = LABEL + "Y" + ":" + std::to_string(VEC_3.y);
		ImGui::Text(lLabel.c_str());
		lLabel = LABEL + "Z" + ":" + std::to_string(VEC_3.z);
		ImGui::Text(lLabel.c_str());
	};

	inline void InputXMUINT3(const std::string &LABEL, DirectX::XMUINT3 *VEC_3)
	{
		KazMath::Vec3<int>lVec3;
		lVec3.x = static_cast<UINT>(VEC_3->x);
		lVec3.y = static_cast<UINT>(VEC_3->y);
		lVec3.z = static_cast<UINT>(VEC_3->z);
		
		static std::string lLabel = "";
		lLabel = LABEL + "X";
		ImGui::DragInt(lLabel.c_str(), &lVec3.x);
		lLabel = LABEL + "Y";
		ImGui::DragInt(lLabel.c_str(), &lVec3.y);
		lLabel = LABEL + "Z";
		ImGui::DragInt(lLabel.c_str(), &lVec3.z);

		VEC_3->x = static_cast<UINT>(lVec3.x);
		VEC_3->y = static_cast<UINT>(lVec3.y);
		VEC_3->z = static_cast<UINT>(lVec3.z);
	};

	inline void InputXMFLOAT4(const std::string &LABEL, DirectX::XMFLOAT4 *VEC_4)
	{
		static std::string label = "";
		label = LABEL + "X";
		ImGui::DragFloat(label.c_str(), &VEC_4->x);
		label = LABEL + "Y";
		ImGui::DragFloat(label.c_str(), &VEC_4->y);
		label = LABEL + "Z";
		ImGui::DragFloat(label.c_str(), &VEC_4->z);
		label = LABEL + "W";
		ImGui::DragFloat(label.c_str(), &VEC_4->w);
	};

};
