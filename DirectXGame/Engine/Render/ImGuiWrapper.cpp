#include "ImGuiWrapper.h"
#include <imgui/imgui_impl_win32.h>
#include <Render/Render.h>
#include <Core/EngineTerminal.h>

Camera* ImGuiWrapper::camera_ = nullptr;

void ImGuiOperator::Initialize(DXDevice* device, Render* render, SRVManager* srv) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplWin32_Init(device->GetHwnd());
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable; // ドッキングを有効化

	ImGui_ImplDX12_InitInfo initInfo = render->GetImGuiInitInfo(srv);

    ImGui_ImplDX12_Init(&initInfo);

    
}

void ImGuiOperator::StartFrame(float kClientWidth, float kClientHeight) {
    ImGuiIO& io = ImGui::GetIO();

    ImGui_ImplDX12_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
    ImGuizmo::BeginFrame();

    ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(float(kClientWidth), float(kClientHeight)), ImGuiCond_Always);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f); // 角丸をなくす

    ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);

    ImGui::Begin("DockSpaceWindow", nullptr, ImGuiWindowFlags_NoTitleBar | // タイトルバーなし 
        ImGuiWindowFlags_NoResize | // リサイズ不可
        ImGuiWindowFlags_NoMove | // 移動不可
        ImGuiWindowFlags_NoScrollbar | // スクロールバーなし
        ImGuiWindowFlags_NoCollapse | // 折り畳み不可
        ImGuiWindowFlags_NoBackground | // 背景なし（必要に応じて）
        ImGuiWindowFlags_NoSavedSettings);

    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");

    ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode);

    ImGui::End();
    ImGui::PopStyleVar(2);

}

void ImGuiOperator::EndFrame(ID3D12GraphicsCommandList* commandList) {

#if SH_DEBUG || SH_DEVELOP

    ImGui::Render();
    ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), commandList);

#else

    ImGui::EndFrame();

#endif

}

void ImGuiOperator::Finalize() {
    ImGui_ImplDX12_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}

void ImGuiWrapper::GuizmoUpdate() {

    if (ImGui::IsKeyPressed(ImGuiKey_T)) {
        operationType_ = ImGuizmo::TRANSLATE;
    }

    if (ImGui::IsKeyPressed(ImGuiKey_R)) {
        operationType_ = ImGuizmo::ROTATE;
    }

    if (ImGui::IsKeyPressed(ImGuiKey_S)) {
        operationType_ = ImGuizmo::SCALE;
    }

    for(auto& data : guizmoDataMap_) {
        UseGuizmo(operationType_, data);

        if (ImGuizmo::IsOver()) {
            ImGuizmo::Enable(true);
        } else {
            ImGuizmo::Enable(false);
        }
	}
}

void ImGuiWrapper::UseGuizmo(ImGuizmo::OPERATION operationType, GuizmoData& data) {

    if (!camera_) {
        return;
    }

    float worldMat[16];
    std::memcpy(worldMat, data.matrix->m, sizeof(float) * 16);
	data.transform->rotation *= 180.0f / 3.141592f;
    ImGuizmo::DecomposeMatrixToComponents(worldMat, &data.transform->position.x, &data.transform->rotation.x, &data.transform->scale.x);
    ImGui::InputFloat3("Tr", &data.transform->position.x);
    ImGui::InputFloat3("Rt", &data.transform->rotation.x);
    ImGui::InputFloat3("Sc", &data.transform->scale.x);
    ImGuizmo::RecomposeMatrixFromComponents(&data.transform->position.x, &data.transform->rotation.x, &data.transform->scale.x, worldMat);
	data.transform->rotation *= 3.141592f / 180.0f;

    float viewMat[16];
	float projMat[16];
	memcpy(viewMat, camera_->GetTranformMatrix().m, sizeof(float) * 16);
	memcpy(projMat, camera_->GetProjectionMatrix().m, sizeof(float) * 16);

    ImGuizmo::Manipulate(viewMat, projMat, operationType, ImGuizmo::WORLD, worldMat);

	std::memcpy(data.matrix->m, worldMat, sizeof(float) * 16);
}

int ImGuiWrapper::AddItem(std::string name, Matrix4x4* matrix, Transform* transform) {
    GuizmoData data;
    data.name = name;
    data.matrix = matrix;
    data.transform = transform;
    guizmoDataMap_.push_back(data);

    return static_cast<int>(guizmoDataMap_.size() - 1);
}
