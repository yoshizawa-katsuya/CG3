#include "Player.h"
#include "imgui/imgui.h"

void Player::Initialize(Model* model) {

	model_ = model;

}

void Player::Update() {

	ImGui::Begin("Player");
	if (ImGui::TreeNode("obj")) {
		ImGui::DragFloat3("translate", &model_->GetTransformAddress().translate.x, 0.01f);
		ImGui::DragFloat3("rotate", &model_->GetTransformAddress().rotate.x, 0.01f);
		ImGui::DragFloat3("scale", &model_->GetTransformAddress().scale.x, 0.01f);

		ImGui::TreePop();
	}
	ImGui::End();

}

void Player::Draw(ID3D12GraphicsCommandList* commandList) {

	model_->Draw(commandList);

}