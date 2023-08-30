#pragma once
#include <memory>
#include <vector>
#include <d3d12.h>
#include <string>
#include <wrl/client.h>
#include "../Helper/ISinglton.h"
#include "dxcapi.h"

namespace Raytracing {

	// シェーダーデータクラスの前方宣言
	class RayShaderData;

	/// <summary>
	/// シェーダーデータ保存クラス レイトレ用に持ってきました。
	/// </summary>
	class RayShaderStorage : public ISingleton<RayShaderStorage> {

	private:

		/*-- メンバ変数 --*/

		std::vector<std::shared_ptr<RayShaderData>> m_shaderData;


	public:

		/*-- メンバ関数 --*/

		//シェーダーをロードする。
		Microsoft::WRL::ComPtr<ID3DBlob> LoadShader(const std::string& arg_shaderPath, const std::string& arg_shaderModel, const std::string& arg_entryPoint);
		Microsoft::WRL::ComPtr<ID3DBlob> LoadShaderForDXC(const std::string& arg_shaderPath, const std::string& arg_shaderModel, const std::string& arg_entryPoint);

		//シェーダーデータを返す。
		Microsoft::WRL::ComPtr<ID3DBlob> GetShaderData(const std::string& arg_shaderPath);
		Microsoft::WRL::ComPtr<IDxcBlob> GetShaderDataForDXC(const std::string& arg_shaderPath);
		std::vector<char>& GetShaderBin(const std::string& arg_shaderPath);

	};


}