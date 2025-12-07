#include "WaterPlaneResource.h"
#include <imgui/imgui.h>

WaterPlaneResource::~WaterPlaneResource() {

	Save();

}

void WaterPlaneResource::Initialize(AssetsID modelID) {
	SetModelData(modelID);
	MakeCBV(uniqueData_);
	MakeCBV(directLight_);

	psoConfig_.vs = "Water/WaterPlane.VS.hlsl";
	psoConfig_.ps = "Water/WaterPlane.PS.hlsl";
	psoConfig_.rootID = RootSignatureID::VerC1_PixC1;

	binaryManager_ = std::make_unique<BinaryManager>();

	//初期値設定
	auto data = binaryManager_->Read("WaterPlaneResource.bin");
	size_t index = 0;
	if (!data.empty()) {
		scale = binaryManager_->Reverse<Vector3>(data[index++]);
		rotation = binaryManager_->Reverse<Vector3>(data[index++]);
		position = binaryManager_->Reverse<Vector3>(data[index++]);
		int waveSize = binaryManager_->Reverse<int>(data[index++]);
		waves_.resize(waveSize);
		for (size_t i = 0; i < waves_.size(); i++) {
			waves_[i].direction.x = binaryManager_->Reverse<float>(data[index++]);
			waves_[i].direction.y = binaryManager_->Reverse<float>(data[index++]);
			waves_[i].amplitude = binaryManager_->Reverse<float>(data[index++]);
			waves_[i].wavelength = binaryManager_->Reverse<float>(data[index++]);
			waves_[i].speed = binaryManager_->Reverse<float>(data[index++]);
		}
		directLight_->color = binaryManager_->Reverse<Vector4>(data[index++]);
		directLight_->direction = binaryManager_->Reverse<Vector3>(data[index++]);
		directLight_->intensity = binaryManager_->Reverse<float>(data[index++]);
	} else {
		waves_.resize(8);
	}
}

void WaterPlaneResource::DrawImGui() {
	ImGui::Begin("WaterPlaneResource");

	time_ += 0.0166666667f; //約60fps

	ImGui::ColorEdit4("Light Color", &directLight_->color.x);
	ImGui::DragFloat3("Light Direction", &directLight_->direction.x, 0.01f);
	directLight_->direction = directLight_->direction.Normalize();
	ImGui::DragFloat("Light Intensity", &directLight_->intensity, 0.1f);

	ImGui::Separator();

	ImGui::DragFloat3("Scale", &scale.x, 0.1f, 0.1f);
	ImGui::DragFloat3("Rotation", &rotation.x, 0.1f);
	ImGui::DragFloat3("Position", &position.x, 0.1f);

	ImGui::Separator();

	static int currentWave = 0;
	ImGui::Text("%d", currentWave);
	ImGui::SameLine();
	if (ImGui::Button("+")) {
		currentWave++;
	}
	ImGui::SameLine();
	if (ImGui::Button("-")) {
		currentWave--;
	}
	currentWave = std::clamp(currentWave, 0, static_cast<int>(waves_.size() - 1));

	ImGui::DragFloat2("Direction", &waves_[currentWave].direction.x, 0.1f, -1.0f, 1.0f);
	waves_[currentWave].direction = waves_[currentWave].direction.Normalize();
	ImGui::DragFloat("Amplitude", &waves_[currentWave].amplitude, 0.01f, 0.0f, 10.0f);
	ImGui::DragFloat("Wavelength", &waves_[currentWave].wavelength, 0.1f, 0.1f, 100.0f);
	ImGui::DragFloat("Speed", &waves_[currentWave].speed, 0.1f, 0.0f, 10.0f);

	ImGui::End();
}

void WaterPlaneResource::DrawReady() {
	Matrix4x4 world = Matrix::MakeAffineMatrix(scale, rotation, position);
	uniqueData_->world = world;
	uniqueData_->vp = camera_->GetVPMatrix();
	uniqueData_->cameraPos = camera_->GetPosition();
	uniqueData_->time = time_;

	int waveCount = 0;
	for(const auto& wave : waves_) {
		if (wave.wavelength > 0.0f) {
			waveCount++;
		}
	}
	uniqueData_->waveCount = waveCount;
	for (size_t i = 0; i < waves_.size(); i++) {
		uniqueData_->waves[i] = waves_[i];
	}
}

void WaterPlaneResource::Save() {
	binaryManager_->RegistOutput(scale);
	binaryManager_->RegistOutput(rotation);
	binaryManager_->RegistOutput(position);
	binaryManager_->RegistOutput(int(waves_.size()));
	for (size_t i = 0; i < waves_.size(); i++) {
		binaryManager_->RegistOutput(waves_[i].direction.x);
		binaryManager_->RegistOutput(waves_[i].direction.y);
		binaryManager_->RegistOutput(waves_[i].amplitude);
		binaryManager_->RegistOutput(waves_[i].wavelength);
		binaryManager_->RegistOutput(waves_[i].speed);
	}
	binaryManager_->RegistOutput(directLight_->color);
	binaryManager_->RegistOutput(directLight_->direction);
	binaryManager_->RegistOutput(directLight_->intensity);
	binaryManager_->Write("WaterPlaneResource.bin");
}
