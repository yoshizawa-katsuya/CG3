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
	directionalLightData_->intensity = 0.0f;

	//点光源
	pointLightResource_ = CreateBufferResource(device_, sizeof(PointLight));
	pointLightResource_->Map(0, nullptr, reinterpret_cast<void**>(&pointLightData_));
	pointLightData_->color = { 1.0f, 1.0f, 1.0f, 1.0f };
	pointLightData_->position = { 0.0f, 2.0f, 0.0f };
	pointLightData_->intensity = 1.0f;
	pointLightData_->radius = 3.0f;
	pointLightData_->decay = 1.0f;

	cameratransform_ = { {1.0f, 1.0f, 1.0f}, {0.3f, 0.0f, 0.0f}, {0.0f, 5.0f, -15.0f} };

	cameraResource_ = CreateBufferResource(device_, sizeof(CameraForGPU));
	cameraResource_->Map(0, nullptr, reinterpret_cast<void**>(&cameraForGPUData_));

	cameraForGPUData_->worldPosition = cameratransform_.translate;

	textureHandle1 = textureManager_->Load("Game/resources/uvChecker.png");

	model_ = std::make_unique<Model>(device_.Get(), &cameratransform_, textureManager_, kClientWidth_, kClientHeight_);
	model_->CreateFromOBJ("./Game/resources", "Sphere.obj");

	modelTerrain_ = std::make_unique<Model>(device_.Get(), &cameratransform_, textureManager_, kClientWidth_, kClientHeight_);
	modelTerrain_->CreateFromOBJ("./Game/resources", "terrain.obj");

	sprite_ = std::make_unique<Sprite>(device_.Get(), textureHandle1, Vector2{320.0f, 180.0f}, Vector2{640.0f, 360.0f}, Vector4{1.0f, 1.0f, 1.0f, 1.0f}, kClientWidth_, kClientHeight_);

	//プレイヤーの初期化
	player_ = std::make_unique<Player>();
	player_->Initialize(model_.get());

}

void GameScene::Update() {

	//プレイヤーの更新
	player_->Update();

	ImGui::Begin("Window");
	ImGui::DragFloat3("tranlateSprite", &sprite_->GetTransformAddress().translate.x, 0.01f);
	if (ImGui::TreeNode("camera")) {
		ImGui::DragFloat3("translate", &cameratransform_.translate.x, 0.01f);
		ImGui::DragFloat3("rotate", &cameratransform_.rotate.x, 0.01f);
		ImGui::DragFloat3("scale", &cameratransform_.scale.x, 0.01f);

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

	if (ImGui::TreeNode("PointLight")) {
		ImGui::ColorEdit4("color", &pointLightData_->color.x);
		ImGui::DragFloat3("position", &pointLightData_->position.x, 0.01f);
		ImGui::DragFloat("intensity", &pointLightData_->intensity, 0.01f);
		ImGui::DragFloat("radius", &pointLightData_->radius, 0.01f);
		ImGui::DragFloat("dacay", &pointLightData_->decay, 0.01f);

		ImGui::TreePop();
	}

	ImGui::RadioButton("BlendModeNone", &blendMode, static_cast<int>(BlendMode::kBlendModeNone));
	ImGui::RadioButton("BlendModeNormal", &blendMode, static_cast<int>(BlendMode::kBlendModeNormal));
	ImGui::RadioButton("BlendModeAdd", &blendMode, static_cast<int>(BlendMode::kBlendModeAdd));
	ImGui::RadioButton("BlendModeSubtract", &blendMode, static_cast<int>(BlendMode::kBlendModeSubtract));
	ImGui::RadioButton("BlendModeMultiply", &blendMode, static_cast<int>(BlendMode::kBlendModeMultiply));
	ImGui::RadioButton("BlendModeScreen", &blendMode, static_cast<int>(BlendMode::kBlendModeScreen));

	
	//ImGui::Checkbox("useMonsterBall", &useMonaterBall);
	ImGui::End();

	cameraForGPUData_->worldPosition = cameratransform_.translate;


}

void GameScene::Draw(ID3D12GraphicsCommandList* commandList, PrimitiveDrawer* primitiveDrawer) {

	primitiveDrawer->SetPipelineSet(commandList, static_cast<BlendMode>(blendMode));


	//dxCommon->GetCommandList()->SetGraphicsRootDescriptorTable(2, useMonaterBall ? textureSrvHandleGPU2 : textureSrvHandleGPU);
	//DirectionalRight
	commandList->SetGraphicsRootConstantBufferView(3, directionalLightResource_->GetGPUVirtualAddress());

	commandList->SetGraphicsRootConstantBufferView(5, pointLightResource_->GetGPUVirtualAddress());

	commandList->SetGraphicsRootConstantBufferView(4, cameraResource_->GetGPUVirtualAddress());

	//プレイヤーの描画
	player_->Draw(commandList);

	modelTerrain_->Draw(commandList);

	//modelの描画
	//model_->Draw(commandList);

	//Spriteの描画。変更が必要なものだけ変更する
	//sprite_->Draw(commandList, textureManager_);

}