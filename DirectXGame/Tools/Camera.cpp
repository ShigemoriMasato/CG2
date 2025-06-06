#include "Camera.h"

void Camera::SetPerspectiveFovMatrix(PerspectiveFovDesc desc) {
	projectionMatrix = MakePerspectiveFovMatrix(desc.fovY, desc.aspectRatio, desc.nearClip, desc.farClip);
}

void Camera::SetOrthographicMatrix(OrthographicDesc desc) {
	projectionMatrix = MakeOrthographicMatrix(desc.left, desc.top, desc.right, desc.bottom, desc.nearClip, desc.farClip);
}

void Camera::MakeMatrix() {
	vpMatrix = Inverse(MakeTransformMatrix(transform)) * projectionMatrix;
}

Matrix4x4 Camera::GetViewportMatrix() const {
	return vpMatrix;
}
