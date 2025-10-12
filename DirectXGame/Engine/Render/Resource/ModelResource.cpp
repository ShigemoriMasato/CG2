#include "ModelResource.h"

DXDevice* ModelResource::dxDevice_ = nullptr;
SRVManager* ModelResource::srvManager_ = nullptr;

ModelResource::ModelResource() {
}

ModelResource::~ModelResource() {
}

void ModelResource::Initialize(ModelManager* manager, int modelHandle) {
	auto modelData = manager->GetModelData(modelHandle);
	if (!modelData) {
		assert(false && "ModelData is nullptr");
		return;
	}

	node_ = ConvertNodeToTransform(modelData->GetParentNode());

	//データのコピー
	auto vertex = modelData->GetVertexResource();
	auto idnex = modelData->GetIndexResource();
	auto materials = modelData->GetMaterials();

	for (const auto& material : materials) {
		ModelDrawData drawData{};
		drawData.textureHandle = material.textureHandle;

		drawData.vertexBufferView = vertex[material.name].bufferView.get();
		drawData.indexBufferView = idnex[material.name].bufferView.get();
		drawData.indexNum = idnex[material.name].indexNum;

		modelDrawDatas_[material.name] = drawData;
	}

	matrixResource_.Attach(CreateBufferResource(dxDevice_->GetDevice(), sizeof(MatrixData) * modelData->GetNodeCount()));
	matrixResource_->Map(0, nullptr, (void**)&matrix_);

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	srvDesc.Buffer.FirstElement = 0;
	srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
	srvDesc.Buffer.NumElements = modelData->GetNodeCount();
	srvDesc.Buffer.StructureByteStride = sizeof(MatrixData);

	D3D12_CPU_DESCRIPTOR_HANDLE matrixCPUHandle = srvManager_->GetCPUHandle();
	matrixGPUHandle_ = srvManager_->GetGPUHandle();

	dxDevice_->GetDevice()->CreateShaderResourceView(matrixResource_.Get(), &srvDesc, matrixCPUHandle);


	materialResource_.Attach(CreateBufferResource(dxDevice_->GetDevice(), sizeof(Material)));
	materialResource_->Map(0, nullptr, (void**)&material_);

	psoConfig_.vs = "Model.VS.hlsl";
	psoConfig_.ps = "Model.PS.hlsl";
	psoConfig_.rootID = RootSignatureID::Model;
	psoConfig_.inputLayoutID = InputLayoutID::Model;
}

void ModelResource::DrawReady() {

	material_->color = {
		((color_ >> 16) & 0xff) / 255.0f,
		((color_ >> 8) & 0xff) / 255.0f,
		(color_ & 0xff) / 255.0f,
		((color_ >> 24) & 0xff) / 255.0f
	};

	DrawReadyNode(node_, Matrix::MakeIdentity4x4());
}

ModelResource::NodeTransform ModelResource::ConvertNodeToTransform(const Node& node) {
	NodeTransform result{};
	result.localMatrix = node.localMatrix;
	result.name = node.name;
	result.nodeIndex = node.nodeIndex;
	for (const auto& child : node.children) {
		result.children.push_back(ConvertNodeToTransform(child));
	}

	return result;
}

void ModelResource::DrawReadyNode(NodeTransform node, const Matrix4x4& parentMatrix) {

	Matrix4x4 worldMatrix = Matrix::MakeScaleMatrix(scale_) * Matrix::MakeRotationMatrix(rotate_) * Matrix::MakeTranslationMatrix(position_);
	Matrix4x4 localMatrix = Matrix::MakeScaleMatrix(node.scale) * Matrix::MakeRotationMatrix(node.rotate) * Matrix::MakeTranslationMatrix(node.translate);

	matrix_[node.nodeIndex].world = parentMatrix * node.localMatrix * localMatrix * worldMatrix;
	matrix_[node.nodeIndex].wvp = matrix_[node.nodeIndex].world * camera_->GetVPMatrix();

	for(auto& child : node.children) {
		DrawReadyNode(child, matrix_[node.nodeIndex].world);
	}
}
