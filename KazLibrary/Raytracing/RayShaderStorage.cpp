#include "RayShaderStorage.h"
#include "RayShaderData.h"
#include <assert.h>

#pragma comment(lib, "dxcompiler.lib")

namespace Raytracing {

	Microsoft::WRL::ComPtr<ID3DBlob> RayShaderStorage::LoadShader(const std::string& arg_shaderPath, const std::string& arg_shaderModel, const std::string& arg_entryPoint)
	{

		/*-- シェーダーのロード処理 --*/

		const int SHADER_COUNT = static_cast<int>(m_shaderData.size());

		//シェーダーの数分ループして、ロード済みのシェーダーかをチェックする。
		for (int index = 0; index < SHADER_COUNT; ++index) {

			//シェーダの名前が違っていたら次へ。
			if (m_shaderData[index]->GetShaderPath() != arg_shaderPath) continue;

			//このindexのシェーダーをリターンする。
			return m_shaderData[index]->GetShaderBlob();

		}

		//シェーダーをロードして保存。
		m_shaderData.emplace_back(std::make_unique<RayShaderData>(arg_shaderPath, arg_entryPoint, arg_shaderModel));

		//最後尾のデータをリターンする。
		return m_shaderData[m_shaderData.size() - 1]->GetShaderBlob();
	}

	Microsoft::WRL::ComPtr<ID3DBlob> RayShaderStorage::LoadShaderForDXC(const std::string& arg_shaderPath, const std::string& arg_shaderModel, const std::string& arg_entryPoint)
	{

		/*-- シェーダーのロード処理 --*/

		const int SHADER_COUNT = static_cast<int>(m_shaderData.size());

		//シェーダーの数分ループして、ロード済みのシェーダーかをチェックする。
		for (int index = 0; index < SHADER_COUNT; ++index) {

			//シェーダの名前が違っていたら次へ。
			if (m_shaderData[index]->GetShaderPath() != arg_shaderPath) continue;

			//このindexのシェーダーをリターンする。
			return m_shaderData[index]->GetShaderBlob();

		}

		//シェーダーをロードして保存。
		m_shaderData.emplace_back(std::make_unique<RayShaderData>(arg_shaderPath, arg_entryPoint, arg_shaderModel, true));

		//最後尾のデータをリターンする。
		return m_shaderData[m_shaderData.size() - 1]->GetShaderBlob();

	}

	Microsoft::WRL::ComPtr<ID3DBlob> RayShaderStorage::GetShaderData(const std::string& arg_shaderPath)
	{

		/*-- シェーダーデータを返す処理 --*/

		const int SHADER_COUNT = static_cast<int>(m_shaderData.size());

		//全てのシェーダーデータを検索する。
		for (int index = 0; index < SHADER_COUNT; ++index) {

			//引数の要素が合っているかをチェックする。
			if (!(m_shaderData[index]->GetShaderPath() == arg_shaderPath)) continue;

			return m_shaderData[index]->GetShaderBlob();

		}

		return Microsoft::WRL::ComPtr<ID3DBlob>();
	}

	Microsoft::WRL::ComPtr<IDxcBlob> RayShaderStorage::GetShaderDataForDXC(const std::string& arg_shaderPath)
	{
		/*-- シェーダーデータを返す処理 --*/

		const int SHADER_COUNT = static_cast<int>(m_shaderData.size());

		//全てのシェーダーデータを検索する。
		for (int index = 0; index < SHADER_COUNT; ++index) {

			//引数の要素が合っているかをチェックする。
			if (!(m_shaderData[index]->GetShaderPath() == arg_shaderPath)) continue;

			return m_shaderData[index]->GetShaderBlobDXC();

		}

		return Microsoft::WRL::ComPtr<IDxcBlob>();
	}

	std::vector<char>& RayShaderStorage::GetShaderBin(const std::string& arg_shaderPath)
	{
		/*-- シェーダーデータを返す処理 --*/

		const int SHADER_COUNT = static_cast<int>(m_shaderData.size());

		//全てのシェーダーデータを検索する。
		for (int index = 0; index < SHADER_COUNT; ++index) {

			//引数の要素が合っているかをチェックする。
			if (!(m_shaderData[index]->GetShaderPath() == arg_shaderPath)) continue;

			return m_shaderData[index]->GetShaderBin();

		}

		//シェーダーがロードされていない。
		assert(0);
		return m_shaderData[0]->GetShaderBin();
	}

}