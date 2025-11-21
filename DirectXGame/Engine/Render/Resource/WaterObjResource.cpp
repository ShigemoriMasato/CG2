#include "WaterObjResource.h"
#include <Camera/Camera.h>

WaterObjResource::WaterObjResource() {
	MakeCBV(matrixData_);
	MakeCBV(uniqueData_);
	MakeUAV();
}

void WaterObjResource::Initialize(AssetsID id) {
	SetModelData(id);

	scale = { 1.0f, 1.0f, 1.0f };
	psoConfig_.ps = "Water/WaterObj.PS.hlsl";
	psoConfig_.vs = "Water/WaterObj.VS.hlsl";
	psoConfig_.inputLayoutID = InputLayoutID::Model;
	psoConfig_.rootID = RootSignatureID::Water;
}

void WaterObjResource::DrawReady() {
	Matrix4x4 world = Matrix::MakeAffineMatrix(scale, rotation, position);
	Matrix4x4 wvp = world * camera_->GetVPMatrix();
	matrixData_->wvp = wvp;
	matrixData_->world = world;

	uniqueData_->color = ConvertColor(color);
	uniqueData_->waterHeight = waterHeight;
	uniqueData_->textureIndex_ = textureIndex;
}
