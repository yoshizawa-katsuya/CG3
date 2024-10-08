#include "GameScene.h"
#include "Vector.h"
#include "dx12.h"
#include "imgui/imgui.h"


GameScene::~GameScene() {



}

void GameScene::Initialize(ID3D12Device* device, TextureManager* textureManager, const int32_t kClientWidth, const int32_t kClientHeight) {

	device_ = device;
	textureManager_ = textureManager;
	kClientWidth_ = kClientWidth;
	kClientHeight_ = kClientHeight;

	//平行光源用のResourceを作成
	directionalLightResource_ = CreateBufferResource(device_, sizeof(DirectionalLight));
	//データを書き込む
	//書き込むためのアドレスを取得
	directionalLightResource_->Map(0, nullptr, reinterpret_cast<void**>(&directionalLightData_));
	//デフォルト値
	directionalLightData_->color = { 1.0f, 1.0f, 1.0f, 1.0f };
	directionalLightData_->direction = { 0.0f, -1.0f, 0.0f };
	directionalLightData_->intensity = 1.0f;


	cameratransform = { {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, -10.0f} };

	textureHandle1 = textureManager_->Load("Game/resources/uvChecker.png");

	model_ = std::make_unique<Model>(device_.Get(), &cameratransform, textureManager_, kClientWidth_, kClientHeight_);
	model_->CreateFromOBJ("./Game/resources", "axis.obj");

	sprite_ = std::make_unique<Sprite>(device_.Get(), textureHandle1, Vector2{320.0f, 180.0f}, Vector2{640.0f, 360.0f}, Vector4{1.0f, 1.0f, 1.0f, 1.0f}, kClientWidth_, kClientHeight_);

	//プレイヤーの初期化
	player_ = std::make_unique<Player>();
	player_->Initialize(model_.get());

}

void GameScene::Update() {

	//プレイヤーの更新
	player_->Update();

	ImGui::Begin("Window");
	ImGui::DragFloat4("color", &model_->GetMaterialDataAddress().color.x, 0.01f);
	ImGui::DragFloat3("tranlateSprite", &sprite_->GetTransformAddress().translate.x, 0.01f);
	if (ImGui::TreeNode("camera")) {
		ImGui::DragFloat3("translate", &cameratransform.translate.x, 0.01f);
		ImGui::DragFloat3("rotate", &cameratransform.rotate.x, 0.01f);
		ImGui::DragFloat3("scale", &cameratransform.scale.x, 0.01f);

		ImGui::TreePop();
	}

	if (ImGui::TreeNode("SpriteUV")) {
		ImGui::DragFloat2("UVTranslate", &sprite_->GetUVTransformAddress().translate.x, 0.01f, -10.0f, 10.0f);
		ImGui::DragFloat2("UVScale", &sprite_->GetUVTransformAddress().scale.x, 0.01f, -10.0f, 10.0f);
		ImGui::SliderAngle("UVRotate", &sprite_->GetUVTransformAddress().rotate.z);

		ImGui::TreePop();
	}

	if (ImGui::TreeNode("directionalLight")) {
		ImGui::DragFloat4("color", &directionalLightData_->color.x, 0.01f);
		ImGui::DragFloat3("direction", &directionalLightData_->direction.x, 0.01f);
		directionalLightData_->direction = Normalize(directionalLightData_->direction);
		ImGui::DragFloat("intensity", &directionalLightData_->intensity, 0.01f);

		ImGui::TreePop();
	}
	//ImGui::Checkbox("useMonsterBall", &useMonaterBall);
	ImGui::End();

}

void GameScene::Draw(ID3D12GraphicsCommandList* commandList) {

	//dxCommon->GetCommandList()->SetGraphicsRootDescriptorTable(2, useMonaterBall ? textureSrvHandleGPU2 : textureSrvHandleGPU);
	//DirectionalRight
	commandList->SetGraphicsRootConstantBufferView(3, directionalLightResource_->GetGPUVirtualAddress());

	//プレイヤーの描画
	player_->Draw(commandList);

	//modelの描画
	//model_->Draw(commandList);

	//Spriteの描画。変更が必要なものだけ変更する
	//sprite_->Draw(commandList, textureManager_);

}