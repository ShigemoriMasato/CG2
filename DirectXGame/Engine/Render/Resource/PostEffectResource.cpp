#include "PostEffectResource.h"
#include <Core/DXCommonFunction.h>

PostEffectResource::PostEffectResource() {
}

void PostEffectResource::Initialize() {
	auto windowSize = dxDevice_->GetWindowSize();

	MakeVertex(vertex_, 3);

	//左上
	vertex_[0].position = { -1.0f, 1.0f, 1.0f, 1.0f };
	vertex_[0].texcoord = { 0.0f, 0.0f };
	vertex_[0].normal = { 0.0f, 0.0f, -1.0f };

	//右上
	vertex_[1].position = { 3.0f, 1.0f, 1.0f, 1.0f };
	vertex_[1].texcoord = { 2.0f, 0.0f };
	vertex_[1].normal = { 0.0f, 0.0f, -1.0f };

	//左下
	vertex_[2].position = { -1.0f, -3.0f, 1.0f, 1.0f };
	vertex_[2].texcoord = { 0.0f, 2.0f };
	vertex_[2].normal = { 0.0f, 0.0f, -1.0f };

	MakeCBV(blurData_);
	MakeCBV(grayscaleData_);
	MakeCBV(fadeData_);
	MakeCBV(gridTransitionData_);
	MakeCBV(slowMotionData_);
	MakeCBV(glitchData_);
	MakeCBV(constantScanlineData_);
}

void PostEffectResource::SetJobs(PostEffectJob jobs) {
	this->jobs_ = static_cast<uint32_t>(jobs);
}

void PostEffectResource::SetJobs(uint32_t jobs) {
	this->jobs_ = jobs;
}

void PostEffectResource::DrawReady() {
	task_ = jobs_;

	if (!task_) {
		psoConfig_.ps = "Simple.PS.hlsl";
		return;
	}

	std::memcpy(blurData_, &blur_, sizeof(Blur));
	std::memcpy(grayscaleData_, &grayscale_, sizeof(Grayscale));
	std::memcpy(fadeData_, &fade_, sizeof(Fade));
	std::memcpy(gridTransitionData_, &gridTransition_, sizeof(GridTransition));
	std::memcpy(slowMotionData_, &slowMotion_, sizeof(SlowMotion));
	std::memcpy(glitchData_, &glitch_, sizeof(Glitch));
	std::memcpy(constantScanlineData_, &constantScanline_, sizeof(ConstantScanline));

	psoConfig_.ps = shaderBasePath_ + psoConfig_.ps;
	psoConfig_.vs = shaderBasePath_ + "PostEffect.VS.hlsl";
	psoConfig_.depthStencilID = DepthStencilID::Transparent;
	psoConfig_.rootID = RootSignatureID::PixC1;
}

void PostEffectResource::SwapPS() {
	if (task_ & PostEffectJob::Blur) {

		psoConfig_.ps = shaderBasePath_ + "Blur.PS.hlsl";
		task_ &= ~PostEffectJob::Blur;

	} else if (task_ & PostEffectJob::Grayscale) {

		psoConfig_.ps = shaderBasePath_ + "GrayScale.PS.hlsl";
		task_ &= ~PostEffectJob::Grayscale;

	} else if (task_ & PostEffectJob::Fade) {

		psoConfig_.ps = shaderBasePath_ + "Fade.PS.hlsl";
		task_ &= ~PostEffectJob::Fade;

	} else if (task_ & PostEffectJob::GridTransition) {

		psoConfig_.ps = shaderBasePath_ + "GridTransition.PS.hlsl";
		task_ &= ~PostEffectJob::GridTransition;

	} else if (task_ & PostEffectJob::SlowMotion) {

		psoConfig_.ps = shaderBasePath_ + "SlowMotion.PS.hlsl";
		task_ &= ~PostEffectJob::SlowMotion;

	} else if (task_ & PostEffectJob::Glitch) {

		psoConfig_.ps = shaderBasePath_ + "Glitch.PS.hlsl";
		task_ &= ~PostEffectJob::Glitch;

	} else if (task_ & PostEffectJob::ConstantScanline) {

		psoConfig_.ps = shaderBasePath_ + "ConstantScanline.PS.hlsl";
		task_ &= ~PostEffectJob::ConstantScanline;

	}
}

void PostEffectResource::SimpleDrawReady() {
	psoConfig_.ps = shaderBasePath_ + "Simple.PS.hlsl";
	psoConfig_.vs = shaderBasePath_ + "PostEffect.VS.hlsl";
	psoConfig_.depthStencilID = DepthStencilID::Transparent;
	psoConfig_.rootID = RootSignatureID::PixC1;
}

bool PostEffectResource::IsContinue() const {
	return task_;
}

uint32_t operator|(PostEffectJob a, PostEffectJob b) {
	return static_cast<uint32_t>(a) | static_cast<uint32_t>(b);
}

uint32_t operator|(uint32_t a, PostEffectJob b) {
	return a | static_cast<uint32_t>(b);
}

uint32_t operator&(uint32_t a, PostEffectJob b) {
	return a & static_cast<uint32_t>(b);
}

uint32_t operator~(PostEffectJob a) {
	return ~static_cast<uint32_t>(a);
}
