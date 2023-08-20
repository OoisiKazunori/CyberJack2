#pragma once
#include"../DirectXCommon/Base.h"
#include"../Helper/ISinglton.h"
#include<vector>
#include"../KazLibrary/Helper/HandleMaker.h"

using namespace std;

enum RootsignatureType
{
	ROOTSIGNATURE_NONE,
	ROOTSIGNATURE_GRAPHICS,
	ROOTSIGNATURE_COMPUTE,
};

/// <summary>
/// ルートシグネチャーの種類
/// </summary>
enum RootSignatureMode
{
	ROOTSIGNATURE_DATA_DRAW,
	ROOTSIGNATURE_DATA_DRAW_UAV,
	ROOTSIGNATURE_DATA_UAV,
	ROOTSIGNATURE_DATA_DRAW_TEX,
	ROOTSIGNATURE_DATA_DRAW_TEX_DATA1,
	ROOTSIGNATURE_DATA_DRAW_TEX_TEX2,
	ROOTSIGNATURE_DATA_DRAW_DATA1_TEX_TEX2,
	ROOTSIGNATURE_DATA_DRAW_UAV_DATA2_TEX_TEX2,
	ROOTSIGNATURE_DATA_DRAW_UAV_DATA_TEX,
	ROOTSIGNATURE_DATA_UAV_UAV_UAV_CBV,
	ROOTSIGNATURE_DATA_UAV_UAV_UAV_CBV_ALLVIEW,
	ROOTSIGNATURE_DATA_UAV_CBV,
	ROOTSIGNATURE_DATA_UAV_UAV_CBV,
	ROOTSIGNATURE_DATA_UAVDESC_UAVVIEW_CBV,
	ROOTSIGNATURE_DATA_UAVVIEW_UAVDESC_CBV,
	ROOTSIGNATURE_DATA_UAV_UAV_CBV_CBV,
	ROOTSIGNATURE_DATA_UAV_UAV_UAV_CBV_CBV,
	ROOTSIGNATURE_DATA_UAV_UAV_UAV_UAV_CBV,
	ROOTSIGNATURE_DATA_UAV_UAV_CBV_SRV,
	ROOTSIGNATURE_DATA_CBV_CBV_CBV_SRV_SRV_SRV,
	ROOTSIGNATURE_DATA_DRAW_UAB_CB,
	ROOTSIGNATURE_DATA_UAV_CB,
	ROOTSIGNATURE_DATA_UAV_UAV,
	ROOTSIGNATURE_DATA_UAB_UAB_UAB_CB,
	ROOTSIGNATURE_DATA_DRAW_UAB_TEX,
	ROOTSIGNATURE_DATA_DRAW_TEX_DATA1_DATA2,
	ROOTSIGNATURE_DATA_DRAW_TEX_TEX2_DATA1_DATA2,
	ROOTSIGNATURE_MODE_MULTIPASS,
	ROOTSIGNATURE_MODE_LIGHT,
	ROOTSIGNATURE_MODE_LIGHT_BORDER,
	ROOTSIGNATURE_DATA_DRAW_TEX4,
	ROOTSIGNATURE_DATA_DRAW_TEX_SKINING,
	ROOTSIGNATURE_DATA_DRAW_TEX_SKINING_DATA3,
	ROOTSIGNATURE_DATA_DRAW_DATA1,
	ROOTSIGNATURE_DATA_DRAW_TEX_SKINING_DATA1,
	ROOTSIGNATURE_DATA_DRAW_DATA1_TEX_DATA2,
	ROOTSIGNATURE_MODE_LINE,
	ROOTSIGNATURE_DATA_DATA1_TEX,
	ROOTSIGNATURE_DATA_DATA1_DATA2_DATA3_TEX,
	ROOTSIGNATURE_DATA_DATA1,
	ROOTSIGNATURE_DATA_DATA1_DATA2,
	ROOTSIGNATURE_DATA_DRAW_DATA1_DATA2,
	ROOTSIGNATURE_DATA_DRAW_SKINING_DATA1,
	ROOTSIGNATURE_DATA_SRV_UAV,
	ROOTSIGNATURE_DATA_CBV,
};


/// <summary>
/// SetSRVかSetCBVの引数に値を渡した際に
///コマンドリストのSetGraphicsRootConstantBufferView()に値を渡すのか
///SetGraphicsDescriptorTable()に値を渡すのか判断する際に必要
/// </summary>
enum GraphicsRootSignatureType
{
	GRAPHICS_ROOTSIGNATURE_TYPE_NONE = -1,
	GRAPHICS_ROOTSIGNATURE_TYPE_DESCRIPTORTABLE,
	GRAPHICS_ROOTSIGNATURE_TYPE_VIEW
};

/// <summary>
/// どういった種類のデスクリプタレンジなのか判断する際に必要
/// </summary>
enum GraphicsRangeType
{
	GRAPHICS_RANGE_TYPE_NONE = -1,
	GRAPHICS_RANGE_TYPE_SRV_DESC,
	GRAPHICS_RANGE_TYPE_SRV_VIEW,
	GRAPHICS_RANGE_TYPE_UAV_DESC,
	GRAPHICS_RANGE_TYPE_UAV_VIEW,
	GRAPHICS_RANGE_TYPE_CBV_DESC,
	GRAPHICS_RANGE_TYPE_CBV_VIEW,
	GRAPHICS_RANGE_TYPE_SAMPLER
};

/// <summary>
/// そのルートパラムはどういった役割を持っているのか
/// </summary>
enum GraphicsRootParamType
{
	GRAPHICS_PRAMTYPE_NONE = -1,
	//描画系のみ使用
	GRAPHICS_PRAMTYPE_DRAW,
	//テクスチャのみ使用
	GRAPHICS_PRAMTYPE_TEX,
	GRAPHICS_PRAMTYPE_TEX2,
	GRAPHICS_PRAMTYPE_TEX3,
	GRAPHICS_PRAMTYPE_TEX4,
	GRAPHICS_PRAMTYPE_TEX5,
	GRAPHICS_PRAMTYPE_TEX6,
	GRAPHICS_PRAMTYPE_TEX7,
	//マテリアルデータのみ使用
	GRAPHICS_PRAMTYPE_MATERIAL,
	//その他(ライトデータなど使用)
	GRAPHICS_PRAMTYPE_DATA,
	GRAPHICS_PRAMTYPE_DATA2,
	GRAPHICS_PRAMTYPE_DATA3,
	GRAPHICS_PRAMTYPE_DATA4,
	GRAPHICS_PRAMTYPE_DATA5,
	GRAPHICS_PRAMTYPE_DATA6,
	GRAPHICS_PRAMTYPE_DATA7,
	GRAPHICS_PRAMTYPE_DATA8,
	GRAPHICS_PRAMTYPE_DATA9,
	GRAPHICS_PRAMTYPE_DATA10,
	//スキニング情報のみ使用
	GRAPHICS_PRAMTYPE_SKINING
};

/// <summary>
/// ルートパラメーターの番号とルートパラメーターの役割を指定します
/// </summary>
struct RootParamData
{
	short param;
	GraphicsRootParamType type;
};

struct BufferRootsignature
{
	GraphicsRangeType rangeType;
	GraphicsRootParamType dataType;
	BufferRootsignature(GraphicsRangeType RANGE, GraphicsRootParamType DATA) :rangeType(RANGE), dataType(DATA)
	{

	}
};

struct RootSignatureDataTest
{
	std::vector<BufferRootsignature> rangeArray;
	std::vector<D3D12_STATIC_SAMPLER_DESC> samplerArray;

	RootSignatureDataTest()
	{
		samplerArray.emplace_back();

		samplerArray[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		samplerArray[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		samplerArray[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		samplerArray[0].BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
		samplerArray[0].Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
		samplerArray[0].MaxLOD = D3D12_FLOAT32_MAX;
		samplerArray[0].MinLOD = 0.0f;
		samplerArray[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
		samplerArray[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
		samplerArray[0].MaxAnisotropy = 16;
		samplerArray[0].MipLODBias = 0;
		samplerArray[0].ShaderRegister = 0;
		samplerArray[0].RegisterSpace = 0;
	}
};


/// <summary>
/// SetSRV、もしくはSetCBVにて、 ルートパラムの番号とディスクリプタヒープとして積ませるか、ビューとして積ませるか
/// を判断する際に必要になる構造体です
/// </summary>
struct GraphicsRootSignatureParameter
{
	GraphicsRangeType range[10];			//種類
	RootParamData paramData[10];			//パラメーター
	GraphicsRootSignatureType type[10];		//どのセットするか
	short paramMax;		    				//使用したパラメーターの数

	//初期化
	GraphicsRootSignatureParameter()
	{
		for (int i = 0; i < 10; i++)
		{
			range[i] = GRAPHICS_RANGE_TYPE_NONE;
			paramData[i].param = -1;
			paramData[i].type = GRAPHICS_PRAMTYPE_NONE;
			type[i] = GRAPHICS_ROOTSIGNATURE_TYPE_NONE;
		}
		paramMax = -1;
	}
};


struct RootSignatureParameter
{
	GraphicsRangeType range;			//種類
	RootParamData paramData;			//パラメーター
	GraphicsRootSignatureType type;		//どのセットするか

	//初期化
	RootSignatureParameter()
	{
		range = GRAPHICS_RANGE_TYPE_NONE;
		paramData.param = -1;
		paramData.type = GRAPHICS_PRAMTYPE_NONE;
		type = GRAPHICS_ROOTSIGNATURE_TYPE_NONE;
	}
};



/// <summary>
/// ルートシグネチャを生成する際に必要になるデータです
/// </summary>
struct RootSignatureData
{
	GraphicsRangeType range[10];
	RootParamData paramData[10];
	D3D12_STATIC_SAMPLER_DESC sample;

	RootSignatureData()
	{
		sample.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		sample.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		sample.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		sample.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
		sample.Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
		sample.MaxLOD = D3D12_FLOAT32_MAX;
		sample.MinLOD = 0.0f;
		sample.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
		sample.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

		sample.MaxAnisotropy = 16;
		sample.MipLODBias = 0;
		sample.ShaderRegister = 0;
		sample.RegisterSpace = 0;
	}
};


/// <summary>
/// ルートシグネチャーを生成、管理します
/// </summary>
class GraphicsRootSignature :public ISingleton<GraphicsRootSignature>
{
public:
	GraphicsRootSignature();
	~GraphicsRootSignature();

	/// <summary>
	/// ルートシグネチャを生成します
	/// </summary>
	/// <param name="ROOTSIGNATURE_DATA">ルートシグネチャを生成する際に必要になるデータ</param>
	void CreateRootSignature(RootSignatureMode ROOTSIGNATURE, RootSignatureData ROOTSIGNATURE_DATA, short DATA_MAX);

	/// <summary>
	/// 指定のルートシグネチャをコマンドリストに積みます
	/// </summary>
	/// <param name="ROOTSIGNATURE_MODE">どのルートシグネチャを設定するか</param>
	void SetRootSignature(RootSignatureMode ROOTSIGNATURE_MODE);

	/// <summary>
	/// 指定のルートシグネチャをコマンドリストに積みます
	/// </summary>
	/// <param name="ROOTSIGNATURE_MODE">どのルートシグネチャを設定するか</param>
	void SetComputeRootSignature(RootSignatureMode ROOTSIGNATURE_MODE);


	/// <summary>
	/// 指定のルートシグネチャーを返り値で渡します
	/// </summary>
	/// <param name="ROOT_SIGNATURE">指定のルートシグネチャー</param>
	/// <returns>生成されたルートシグネチャー</returns>
	const Microsoft::WRL::ComPtr<ID3D12RootSignature> GetRootSignature(RootSignatureMode ROOT_SIGNATURE);

	/// <summary>
	/// 指定のルートシグネチャー毎に設定されたルートパラメーターを渡します
	/// </summary>
	/// <param name="ROOTSIGNATURE_MODE">指定のルートシグネチャー</param>
	/// <returns>ルートパラメーター</returns>
	const GraphicsRootSignatureParameter &GetRootParam(RootSignatureMode ROOTSIGNATURE_MODE);

	const std::vector<RootSignatureParameter> &GetRootParam(int ROOTSIGNATURE_HANDLE);


	Microsoft::WRL::ComPtr<ID3D12RootSignature> CreateRootSignature(const RootSignatureDataTest &ROOTSIGNATURE_DATA, RootsignatureType TYPE);




	struct ParamData
	{
		UINT cbv, srv, uav;
		ParamData() :cbv(0), srv(0), uav(0), prevCbv(cbv), prevSrv(srv), prevUav(uav)
		{
		};

		UINT DirtyNum()
		{
			if (CheckNum(cbv, &prevCbv))
			{
				return cbv;
			}
			if (CheckNum(srv, &prevSrv))
			{
				return srv;
			}
			if (CheckNum(uav, &prevUav))
			{
				return uav;
			}

			//値が変わらないと言う事は何処かが被っている為、エラーを吐かせる
			assert(0);
			return 0;
		}

		UINT TotalNum()
		{
			return cbv + srv + uav;
		}

	private:
		UINT prevCbv, prevSrv, prevUav;
		bool CheckNum(UINT NUM, UINT *PREV_NUM)
		{
			bool dirtyFlag = NUM != *PREV_NUM;
			*PREV_NUM = NUM;
			return dirtyFlag;
		}
	};
private:

	std::vector<Microsoft::WRL::ComPtr<ID3D12RootSignature>> rootSignature;
	std::vector<GraphicsRootSignatureParameter> paramD;

	std::vector<std::vector<RootSignatureParameter>> computeParamArrayData;

	friend ISingleton<GraphicsRootSignature>;


	void CreateMyRootSignature(D3D12_STATIC_SAMPLER_DESC SAMPLER_DATA, D3D12_ROOT_PARAMETER *ROOT_PARAM_DATA, size_t DATA_MAX, RootSignatureMode ROOTSIGNATURE);
	void CreateMyRootSignature(D3D12_STATIC_SAMPLER_DESC SAMPLER_DATA, D3D12_ROOT_PARAMETER1 *ROOT_PARAM_DATA, size_t DATA_MAX, RootSignatureMode ROOTSIGNATURE);

	void CreateMyRootSignature(std::vector<D3D12_STATIC_SAMPLER_DESC> SAMPLER_DATA, D3D12_ROOT_PARAMETER *ROOT_PARAM_DATA, size_t DATA_MAX, RootSignatureMode ROOTSIGNATURE);

	RootSignatureMode CheckEnum(RootSignatureMode MODE)
	{
		RootSignatureMode tmp = MODE;
		return tmp;
	};

};
