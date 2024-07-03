#include "Player.h"
#include "imgui/imgui.h"

void Player::Initialize(Model* model) {

	model_ = model;

	model->SetRotate({ 0.0f, 3.15f, 0.0f });

}

void Player::Update(std::mt19937& randomEngine) {

	
	/*
	ImGui::Begin("Player");
	if (ImGui::TreeNode("obj")) {
		ImGui::ColorEdit4("color", &model_->GetMaterialDataAddress().color.x);
		ImGui::DragFloat3("translate", &model_->GetTransformAddress().translate.x, 0.01f);
		ImGui::DragFloat3("rotate", &model_->GetTransformAddress().rotate.x, 0.01f);
		ImGui::DragFloat3("scale", &model_->GetTransformAddress().scale.x, 0.01f);

		ImGui::TreePop();
	}
	ImGui::End();
	*/

	model_->UpdateEmit(randomEngine);

	ImGui::Begin("Player");
	ImGui::Checkbox("useBillboard", &model_->GetUseBillboardAddress());
	ImGui::DragFloat3("EmitterTranslate", &model_->GetEmitter().transform.translate.x, 0.01f, -100.0f, 100.0f);
	if (ImGui::Button("Add Particle")) {
		model_->GetParticles().splice(model_->GetParticles().end(), model_->Emit(model_->GetEmitter(), randomEngine));
	}
	
	ImGui::End();
}

void Player::Draw(ID3D12GraphicsCommandList* commandList) {

	model_->DrawParticle(commandList);

}