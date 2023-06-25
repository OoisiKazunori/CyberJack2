#include "RayShaderData.h"
#include <d3dcompiler.h>
#include <system_error>
#include <fstream>
#include <iostream>
#include <sstream>
#include <array>
#include "../KazLibrary/Helper/KazHelper.h"

#pragma comment(lib, "dxcompiler.lib")

namespace Raytracing {

	RayShaderData::RayShaderData(const std::string& arg_shaderPath, const std::string& arg_entryPoint, const std::string& arg_shaderModel, bool arg_isDXC)
	{

		/*-- コンストラクタ --*/

		//引数を保存。
		this->m_shaderPath = arg_shaderPath;
		this->m_entryPoint = arg_entryPoint;
		this->m_shaderModel = arg_shaderModel;
		this->m_shaderBlob = nullptr;

		if (arg_isDXC) {

			//シェーダーをロード。
			LoadShaderDXC();

		}
		else {

			//シェーダーをロード。
			LoadShader();

		}

	}

	void RayShaderData::LoadShader()
	{

		/*-- シェーダーをロードする処理 --*/

		std::array<wchar_t, 128> shaderPathBuff;
		KazHelper::ConvertStringToWchar_t(m_shaderPath, shaderPathBuff.data(), 128);

		Microsoft::WRL::ComPtr<ID3DBlob> errorBlob = nullptr;

		HRESULT result = D3DCompileFromFile(
			shaderPathBuff.data(),								//シェーダファイル名
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE,					//インクルード可能にする
			m_entryPoint.c_str(), m_shaderPath.c_str(),			//エントリーポイント名、シェーダーモデル指定
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,	//デバッグ用設定
			0,
			&m_shaderBlob, &errorBlob);

		//シェーダーのエラーチェック
		if (FAILED(result)) {

			//hresultからエラーメッセージを取得
			std::string errorMsg = std::system_category().message(result);

			//errorBlobからエラー内容をstring型にコピー
			std::string errstr;
			errstr.resize(errorBlob->GetBufferSize());

			std::copy_n((char*)errorBlob->GetBufferPointer(),
				errorBlob->GetBufferSize(),
				errstr.begin());
			errstr += "\n";
			//エラー内容を出力ウィンドウに表示
			OutputDebugStringA(errstr.c_str());
			exit(1);
		}

	}

	void RayShaderData::LoadShaderDXC()
	{
		HRESULT hr;
		std::ifstream infile(m_shaderPath, std::ifstream::binary);
		if (!infile) {
			throw std::runtime_error("failed shader compile.");
		}

		//型変換。
		std::wstring fileName = StringToWString(m_shaderPath);
		std::stringstream strstream;

		strstream << infile.rdbuf();

		std::string shaderCode_ = strstream.str();
		Microsoft::WRL::ComPtr<IDxcLibrary> library;
		DxcCreateInstance(CLSID_DxcLibrary, IID_PPV_ARGS(&library));
		Microsoft::WRL::ComPtr<IDxcCompiler> compiler;
		DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&compiler));
		Microsoft::WRL::ComPtr<IDxcBlobEncoding> source_;

		library->CreateBlobWithEncodingFromPinned(
			(LPBYTE)shaderCode_.c_str(), static_cast<UINT32>(shaderCode_.size()), CP_UTF8, &source_);
		Microsoft::WRL::ComPtr<IDxcIncludeHandler> includeHandler;

		//インクルードを使う場合には適切に設定すること
		library->CreateIncludeHandler(&includeHandler);
		//コンパイルオプションの指定
		std::vector<LPCWSTR> arguments;

		arguments.emplace_back(L"/enable-16bit-types");		//16bit変数を使用可能にする。

		//シェーダーモデルは一旦これで固定する。
		const auto target_ = L"lib_6_4";

		Microsoft::WRL::ComPtr<IDxcOperationResult> dxcResult;
		hr = compiler->Compile(source_.Get(), fileName.c_str(),
			L"", target_, arguments.data(), static_cast<UINT>(arguments.size()),
			nullptr, 0, includeHandler.Get(), &dxcResult);

		if (FAILED(hr)) {
			throw std::runtime_error("failed shader compile.");
		}

		dxcResult->GetStatus(&hr);
		if (FAILED(hr)) {
			Microsoft::WRL::ComPtr<IDxcBlobEncoding> errBlob;
			dxcResult->GetErrorBuffer(&errBlob);

			//GetBufferPointerとGetBufferSizeを使って内容にアクセス
			const char* pData = static_cast<const char*>(errBlob->GetBufferPointer());
			size_t size = errBlob->GetBufferSize();

			//バッファを文字列として読む
			std::string content(pData, pData + size);

			//Visual Studioの出力ウィンドウに出力
			OutputDebugStringA(content.c_str());

			throw std::runtime_error("failed shader compile");
		}

		Microsoft::WRL::ComPtr<IDxcBlob> blob;
		dxcResult->GetResult(&blob);

		m_shaderBlobDxc = blob;

		std::vector<char> result;
		auto size = blob->GetBufferSize();
		result.resize(size);
		memcpy(result.data(), blob->GetBufferPointer(), size);
		m_shaderBin = result;

	}

	std::wstring RayShaderData::StringToWString(std::string OString)
	{
		//SJIS → wstring
		int iBufferSize = MultiByteToWideChar(CP_ACP, 0, OString.c_str()
			, -1, (wchar_t*)NULL, 0);

		//バッファの取得
		wchar_t* cpUCS2 = new wchar_t[iBufferSize];

		//SJIS → wstring
		MultiByteToWideChar(CP_ACP, 0, OString.c_str(), -1, cpUCS2
			, iBufferSize);

		//stringの生成
		std::wstring oRet(cpUCS2, cpUCS2 + iBufferSize - 1);

		//バッファの破棄
		delete[] cpUCS2;

		//変換結果を返す
		return(oRet);
	}

}