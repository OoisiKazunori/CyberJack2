#pragma once
#include <d3d12.h>
#include "../dxcapi.h"
#include <string>
#include <wrl.h>
#include <vector>

namespace Raytracing {

	//シェーダーデータを保存するためのデータクラス
	class RayShaderData {

	private:

		/*-- メンバ変数 --*/

		Microsoft::WRL::ComPtr<ID3DBlob> m_shaderBlob;	//シェーダーのデータ
		Microsoft::WRL::ComPtr<IDxcBlob> m_shaderBlobDxc;//シェーダーのデータ dxc用
		std::vector<char> m_shaderBin;					//dxcでコンパイルした際に出力される謎の文字列
		std::string m_entryPoint;						//シェーダーのエントリーポイント
		std::string m_shaderModel;						//シェーダーモデル
		std::string m_shaderPath;						//シェーダーのファイルパス これを使ってシェーダーを判断する。


	public:

		/*-- メンバ関数 --*/

		//コンストラクタ
		RayShaderData(const std::string& arg_shaderPath, const std::string& arg_entryPoint, const std::string& arg_shaderModel, bool arg_isDXC = false);

		//ロード処理
		void LoadShader();
		void LoadShaderDXC();

		//シェーダーのファイルパスを取得する処理
		const std::string& GetShaderPath() { return m_shaderPath; }
		//シェーダーのデータを取得する処理
		const Microsoft::WRL::ComPtr<ID3DBlob>& GetShaderBlob() { return m_shaderBlob; }
		const Microsoft::WRL::ComPtr<IDxcBlob>& GetShaderBlobDXC() { return m_shaderBlobDxc; }
		std::vector<char>& GetShaderBin() { return m_shaderBin; }

	private:

		std::wstring StringToWString(std::string arg_OString);

	};

}