#pragma once
#include <Windows.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <dxcapi.h>
#include <wrl.h>
#include <string>

class PrimitiveDrawer
{
public:

	void Initialize(ID3D12Device* device);

	Microsoft::WRL::ComPtr<IDxcBlob> CompilerShader(
		//CompilerするShaderファイルへのパス
		const std::wstring& filePath,
		//Compilerに使用するProfile
		const wchar_t* profile,
		//初期化で生成したものを3つ
		Microsoft::WRL::ComPtr<IDxcUtils> dxcUtils,
		Microsoft::WRL::ComPtr<IDxcCompiler3> dxcCompiler,
		Microsoft::WRL::ComPtr<IDxcIncludeHandler> includeHandler);

	void Log(const std::string& message);

	std::wstring ConvertString(const std::string& str);

	std::string ConvertString(const std::wstring& str);

	ID3D12RootSignature* GetRootSignature() { return rootSignature_.Get(); }

	ID3D12PipelineState* GetGrahicsPipelineState() { return graphicsPipelineState_.Get(); }

private:

	
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineState_;

};

