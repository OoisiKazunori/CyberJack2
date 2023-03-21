#include "Shader.h"
#include"../Helper/KazHelper.h"
#include"../DirectXCommon/DirectX12Device.h"
#include<Windows.h>

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>

Shader::Shader()
{
	shaderBlobs.resize(SHADER_TYPE_MAX);
}

void Shader::Compier(std::string SHADER_FILE, std::string ENTRY_POINT, std::string SHADER_MODEL, ShaderType SHADER_TYPE)
{
	//コンパイルの準備-------------------------
	Microsoft::WRL::ComPtr<IDxcLibrary> library;
	HRESULT hr = DxcCreateInstance(CLSID_DxcLibrary, IID_PPV_ARGS(&library));

	Microsoft::WRL::ComPtr<IDxcCompiler> compiler;
	hr = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&compiler));

	std::array<wchar_t, 128> lShaderFile;
	KazHelper::ConvertStringToWchar_t(SHADER_FILE, lShaderFile.data(), lShaderFile.size());

	uint32_t codePage = CP_UTF8;
	Microsoft::WRL::ComPtr<IDxcBlobEncoding> sourceBlob;
	hr = library->CreateBlobFromFile(lShaderFile.data(), &codePage, &sourceBlob);
	//コンパイルの準備-------------------------



	//コンパイル-------------------------
	std::array<wchar_t, 128> lEntryPoint;
	KazHelper::ConvertStringToWchar_t(ENTRY_POINT, lEntryPoint.data(), lEntryPoint.size());

	std::array<wchar_t, 128> lShaderModel;
	KazHelper::ConvertStringToWchar_t(SHADER_MODEL, lShaderModel.data(), lShaderModel.size());


	//Microsoft::WRL::Microsoft::WRL::ComPtr<IDxcIncludeHandler> includeHandler;
	//hr = library->includeHandler(&includeHandler);
	//DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&));


	Microsoft::WRL::ComPtr<IDxcOperationResult> result;
	hr = compiler->Compile(
		sourceBlob.Get(),		// pSource
		lShaderFile.data(),		// pSourceName
		lEntryPoint.data(),		// pEntryPoint
		lShaderModel.data(),	// pTargetProfile
		nullptr, 0,				// pArguments, argCount
		nullptr, 0,				// pDefines, defineCount
		nullptr,	// pIncludeHandler
		&result);				// ppResult

	if (SUCCEEDED(hr))
	{
		result->GetStatus(&hr);
	}
	//コンパイル-------------------------

	//エラー処理-------------------------
	if (FAILED(hr))
	{
		if (result)
		{
			Microsoft::WRL::ComPtr<IDxcBlobEncoding> errorsBlob;
			hr = result->GetErrorBuffer(&errorsBlob);
			if (SUCCEEDED(hr) && errorsBlob)
			{
				wprintf(L"Compilation failed with errors:\n%hs\n",
					static_cast<const char *>(errorsBlob->GetBufferPointer()));
				//assert(0);
			}
		}
		// Handle compilation error...
	}
	//エラー処理-------------------------

	Microsoft::WRL::ComPtr<IDxcBlob> code;
	result->GetResult(&shaderBlobs[SHADER_TYPE]);


	//D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
	//psoDesc.PS = CD3DX12_SHADER_BYTECODE(code->GetBufferPointer(), code->GetBufferSize());
	//CComPtr<ID3D12PipelineState> pso;
	//hr = DirectX12Device::Instance()->dev->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&pso));

}

void Shader::CompileShader(std::string SHADER_FILE, std::string ENTRY_POINT, std::string SHADER_MODEL, ShaderType SHADER_TYPE)
{
	HRESULT hr;
	std::ifstream infile(SHADER_FILE, std::ifstream::binary);
	if (!infile) {
		throw std::runtime_error("failed shader compile.");
	}

	std::wstring fileName = StringToWString(SHADER_FILE);
	std::stringstream strstream;

	strstream << infile.rdbuf();

	std::string shaderCode = strstream.str();
	Microsoft::WRL::ComPtr<IDxcLibrary> library;
	hr = DxcCreateInstance(CLSID_DxcLibrary, IID_PPV_ARGS(&library));
	Microsoft::WRL::ComPtr<IDxcCompiler> compiler;
	hr = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&compiler));
	Microsoft::WRL::ComPtr<IDxcBlobEncoding> source;


	hr = library->CreateBlobWithEncodingFromPinned(
		(LPBYTE)shaderCode.c_str(), (UINT32)shaderCode.size(), CP_UTF8, &source);
	Microsoft::WRL::ComPtr<IDxcIncludeHandler> includeHandler;
	// インクルードを使う場合には適切に設定すること.
	hr = library->CreateIncludeHandler(&includeHandler);
	// コンパイルオプションの指定.
	std::vector<LPCWSTR> arguments;
	arguments.emplace_back(L"/Od");
	arguments.emplace_back(L"/Zi");
	arguments.emplace_back(L"/WX");


	std::array<wchar_t, 128> lEntryPoint;
	KazHelper::ConvertStringToWchar_t(ENTRY_POINT, lEntryPoint.data(), lEntryPoint.size());


	std::array<wchar_t, 128> lShaderModel;
	KazHelper::ConvertStringToWchar_t(SHADER_MODEL, lShaderModel.data(), lShaderModel.size());


	Microsoft::WRL::ComPtr<IDxcOperationResult> dxcResult;
	hr = compiler->Compile(
		source.Get(),
		fileName.c_str(),
		lEntryPoint.data(),
		lShaderModel.data(),
		arguments.data(),
		static_cast<UINT32>(arguments.size()),
		nullptr,
		0,
		includeHandler.Get(),
		&dxcResult
	);

	if (FAILED(hr)) {
		throw std::runtime_error("failed shader compile.");
	}

	dxcResult->GetStatus(&hr);
	if (FAILED(hr)) {
		Microsoft::WRL::ComPtr<IDxcBlobEncoding> errBlob;
		dxcResult->GetErrorBuffer(&errBlob);

		wprintf(L"Compilation failed with errors:\n%hs\n",
			static_cast<const char *>(errBlob->GetBufferPointer()));
		// ... errBlob の内容をエラーメッセージとして表示 (省略)
		throw std::runtime_error("failed shader compile");
	}
	Microsoft::WRL::ComPtr<IDxcBlob> blob;
	dxcResult->GetResult(&blob);
	shaderBlobs[SHADER_TYPE] = blob;
}

IDxcBlob *Shader::GetShaderData(ShaderType SHADER_TYPE)
{
	return shaderBlobs[SHADER_TYPE].Get();
}

IDxcBlob *Shader::CompileShader(const ShaderOptionData &DATA)
{
	std::ifstream infile(DATA.fileName, std::ifstream::binary);
	if (!infile)
	{
		throw std::runtime_error("failed shader compile.");
	}

	std::wstring fileName = StringToWString(DATA.fileName);
	std::stringstream strstream;

	strstream << infile.rdbuf();

	std::string shaderCode = strstream.str();
	Microsoft::WRL::ComPtr<IDxcLibrary> library;
	CheckGenerate(DxcCreateInstance(CLSID_DxcLibrary, IID_PPV_ARGS(&library)));
	Microsoft::WRL::ComPtr<IDxcCompiler> compiler;
	CheckGenerate(DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&compiler)));
	Microsoft::WRL::ComPtr<IDxcBlobEncoding> source;


	CheckGenerate(library->CreateBlobWithEncodingFromPinned(
		(LPBYTE)shaderCode.c_str(), (UINT32)shaderCode.size(), CP_UTF8, &source));
	Microsoft::WRL::ComPtr<IDxcIncludeHandler> includeHandler;
	// インクルードを使う場合には適切に設定すること.
	CheckGenerate(library->CreateIncludeHandler(&includeHandler));
	// コンパイルオプションの指定.
	std::vector<LPCWSTR> arguments;
	arguments.emplace_back(L"/Od");
	arguments.emplace_back(L"/Zi");


	std::array<wchar_t, 128> lEntryPoint;
	KazHelper::ConvertStringToWchar_t(DATA.entryPoint, lEntryPoint.data(), lEntryPoint.size());


	std::array<wchar_t, 128> lShaderModel;
	KazHelper::ConvertStringToWchar_t(DATA.shaderModel, lShaderModel.data(), lShaderModel.size());


	Microsoft::WRL::ComPtr<IDxcOperationResult> dxcResult;
	CheckGenerate(compiler->Compile(
		source.Get(),
		fileName.c_str(),
		lEntryPoint.data(),
		lShaderModel.data(),
		arguments.data(),
		static_cast<UINT32>(arguments.size()),
		nullptr,
		0,
		includeHandler.Get(),
		&dxcResult
	));

	HRESULT lResult;
	dxcResult->GetStatus(&lResult);
	if (FAILED(lResult))
	{
		Microsoft::WRL::ComPtr<IDxcBlobEncoding> errBlob;
		dxcResult->GetErrorBuffer(&errBlob);

		wprintf(L"Compilation failed with errors:\n%hs\n",
			static_cast<const char *>(errBlob->GetBufferPointer()));
		// ... errBlob の内容をエラーメッセージとして表示 (省略)
		throw std::runtime_error("failed shader compile");
	}
	Microsoft::WRL::ComPtr<IDxcBlob> blob;
	dxcResult->GetResult(&blob);

	shaderBlobs[SHADER_TYPE_COMPUTE] = blob.Get();

	return shaderBlobs[SHADER_TYPE_COMPUTE].Get();
}

void Shader::Error()
{
	std::string errstr;
	errstr.resize(errorBlob->GetBufferSize());

	std::copy_n((char *)errorBlob->GetBufferPointer(),
		errorBlob->GetBufferSize(),
		errstr.begin());

	errstr += "\n";

	FailCheck(errstr.c_str());
	exit(1);
}
