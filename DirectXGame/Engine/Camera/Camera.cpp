#include "Camera.h"
#include "../../externals/imgui/imgui.h"

using namespace Matrix;
using namespace MyMath;

namespace {
	Matrix4x4 MakePerspectiveFovMatrix(float fovY, float aspectRatio, float nearClip, float farClip) {
		return {
			cot(fovY / 2) / aspectRatio, 0, 0, 0,
			0, cot(fovY / 2), 0, 0,
			0, 0, farClip / (farClip - nearClip), 1,
			0, 0, (-nearClip * farClip) / (farClip - nearClip), 0
		};
	}

	Matrix4x4 MakeOrthographicMatrix(float left, float top, float right, float bottom, float nearClip, float farClip) {
		return {
			2 / (right - left), 0, 0, 0,
			0, 2 / (top - bottom), 0, 0,
			0, 0, 1 / (farClip - nearClip), 0,
			(left + right) / (left - right), (top + bottom) / (bottom - top), nearClip / (nearClip - farClip), 1
		};
	}

	Matrix4x4 MakeViewportMatrix(float left, float top, float width, float height, float minDepth, float maxDepth) {
		return {
			width / 2, 0, 0, 0,
			0, -height / 2, 0, 0,
			0, 0, maxDepth - minDepth, 0,
			left + width / 2, top + height / 2, minDepth, 1
		};
	}
}

void Camera::SetProjectionMatrix(PerspectiveFovDesc desc) {
	projectionMatrix = MakePerspectiveFovMatrix(desc.fovY, desc.aspectRatio, desc.nearClip, desc.farClip);
}

void Camera::SetProjectionMatrix(OrthographicDesc desc) {
	projectionMatrix = MakeOrthographicMatrix(desc.left, desc.top, desc.right, desc.bottom, desc.nearClip, desc.farClip);
}

void Camera::MakeMatrix() {
	if (!isSetMatrix) {
		transformMatrix_ = MakeTranslationMatrix(-transform_.position) * MakeRotationMatrix(transform_.rotation) * MakeScaleMatrix(transform_.scale);
	}
	vpMatrix = transformMatrix_ * projectionMatrix;
}

void Camera::DrawImGui() {
	ImGui::Begin("Camera");
	ImGui::DragFloat3("Position", &transform_.position.x, 0.1f);
	ImGui::DragFloat3("Rotation", &transform_.rotation.x, 0.01f);
	ImGui::DragFloat3("Scale", &transform_.scale.x, 0.1f);
	ImGui::End();
}

void Camera::SetTransform(Transform transform) {
	transform_ = transform;
	isSetMatrix = false;
}

void Camera::SetTransform(Matrix4x4 mat) {
	transformMatrix_ = mat;
	isSetMatrix = true;
}

Matrix4x4 Camera::GetVPMatrix() const {
	return vpMatrix;
}
