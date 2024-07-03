#pragma once
#include "Struct.h"
#include "TextureManager.h"
#include <Windows.h>
#include <d3d12.h>
#include <wrl.h>
#include <random>
#include <numbers>

class Model
{
public:

	Model(ID3D12Device* device, Transforms* camera, TextureManager* textureManager, const int32_t kClientWidth, const int32_t kClientHeight);

	void CreateFromOBJ(const std::string& directoryPath, const std::string& filename);

	void CreateCircle();

	void CreateParticle(std::mt19937& randomEngine);

	Particle MakeNewParticle(std::mt19937& randomEngine, const Vector3& translate);

	std::list<Particle> Emit(const Emitter& emitter, std::mt19937& randomEngine);

	void UpdateEmit(std::mt19937& randomEngine);

	void Draw(ID3D12GraphicsCommandList* commandList);

	void DrawParticle(ID3D12GraphicsCommandList* commandList);

	//objファイルの読み込み
	ModelData LoadObjFile(const std::string& directoryPath, const std::string& filename);

	//mtlファイルの読み込み
	MaterialData LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename);

	//Resource作成の関数化
	Microsoft::WRL::ComPtr<ID3D12Resource> CreateBufferResource(Microsoft::WRL::ComPtr<ID3D12Device> device, size_t sizeInBytes);

	Material& GetMaterialDataAddress() { return *materialData_; }

	Transforms& GetTransformAddress() { return transform_; }

	bool& GetUseBillboardAddress() { return useBillboard_; }

	bool& GetUseAccelerationField() { return useAccelerationField_; }

	std::list<Particle>& GetParticles() { return particles_; }

	Emitter& GetEmitter() { return emitter_; }

	void SetRotate(const Vector3& rotate) { transform_.rotate = rotate; }

private:

	void Create();

	const float kDeltaTime_ = 1.0f / 60.0f;

	int32_t kClientWidth_;
	int32_t kClientHeight_;

	//デバイス
	Microsoft::WRL::ComPtr<ID3D12Device> device_;

	ModelData modelData_;

	//VertexResourceを生成
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource_;
	//頂点バッファビューを作成する
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};

	//マテリアル用のリソースを作る
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource_;
	//マテリアルにデータを書き込む
	Material* materialData_ = nullptr;

	//TransformationMatrix用のリソースを作る
	Microsoft::WRL::ComPtr<ID3D12Resource> transformationMatrixResource_;
	//データを書き込む
	TransformationMatrix* transformationMatrixData_ = nullptr;

	//Inatance用のTransformationMatrixリソースを作る
	Microsoft::WRL::ComPtr<ID3D12Resource> instancingResouce_;
	ParticleForGPU* instancingData_ = nullptr;

	const uint32_t kNumMaxInstance_ = 100;

	std::list<Particle> particles_;
	Emitter emitter_{};

	AccelerationField accelerationField_;
	bool useAccelerationField_ = false;

	bool useBillboard_ = true;

	D3D12_CPU_DESCRIPTOR_HANDLE instancingSrvHandleCPU_;
	D3D12_GPU_DESCRIPTOR_HANDLE instancingSrvHandleGPU_;

	//Transform変数を作る
	Transforms transform_{ {1.0f, 1.0f, 1.0f}, { 0.0f, 0.0f, 0.0f}, { 0.0f, 0.0f, 0.0f } };

	//カメラの変数
	Transforms* cameratransform_;

	TextureManager* textureManager_;
	uint32_t textureHandle_;

};

