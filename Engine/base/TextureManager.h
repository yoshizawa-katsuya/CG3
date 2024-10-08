#pragma once

#include <array>
#include <d3d12.h>
#include <string>
#include <wrl.h>
#include "DirectXTex/DirectXTex.h"

class TextureManager
{
public:

	//初期化
	void Initialize(ID3D12Device* device);

	//全テクスチャリセット
	void ResetAll();

	//読み込み
	uint32_t Load(const std::string& fileName);

	
	// デスクリプタテーブルをセット
	void SetGraphicsRootDescriptorTable(ID3D12GraphicsCommandList* commandList, UINT rootParamIndex, uint32_t textureHandle);


	//DiscriptorHeap作成の関数
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> CreateDescriptorHeap(
		Microsoft::WRL::ComPtr<ID3D12Device> device, D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors, bool shaderVisiblr);

	//Textureデータを読む
	DirectX::ScratchImage LoadTexture(const std::string& filePath);

	//TextureResourceを作る
	Microsoft::WRL::ComPtr<ID3D12Resource> CreateTextureResource(Microsoft::WRL::ComPtr<ID3D12Device> device, const DirectX::TexMetadata& metadata);

	//TextureResourceにデータを転送する
	void UploadTextureData(Microsoft::WRL::ComPtr<ID3D12Resource> texture, const DirectX::ScratchImage& mipImages);

	//DescriptorHandleを取得する関数。CPU
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap, uint32_t descriptorSize, uint32_t index);

	//DescriptorHandleを取得する関数。GPU
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap, uint32_t descriptorSize, uint32_t index);

	std::wstring ConvertString(const std::string& str);

	std::string ConvertString(const std::wstring& str);

	ID3D12DescriptorHeap* GetSrvDescriptorHeap() const { return srvDescriptorHeap_.Get(); }

	/// <summary>
	/// テクスチャ
	/// </summary>
	struct Texture {
		// テクスチャリソース
		Microsoft::WRL::ComPtr<ID3D12Resource> resource;
		// シェーダリソースビューのハンドル(CPU)
		D3D12_CPU_DESCRIPTOR_HANDLE cpuDescHandleSRV;
		// シェーダリソースビューのハンドル(CPU)
		D3D12_GPU_DESCRIPTOR_HANDLE gpuDescHandleSRV;
	};

private:

	//デバイス
	Microsoft::WRL::ComPtr<ID3D12Device> device_;

	uint32_t descriptorSizeSRV_;

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> srvDescriptorHeap_;

	uint32_t index_;

	// デスクリプターの数
	const UINT kNumDescriptors_ = 128;

	// テクスチャコンテナ
	std::array<Texture, 128> textures_;

};