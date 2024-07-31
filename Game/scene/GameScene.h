#pragma once
#include <cstdlib>
#include <memory>
#include "Struct.h"
#include "DirectXCommon.h"
#include "TextureManager.h"
#include "PrimitiveDrawer.h"
#include "Model.h"
#include "Sprite.h"
#include "Player.h"

class GameScene
{
public:

	~GameScene();

	void Initialize(ID3D12Device* device, TextureManager* textureManager, const int32_t kClientWidth, const int32_t kClientHeight);

	void Update();

	void Draw(ID3D12GraphicsCommandList* commandList, PrimitiveDrawer* primitiveDrawer);

private:

	int32_t kClientWidth_;
	int32_t kClientHeight_;

	//デバイス
	Microsoft::WRL::ComPtr<ID3D12Device> device_;

	TextureManager* textureManager_;

	int blendMode = static_cast<int>(BlendMode::kBlendModeNoneGeometry);

	//カメラの変数
	Microsoft::WRL::ComPtr<ID3D12Resource> cameraResource_;
	CameraForGPU* cameraForGPUData_;
	Transforms cameratransform_;

	uint32_t textureHandle1;

	//平行光源用のResourceを作成
	Microsoft::WRL::ComPtr<ID3D12Resource> directionalLightResource_;
	//データを書き込む
	DirectionalLight* directionalLightData_;

	//点光源
	Microsoft::WRL::ComPtr<ID3D12Resource> pointLightResource_;
	//データを書き込む
	PointLight* pointLightData_;

	//スポットライト
	Microsoft::WRL::ComPtr<ID3D12Resource> spotLightResource_;
	//データを書き込む
	SpotLight* spotLightData_;


	std::unique_ptr<Model> model_;
	std::unique_ptr<Model> modelTerrain_;

	std::unique_ptr<Sprite> sprite_;

	//プレイヤー
	std::unique_ptr<Player> player_;

};

