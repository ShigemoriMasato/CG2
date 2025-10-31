#include "PostEffectResource.h"

PostEffectResource::PostEffectResource() {
}

void PostEffectResource::Initialize() {
	auto windowSize = dxDevice_->GetWindowSize();

	vertexNum_ = 3;
	vertexResource.Attach(CreateBufferResource(dxDevice_->GetDevice(), sizeof(VertexData) * vertexNum_));
	vertexResource->Map(0, nullptr, (void**)&vertex_);

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

	vertexBufferView.BufferLocation = vertexResource->GetGPUVirtualAddress();
	vertexBufferView.SizeInBytes = sizeof(VertexData) * vertexNum_;
	vertexBufferView.StrideInBytes = sizeof(VertexData);

	infoResource_.Attach(CreateBufferResource(dxDevice_->GetDevice(), sizeof(InfoForGPU)));
	infoResource_->Map(0, nullptr, (void**)&infoForGPU_);

}

void PostEffectResource::SetJobs(PostEffectJob jobs) {
	this->jobs_ = static_cast<uint32_t>(jobs);
	task_ = jobs_;
}

void PostEffectResource::SetJobs(uint32_t jobs) {
	this->jobs_ = jobs;
	task_ = jobs;
}

void PostEffectResource::DrawReady() {
	if (!task_) {
		psoConfig_.ps = "Simple.PS.hlsl";
		return;
	}

	if (task_ & PostEffectJob::Blur) {

		infoForGPU_->slot1.x = data_.blur.intensity;
		infoForGPU_->slot1.y = data_.blur.kernelSize;

		psoConfig_.ps = data_.blur.shaderFile;
		task_ &= ~PostEffectJob::Blur;

	} else if (task_ & PostEffectJob::Grayscale) {

		infoForGPU_->slot1.z = data_.grayscale.intensity;

		psoConfig_.ps = data_.grayscale.shaderFile;
		task_ &= ~PostEffectJob::Grayscale;

	} else if (task_ & PostEffectJob::Fade) {
		// slot1: alpha, type, -, -
		infoForGPU_->slot1.x = data_.fade.alpha;
		infoForGPU_->slot1.y = static_cast<float>(static_cast<int>(data_.fade.type));
		infoForGPU_->slot1.z = 0.0f;
		infoForGPU_->slot1.w = 0.0f;
		
		// slot2: fadeColor (r, g, b, -)
		infoForGPU_->slot2.x = data_.fade.color.x;
		infoForGPU_->slot2.y = data_.fade.color.y;
		infoForGPU_->slot2.z = data_.fade.color.z;
		infoForGPU_->slot2.w = 0.0f;

		psoConfig_.ps = data_.fade.shaderFile;
		task_ &= ~PostEffectJob::Fade;
	} else if (task_ & PostEffectJob::GridTransition) {
		// slot1: progress, gridSize, fadeColor, pattern
		infoForGPU_->slot1.x = data_.gridTransition.progress;
		infoForGPU_->slot1.y = data_.gridTransition.gridSize;
		infoForGPU_->slot1.z = data_.gridTransition.fadeColor;
		infoForGPU_->slot1.w = data_.gridTransition.pattern;

		psoConfig_.ps = data_.gridTransition.shaderFile;
		task_ &= ~PostEffectJob::GridTransition;
	} else if (task_ & PostEffectJob::SlowMotion) {
		// slot1: chromaticAberration, vignetteStrength, saturation, intensity
		infoForGPU_->slot1.x = data_.slowMotion.chromaticAberration;
		infoForGPU_->slot1.y = data_.slowMotion.vignetteStrength;
		infoForGPU_->slot1.z = data_.slowMotion.saturation;
		infoForGPU_->slot1.w = data_.slowMotion.intensity;

		psoConfig_.ps = data_.slowMotion.shaderFile;
		task_ &= ~PostEffectJob::SlowMotion;
	} else if (task_ & PostEffectJob::Glitch) {
		// slot1: intensity, rgbSplit, scanlineIntensity, blockIntensity
		infoForGPU_->slot1.x = data_.glitch.intensity;
		infoForGPU_->slot1.y = data_.glitch.rgbSplit;
		infoForGPU_->slot1.z = data_.glitch.scanlineIntensity;
		infoForGPU_->slot1.w = data_.glitch.blockIntensity;
		
		// slot2: time, -, -, -
		infoForGPU_->slot2.x = data_.glitch.time;
		infoForGPU_->slot2.y = 0.0f;
		infoForGPU_->slot2.z = 0.0f;
		infoForGPU_->slot2.w = 0.0f;

		psoConfig_.ps = data_.glitch.shaderFile;
		task_ &= ~PostEffectJob::Glitch;
	} else if (task_ & PostEffectJob::ConstantScanline) {
		// slot1: intensity, speed, lineWidth, time
		infoForGPU_->slot1.x = data_.constantScanline.intensity;
		infoForGPU_->slot1.y = data_.constantScanline.speed;
		infoForGPU_->slot1.z = data_.constantScanline.lineWidth;
		infoForGPU_->slot1.w = data_.constantScanline.time;

		psoConfig_.ps = data_.constantScanline.shaderFile;
		task_ &= ~PostEffectJob::ConstantScanline;

	}
	
	psoConfig_.ps = shaderBasePath_ + psoConfig_.ps;
	psoConfig_.vs = shaderBasePath_ + "PostEffect.VS.hlsl";
	psoConfig_.depthStencilID = DepthStencilID::Transparent;
	psoConfig_.rootID = RootSignatureID::PostEffect;
}

void PostEffectResource::SimpleDrawReady() {
	psoConfig_.ps = shaderBasePath_ + "Simple.PS.hlsl";
	psoConfig_.vs = shaderBasePath_ + "PostEffect.VS.hlsl";
	psoConfig_.depthStencilID = DepthStencilID::Transparent;
	psoConfig_.rootID = RootSignatureID::PostEffect;
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
