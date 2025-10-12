#include "ShaderEditScene.h"
#include <imgui/imgui.h>
#include <Math/MyString.h>

bool ComboGetter(void* data, int idx, const char** out_text) {
	const auto& vec = *static_cast<std::vector<std::string>*>(data);
	if (idx < 0 || idx >= static_cast<int>(vec.size())) return false;
	*out_text = vec[idx].c_str();
	return true;
}

ShaderEditScene::ShaderEditScene() {
	binaryManager_ = std::make_unique<BinaryManager>();
}

ShaderEditScene::~ShaderEditScene() {
	Save();
}

void ShaderEditScene::Initialize() {
	buffer_.resize(2);

	std::vector<std::string> shaderNames;

	std::vector<std::string> shaderNameBuffer = SerchFilePathsAddChild("Assets/Shader", ".hlsl");
	for (const auto& name : shaderNameBuffer) {
		shaderNames.push_back(name);
	}

	for (const auto& sn : shaderNames) {
		//VertexShaderだったら
		if (sn.find("VS") != std::string::npos) {
			buffer_[1].push_back(sn);
			vss_.push_back(buffer_[1].back().c_str());
		}
		//PixelShaderだったら
		else if (sn.find("PS") != std::string::npos) {
			buffer_[0].push_back(sn);
			pss_.push_back(buffer_[0].back().c_str());
		}
	}

	inputLayouts_.resize(static_cast<int>(InputLayoutID::Count));
	rootSignatures_.resize(static_cast<int>(RootSignatureID::Count));

	inputLayouts_[static_cast<int>(InputLayoutID::Default)] = "Default";
	inputLayouts_[static_cast<int>(InputLayoutID::Model)] = "Model";

	rootSignatures_[static_cast<int>(RootSignatureID::Default)] = "Default";
	rootSignatures_[static_cast<int>(RootSignatureID::NonMatrix)] = "NonMatrix";
	rootSignatures_[static_cast<int>(RootSignatureID::Particle)] = "Particle";
	rootSignatures_[static_cast<int>(RootSignatureID::Model)] = "Model";

	auto rawData = binaryManager_->Read(saveFileName);
	shaderDataIndex_.clear();
	for (int i = 0; i < rawData.size(); i += 4) {

		auto ps = dynamic_cast<Value<std::string>*>(rawData[i].get())->value;
		auto vs = dynamic_cast<Value<std::string>*>(rawData[i + 1].get())->value;
		auto inputLayoutID = static_cast<InputLayoutID>(dynamic_cast<Value<int>*>(rawData[i + 2].get())->value);
		auto rootSignatureID = static_cast<RootSignatureID>(dynamic_cast<Value<int>*>(rawData[i + 3].get())->value);

		shaderDataIndex_.push_back(ShaderDataIndex{});
		shaderDataIndex_.back().psIndex = static_cast<int>(std::distance(buffer_[0].begin(), std::find(buffer_[0].begin(), buffer_[0].end(), ps)));
		shaderDataIndex_.back().vsIndex = static_cast<int>(std::distance(buffer_[1].begin(), std::find(buffer_[1].begin(), buffer_[1].end(), vs)));
		shaderDataIndex_.back().inputLayoutIndex = static_cast<int>(inputLayoutID);
		shaderDataIndex_.back().rootSignatureIndex = static_cast<int>(rootSignatureID);
	}
}

std::unique_ptr<BaseScene> ShaderEditScene::Update() {

	ImGui::Begin("ShaderEdit");

	if (ImGui::Button("Add")) {
		shaderDataIndex_.push_back(ShaderDataIndex{});
	}

	ImGui::SameLine();

	if(ImGui::Button("Delete")) {
		if(!shaderDataIndex_.empty()) {
			shaderDataIndex_.pop_back();
		}
	}

	for (int i = 0; i < shaderDataIndex_.size(); ++i) {
		ImGui::PushID(i);

		ImGui::SetNextItemWidth(200.0f);
		ImGui::Combo("VS", &shaderDataIndex_[i].vsIndex, vss_.data(), static_cast<int>(vss_.size()));
		
		ImGui::SameLine();
		ImGui::SetNextItemWidth(200.0f);
		ImGui::Combo("PS", &shaderDataIndex_[i].psIndex, pss_.data(), static_cast<int>(pss_.size()));
		
		ImGui::SameLine();
		ImGui::SetNextItemWidth(200.0f);
		ImGui::Combo("IL", &shaderDataIndex_[i].inputLayoutIndex, inputLayouts_.data(), static_cast<int>(inputLayouts_.size()));
		
		ImGui::SameLine();
		ImGui::SetNextItemWidth(200.0f);
		ImGui::Combo("RS", &shaderDataIndex_[i].rootSignatureIndex, rootSignatures_.data(), static_cast<int>(rootSignatures_.size()));

		ImGui::PopID();
	}

	ImGui::End();

	return nullptr;
}

void ShaderEditScene::Draw() {
}

void ShaderEditScene::Save() {
	for(int i = 0; i < shaderDataIndex_.size(); ++i) {
		ShaderData data;
		binaryManager_->RegistOutput(buffer_[0][shaderDataIndex_[i].psIndex], "");
		binaryManager_->RegistOutput(buffer_[1][shaderDataIndex_[i].vsIndex], "");
		binaryManager_->RegistOutput(shaderDataIndex_[i].inputLayoutIndex, "");
		binaryManager_->RegistOutput(shaderDataIndex_[i].rootSignatureIndex, "");
	}

	binaryManager_->Write(saveFileName);
}
