#include "Model.h"
#include "Matrix.h"
#include "Vector.h"
#include "dx12.h"
#include <fstream>
#include <sstream>
#include <cassert>

Model::Model(ID3D12Device* device, Transforms* camera, TextureManager* textureManager, const int32_t kClientWidth, const int32_t kClientHeight) {

	device_ = device;
	cameratransform_ = camera;
	textureManager_ = textureManager;
	kClientWidth_ = kClientWidth;
	kClientHeight_ = kClientHeight;
	emitter_.count = 3;
	emitter_.frequency = 0.5f;	//0.5秒ごとに発生
	emitter_.frequencyTime = 0.0f;	//発生頻度用の時刻、0で初期化
	emitter_.transform.translate = { 0.0f, 0.0f, 0.0f };
	emitter_.transform.rotate = { 0.0f, 0.0f, 0.0f };
	emitter_.transform.scale = { 1.0f, 1.0f, 1.0f };

}

void Model::CreateFromOBJ(const std::string& directoryPath, const std::string& filename) {

	//モデル読み込み
	modelData_ = LoadObjFile(directoryPath, filename);

	Create();

	textureHandle_ = textureManager_->Load(modelData_.material.textureFilePath);

}

void Model::CreateCircle() {

	modelData_.vertices.push_back({ .position = {1.0f, 1.0f, 0.0f, 1.0f}, .texcoord = {0.0f, 0.0f}, .normal = {0.0f, 0.0f, 1.0f} });	//左上
	modelData_.vertices.push_back({ .position = {-1.0f, 1.0f, 0.0f, 1.0f}, .texcoord = {1.0f, 0.0f}, .normal = {0.0f, 0.0f, 1.0f} });	//右上
	modelData_.vertices.push_back({ .position = {1.0f, -1.0f, 0.0f, 1.0f}, .texcoord = {0.0f, 1.0f}, .normal = {0.0f, 0.0f, 1.0f} });	//左下
	modelData_.vertices.push_back({ .position = {1.0f, -1.0f, 0.0f, 1.0f}, .texcoord = {0.0f, 1.0f}, .normal = {0.0f, 0.0f, 1.0f} });	//左下
	modelData_.vertices.push_back({ .position = {-1.0f, 1.0f, 0.0f, 1.0f}, .texcoord = {1.0f, 0.0f}, .normal = {0.0f, 0.0f, 1.0f} });	//右上
	modelData_.vertices.push_back({ .position = {-1.0f, -1.0f, 0.0f, 1.0f}, .texcoord = {1.0f, 1.0f}, .normal = {0.0f, 0.0f, 1.0f} });	//右下

	Create();

	modelData_.material.textureFilePath = "Game/resources/circle.png";
	textureHandle_ = textureManager_->Load(modelData_.material.textureFilePath);

}

void Model::CreateParticle(std::mt19937& randomEngine) {

	CreateCircle();

	
	//Inatance用のTransformationMatrixリソースを作る
	instancingResouce_ = CreateBufferResource(device_, sizeof(ParticleForGPU) * kNumMaxInstance_);
	//書き込むためのアドレスを取得
	instancingResouce_->Map(0, nullptr, reinterpret_cast<void**>(&instancingData_));

	//単位行列を書き込んでおく
	for (uint32_t index = 0; index < kNumMaxInstance_; ++index) {
		instancingData_[index].WVP = MakeIdentity4x4();
		instancingData_[index].World = MakeIdentity4x4();
		instancingData_[index].color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	}

	D3D12_SHADER_RESOURCE_VIEW_DESC instancingSrvDesc{};
	instancingSrvDesc.Format = DXGI_FORMAT_UNKNOWN;
	instancingSrvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	instancingSrvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	instancingSrvDesc.Buffer.FirstElement = 0;
	instancingSrvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
	instancingSrvDesc.Buffer.NumElements = kNumMaxInstance_;
	instancingSrvDesc.Buffer.StructureByteStride = sizeof(ParticleForGPU);
	instancingSrvHandleCPU_ = GetCPUDescriptorHandle(textureManager_->GetSrvDescriptorHeap(), textureManager_->GetDescriptorSizeSRV(), 3);
	instancingSrvHandleGPU_ = GetGPUDescriptorHandle(textureManager_->GetSrvDescriptorHeap(), textureManager_->GetDescriptorSizeSRV(), 3);
	device_->CreateShaderResourceView(instancingResouce_.Get(), &instancingSrvDesc, instancingSrvHandleCPU_);
	/*
	for (uint32_t index = 0; index < kNumMaxInstance_; ++index) {

		//particles_[index] = MakeNewParticle(randomEngine);
		particles_.push_back(MakeNewParticle(randomEngine));

	}
	*/
}

Particle Model::MakeNewParticle(std::mt19937& randomEngine, const Vector3& translate) {

	std::uniform_real_distribution<float> distribution(-1.0f, 1.0f);

	Particle particle;

	particle.transform.scale = { 1.0f, 1.0f, 1.0f };
	particle.transform.rotate = { 0.0f, 0.0f, 0.0f };
	//particle.transform.translate = { index * 0.1f, index * 0.1f, index * 0.1f };
	Vector3 randomTranslate{ distribution(randomEngine), distribution(randomEngine), distribution(randomEngine) };
	particle.transform.translate = translate + randomTranslate;
	//particle.velocity = { 0.0f, 1.0f, 0.0f };
	particle.velocity = { distribution(randomEngine), distribution(randomEngine), distribution(randomEngine) };

	std::uniform_real_distribution<float> distcolor(0.0f, 1.0f);
	particle.color = { distcolor(randomEngine), distcolor(randomEngine), distcolor(randomEngine), 1.0f };

	std::uniform_real_distribution<float> distTime(1.0f, 3.0f);
	particle.lifeTime = distTime(randomEngine);
	particle.currentTime = 0;

	return particle;

}

std::list<Particle> Model::Emit(const Emitter& emitter_, std::mt19937& randomEngine) {

	std::list<Particle> particles;
	for (uint32_t count = 0; count < emitter_.count; ++count) {
		particles.push_back(MakeNewParticle(randomEngine, emitter_.transform.translate));
	}
	return particles;
}

void Model::UpdateEmit(std::mt19937& randomEngine) {

	emitter_.frequencyTime += kDeltaTime_;	//時刻を進める
	if (emitter_.frequency <= emitter_.frequencyTime) {	//頻度より大きいなら発生
		particles_.splice(particles_.end(), Emit(emitter_, randomEngine));	//発生処理
		emitter_.frequencyTime -= emitter_.frequency;	//余計に過ぎた時間も加味して頻度計算する
	}

}

void Model::Create() {

	//VertexResourceを生成
	vertexResource_ = CreateBufferResource(device_, sizeof(VertexData) * modelData_.vertices.size());

	//リソースの先頭のアドレスから使う
	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	//使用するリソースのサイズは頂点3つ分のサイズ
	vertexBufferView_.SizeInBytes = UINT(sizeof(VertexData) * modelData_.vertices.size());
	//1頂点当たりのサイズ
	vertexBufferView_.StrideInBytes = sizeof(VertexData);

	//頂点リソースにデータを書き込む
	VertexData* vertexData = nullptr;
	//書き込むためのアドレスを取得
	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));
	//頂点データをリソースにコピー
	std::memcpy(vertexData, modelData_.vertices.data(), sizeof(VertexData) * modelData_.vertices.size());

	//マテリアル用のリソースを作る。今回はcolor1つ分のサイズを用意する
	materialResource_ = CreateBufferResource(device_, sizeof(Material));
	//マテリアルにデータを書き込む
	//書き込むためのアドレスを取得
	materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));
	//白を書き込む
	materialData_->color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	materialData_->enableLighting = true;
	materialData_->uvTransform = MakeIdentity4x4();

	//transformationMatrixのリソースを作る
	transformationMatrixResource_ = CreateBufferResource(device_, sizeof(TransformationMatrix));
	//データを書き込む
	//書き込むためのアドレスを取得
	transformationMatrixResource_->Map(0, nullptr, reinterpret_cast<void**>(&transformationMatrixData_));
	//単位行列を書き込んでおく
	transformationMatrixData_->WVP = MakeIdentity4x4();

}

void Model::Draw(ID3D12GraphicsCommandList* commandList) {

	Matrix4x4 worldMatrix = MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.translate);
	Matrix4x4 cameraMatrix = MakeAffineMatrix(cameratransform_->scale, cameratransform_->rotate, cameratransform_->translate);
	Matrix4x4 viewMatrix = Inverse(cameraMatrix);
	Matrix4x4 projectionMatrix = MakePerspectiveFovMatrix(0.45f, float(kClientWidth_) / float(kClientHeight_), 0.1f, 100.0f);
	Matrix4x4 worldViewProjectionMatrix = Multiply(worldMatrix, Multiply(viewMatrix, projectionMatrix));

	transformationMatrixData_->WVP = worldViewProjectionMatrix;
	transformationMatrixData_->World = worldMatrix;

	commandList->IASetVertexBuffers(0, 1, &vertexBufferView_);	//VBVを設定
	//マテリアルのCBufferの場所を設定
	commandList->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());
	//wvp用のCBufferの場所を設定
	commandList->SetGraphicsRootConstantBufferView(1, transformationMatrixResource_->GetGPUVirtualAddress());
	//SRVのDescriptorTableの先頭を設定。2はrootParameter[2]である。
	textureManager_->SetGraphicsRootDescriptorTable(commandList, 2, textureHandle_);

	//描画1(DrawCall/ドローコール)。3頂点で1つのインスタンス。インスタンスについては今後
		//commandList_->DrawIndexedInstanced((kSubdivision * kSubdivision * 6), 1, 0, 0, 0);
	commandList->DrawInstanced(UINT(modelData_.vertices.size()), 1, 0, 0);


}

void Model::DrawParticle(ID3D12GraphicsCommandList* commandList) {


	Matrix4x4 cameraMatrix = MakeAffineMatrix(cameratransform_->scale, cameratransform_->rotate, cameratransform_->translate);
	Matrix4x4 viewMatrix = Inverse(cameraMatrix);
	Matrix4x4 projectionMatrix = MakePerspectiveFovMatrix(0.45f, float(kClientWidth_) / float(kClientHeight_), 0.1f, 100.0f);
	Matrix4x4 backToFrontMatrix = MakeRotateYMatrix(std::numbers::pi_v<float>);
	Matrix4x4 billboardMatrix = Multiply(backToFrontMatrix, cameraMatrix);
	billboardMatrix.m[3][0] = 0.0f;	//平行移動成分はいらない
	billboardMatrix.m[3][1] = 0.0f;
	billboardMatrix.m[3][2] = 0.0f;
	if (!useBillboard_) {
		billboardMatrix = MakeIdentity4x4();
	}
	
	
	
	uint32_t numInstance = 0;

	for (std::list<Particle>::iterator particleIterator = particles_.begin();
		particleIterator != particles_.end();) {
		if (particleIterator->lifeTime <= particleIterator->currentTime) {
			particleIterator = particles_.erase(particleIterator);	//生存時間が過ぎたParticleはlistから消す。戻り値が次のイテレータとなる
			continue;
		}

		particleIterator->transform.translate += particleIterator->velocity * kDeltaTime_;
		particleIterator->currentTime += kDeltaTime_;	//経過時間を足す

		if (numInstance < kNumMaxInstance_) {
			Matrix4x4 scaleMatrix = MakeScaleMatrix(particleIterator->transform.scale);
			Matrix4x4 translateMatrix = MakeTranslateMatrix(particleIterator->transform.translate);
			//Matrix4x4 worldMatrix = MakeAffineMatrix(particles_[index].transform.scale, particles_[index].transform.rotate, particles_[index].transform.translate);
			Matrix4x4 worldMatrix = scaleMatrix * billboardMatrix * translateMatrix;

			Matrix4x4 worldViewProjectionMatrix = Multiply(worldMatrix, Multiply(viewMatrix, projectionMatrix));
			instancingData_[numInstance].WVP = worldViewProjectionMatrix;
			instancingData_[numInstance].World = worldMatrix;
			instancingData_[numInstance].color = particleIterator->color;

			float alpha = 1.0f - (particleIterator->currentTime / particleIterator->lifeTime);
			instancingData_[numInstance].color.w = alpha;

			++numInstance;
		}
		++particleIterator;

	}



	commandList->IASetVertexBuffers(0, 1, &vertexBufferView_);	//VBVを設定
	//マテリアルのCBufferの場所を設定
	commandList->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());
	//wvp用のCBufferの場所を設定

	

	//SRVのDescriptorTableの先頭を設定。2はrootParameter[2]である。
	textureManager_->SetGraphicsRootDescriptorTable(commandList, 2, textureHandle_);

	//instancing用のDataを読むためにStructBufferのSRVを設定する
	commandList->SetGraphicsRootDescriptorTable(1, instancingSrvHandleGPU_);


	//描画!6頂点のポリゴンを、kNumInstance(今回は10)だけInstance描画を行う
	commandList->DrawInstanced(UINT(modelData_.vertices.size()), numInstance, 0, 0);


}

ModelData Model::LoadObjFile(const std::string& directoryPath, const std::string& filename) {

	//1.中で必要となる変数の宣言
	ModelData modelData;	//構築するModelData
	std::vector<Vector4> positions;	//位置
	std::vector<Vector3> normals;	//法線
	std::vector<Vector2> texcords;	//テクスチャ座標
	std::string line;	//ファイルから読んだ1行を格納するもの

	//2.ファイルを開く
	std::ifstream file(directoryPath + "/" + filename);	//ファイルを開く
	assert(file.is_open());	//とりあえず開けなかったら止める

	//3.実際にファイルを読み、ModelDataを構築していく
	while (std::getline(file, line)) {
		std::string identifier;
		std::istringstream s(line);
		s >> identifier;	//先頭の識別子を読む

		if (identifier == "v") {
			Vector4 position;
			s >> position.x >> position.y >> position.z;
			position.x *= -1.0f;
			position.w = 1.0f;
			positions.push_back(position);
		}
		else if (identifier == "vt") {
			Vector2 texcord;
			s >> texcord.x >> texcord.y;
			texcord.y = 1.0f - texcord.y;
			texcords.push_back(texcord);
		}
		else if (identifier == "vn") {
			Vector3 normal;
			s >> normal.x >> normal.y >> normal.z;
			normal.x *= -1.0f;
			normals.push_back(normal);
		}
		else if (identifier == "f") {
			VertexData triangle[3];
			//面は三角形限定。その他は未対応
			for (int32_t faceVertex = 0; faceVertex < 3; ++faceVertex) {
				std::string vertexDefinition;
				s >> vertexDefinition;
				//頂点への要素へのIndexは「位置/UV/法線」で格納されているので、分解してIndexを取得する
				std::istringstream v(vertexDefinition);
				uint32_t elementIndices[3];
				for (int32_t element = 0; element < 3; ++element) {
					std::string index;
					std::getline(v, index, '/');	//区切りでインデックスを読んでいく
					elementIndices[element] = std::stoi(index);
				}
				//要素へのIndexから、実際の要素の値を取得して、頂点を構築する
				Vector4 position = positions[elementIndices[0] - 1];
				Vector2 texcord = texcords[elementIndices[1] - 1];
				Vector3 normal = normals[elementIndices[2] - 1];
				//VertexData vertex = { position, texcord, normal };
				//modelData.vertices.push_back(vertex);
				triangle[faceVertex] = { position, texcord, normal };
			}
			modelData.vertices.push_back(triangle[2]);
			modelData.vertices.push_back(triangle[1]);
			modelData.vertices.push_back(triangle[0]);
		}
		else if (identifier == "mtllib") {
			//materialTemplateLibraryファイルの名前を取得する
			std::string materialFilename;
			s >> materialFilename;
			//基本的にobjファイルと同一階層にmtlは存在させるので、ディレクトリ名とファイル名を渡す
			modelData.material = LoadMaterialTemplateFile(directoryPath, materialFilename);
		}

	}

	//4.ModelDataを返す
	return modelData;
}

MaterialData Model::LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename) {

	//1.中で必要となる変数の宣言
	MaterialData materialData;	//構築するMaterialData
	std::string line;	//ファイルから読んだ1行を格納するもの

	//2.ファイルを開く
	std::ifstream file(directoryPath + "/" + filename);	//ファイルを開く
	assert(file.is_open());	//とりあえず開けなかったら止める

	//3.実際にファイルを読み、ModelDataを構築していく
	while (std::getline(file, line)) {
		std::string identifier;
		std::istringstream s(line);
		s >> identifier;

		//identifierに応じた処理
		if (identifier == "map_Kd") {
			std::string textureFilename;
			s >> textureFilename;
			//連結してファイルパスにする
			materialData.textureFilePath = directoryPath + "/" + textureFilename;
		}
	}

	//4.ModelDataを返す
	return materialData;
}

Microsoft::WRL::ComPtr<ID3D12Resource> Model::CreateBufferResource(Microsoft::WRL::ComPtr<ID3D12Device> device, size_t sizeInBytes)
{

	D3D12_HEAP_PROPERTIES uploadHeapProperties{};
	uploadHeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;
	//頂点リソースの設定
	D3D12_RESOURCE_DESC ResourceDesc{};
	//バッファリソース。テクスチャの場合はまた別の設定をする
	ResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	ResourceDesc.Width = sizeInBytes;
	//バッファの場合はこれらは1にする決まり
	ResourceDesc.Height = 1;
	ResourceDesc.DepthOrArraySize = 1;
	ResourceDesc.MipLevels = 1;
	ResourceDesc.SampleDesc.Count = 1;
	//バッファの場合はこれにする決まり
	ResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	//実際に頂点リソースを作る
	Microsoft::WRL::ComPtr<ID3D12Resource> Resource = nullptr;
	HRESULT hr = device->CreateCommittedResource(&uploadHeapProperties, D3D12_HEAP_FLAG_NONE,
		&ResourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
		IID_PPV_ARGS(&Resource));
	assert(SUCCEEDED(hr));

	return Resource;

	//assert(SUCCEEDED(hr));

}