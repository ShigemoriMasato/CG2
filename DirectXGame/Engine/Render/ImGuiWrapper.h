#pragma once
#include <cstdint>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_dx12.h>
#include <Core/DXDevice.h>
#include <Core/SRVManager.h>
#include <imgui/ImGuizmo.h>
#include <Camera/Camera.h>

class Render;

namespace ImGuiOperator {
	void Initialize(DXDevice* device, Render* render, SRVManager* srv);
	void StartFrame(float kClientWidth, float kClientHeight);
	void EndFrame(ID3D12GraphicsCommandList* commandList);
	void Finalize();
}


/// <summary>
/// ギズモで使用
/// </summary>
class ImGuiWrapper {
public:

	ImGuiWrapper() = default;
	~ImGuiWrapper() = default;

	void SetCamera(Camera* camera) { camera_ = camera; };

	void GuizmoUpdate();

	int AddItem(std::string name, Matrix4x4* matrix, Transform* transform);

private:

	struct GuizmoData {
		std::string name;
		Matrix4x4* matrix;
		Transform* transform;
	};

	void UseGuizmo(ImGuizmo::OPERATION operationType, GuizmoData& data);

	std::vector<GuizmoData> guizmoDataMap_;

	static Camera* camera_;

	ImGuizmo::OPERATION operationType_ = ImGuizmo::TRANSLATE;
};

