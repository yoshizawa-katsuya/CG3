#pragma once
#include <Windows.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <dxcapi.h>
#include <wrl.h>
#include <string>
#include <cstdlib>
#include <array>
#include <memory>

//ブレンドモード
enum class BlendMode {
	kBlendModeNone, //ブレンドなし
	kBlendModeNormal, //NormalBlend
	kBlendModeAdd,	//加算
	kBlendModeSubtract,	//減算
	kBlendModeMultiply,	//乗算
	kBlendModeScreen,	//スクリーン
	kBlendModeNoneGeometry,	//ジオメトリ

	kCountOfBlendMode,	//利用してはいけない
};

class PrimitiveDrawer
{
public:

	

	//パイプラインセット
	struct PipelineSet
	{
		Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature;
		Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineState;
	};

	void Initialize(ID3D12Device* device);

	//パイプライン生成
	std::unique_ptr<PipelineSet> CreateGraphicsPipeline(BlendMode blendMode, ID3D12Device* device);

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

	void SetPipelineSet(ID3D12GraphicsCommandList* commandList, BlendMode blendMode);

	//ID3D12RootSignature* GetRootSignature() { return rootSignature_.Get(); }

	//ID3D12PipelineState* GetGrahicsPipelineState() { return graphicsPipelineState_.Get(); }

private:

	
	//Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_;

	//パイプライン。ブレンドモードの数だけ用意する
	std::array<std::unique_ptr<PipelineSet>, (uint16_t)BlendMode::kCountOfBlendMode> pipelineSets_;
	//Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineState_;

};

