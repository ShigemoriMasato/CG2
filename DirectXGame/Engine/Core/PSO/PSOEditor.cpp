#include "PSOEditor.h"

PSOEditor::PSOEditor(ID3D12Device* device) {
	psoManager_ = std::make_unique<PSOManager>(device);
}

void PSOEditor::Initialize() {
	psoManager_->Initialize();
}

void PSOEditor::Setting(ID3D12GraphicsCommandList* commandList) {
	//以前の設定と同じPSOなら何もしない
	if (nowConfig_ == nextConfig_) {
		nextConfig_ = {};
		return;
	}

	if (nextConfig_.rootID != nowConfig_.rootID) {
		commandList->SetGraphicsRootSignature(psoManager_->GetRootSignature(nextConfig_.rootID));
	}
	if (nextConfig_.topology != nowConfig_.topology) {
		commandList->IASetPrimitiveTopology(nextConfig_.topology);
	}

	commandList->SetPipelineState(psoManager_->GetPSO(nextConfig_));

	nowConfig_ = nextConfig_;
	nextConfig_ = {};
}

void PSOEditor::FrameInitialize(ID3D12GraphicsCommandList* commandList) {
	nowConfig_ = {};
	commandList->SetGraphicsRootSignature(psoManager_->GetRootSignature(nowConfig_.rootID));
	commandList->SetPipelineState(psoManager_->GetPSO(nowConfig_));
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}
