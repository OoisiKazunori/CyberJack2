#pragma once
#include"../DirectXCommon/Base.h"
#include"../Pipeline/GraphicsPipeLineMgr.h"

/// <summary>
/// パイプラインの生成と登録を行います
/// regionで仕分けしています
/// </summary>
class PreCreateBasePipeLine
{
public:
	PreCreateBasePipeLine();


private:

	D3D12_STATIC_SAMPLER_DESC SetDefaultSampler()
	{
		D3D12_STATIC_SAMPLER_DESC sampler;
		sampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		sampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		sampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		sampler.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
		sampler.Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
		sampler.MaxLOD = D3D12_FLOAT32_MAX;
		sampler.MinLOD = 0.0f;
		sampler.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
		sampler.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
		sampler.MaxAnisotropy = 16;
		sampler.MipLODBias = 0;
		sampler.ShaderRegister = 0;
		sampler.RegisterSpace = 0;
		return sampler;
	};

};

