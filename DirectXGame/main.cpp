#include "MyDirectX.h"
#include "MyMath.h"
#include "externals/imgui/imgui.h"

namespace {
	void input(float* buffer, Vector3 vec) {
		buffer[0] = vec.x;
		buffer[1] = vec.y;
		buffer[2] = vec.z;
	}

	void input(float* buffer, Vector4 vec) {
		buffer[0] = vec.x;
		buffer[1] = vec.y;
		buffer[2] = vec.z;
		buffer[3] = vec.w;
	}

	void output(float buffer[3], Vector3& vec) {
		vec.x = buffer[0];
		vec.y = buffer[1];
		vec.z = buffer[2];
	}

	void output(float buffer[4], Vector4& vec) {
		vec.x = buffer[0];
		vec.y = buffer[1];
		vec.z = buffer[2];
		vec.w = buffer[3];
	}
}

namespace ImGui {
	static void SliderVector(const char* label, Vector3& vec, float min, float max) {
		float buffer[3];
		input(buffer, vec);
		ImGui::SliderFloat3(label, buffer, min, max);
		output(buffer, vec);
	}
	static void ColorEditVector(const char* label, Vector4& vec) {
		float buffer[4];
		input(buffer, vec);
		ImGui::ColorEdit4(label, buffer);
		output(buffer, vec);
	}
}

struct SpriteData {
	Vector4 lt,
		rt,
		lb,
		rb;
};

int WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	const float kWindowWidth = 1280.0f;
	const float kWindowHeight = 720.0f;

	MyDirectX* myDirectX = new MyDirectX(int(kWindowWidth), int(kWindowHeight));
	myDirectX->CreateDrawResource(MyDirectX::kPrism, 1);
	myDirectX->CreateDrawResource(MyDirectX::kSphere, 6);

	myDirectX->ReadTexture("resources/white1x1.png");

	const float pie = 3.14159265358f;

	Transform transform = { 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };

	Transform tra = { 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
	tra.position.z = 5.0f;
	Transform camera = { 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f };
	//camera.position.y = 4.0f;
	//camera.position.z = -1000.0f;
	//camera.rotation.x = 0.7f;
	Matrix4x4 wvpMatrix{};
	MaterialData material = {};
	material.color = { 1.0f, 1.0f, 1.0f, 1.0f };
	material.enableLighting = true;
	DirectionalLightData dLightData = {};

	dLightData.color = { 1.0f, 1.0f, 1.0f, 1.0f };
	dLightData.direction = { 0.0f, -1.0f, 0.0f };
	dLightData.intensity = 1.0f;

	const Matrix4x4 projectionMatrix = MakePerspectiveFovMatrix(0.45f, float(kWindowWidth) / float(kWindowHeight), 0.1f, 100.0f);

	Vector4 lineVertexlist[5]{};
	//上段
	lineVertexlist[0] = { 0.0f, 2.0f, 0.0f, 1.0f };

	//中段
	for (int i = 1; i < 4; ++i) {
		lineVertexlist[i] = { cosf(pie * 2.0f * (i - 1) / 3.0f) / 2.0f, 0.0f, sinf(pie * 2.0f * (i - 1) / 3.0f) / 2.0f, 1.0f };
	}
	//下段
	lineVertexlist[4] = { 0.0f, -2.0f, 0.0f, 1.0f };


	TriangleData3 triangle = {
		{ -0.5f, -0.5f, 0.0f },
		{ 0.0f, 0.5f, 0.0f },
		{ 0.5f, -0.5f, 0.0f }
	};

	TriangleData3 screentr[2]{};

	MSG msg{};

	const float ratioSphereToPrism = 10.0f;

	bool enableLighting = false;

	bool isDrawSphere = false;

	const Vector4 lineBuffer = { 0.01f, 0.0f, 0.01f, 0.0f };

	//ウィンドウのxボタンが押されるまでループ
	while (msg.message != WM_QUIT) {
		//メッセージがあれば処理する
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else {
			//ゲームの更新処理
			myDirectX->BeginFrame();
			ImGui::Begin("Prism");
			ImGui::SliderVector("Position", transform.position, -10.0f, 10.0f);
			ImGui::SliderVector("Rotation", transform.rotation, -3.14f, 3.14f);
			ImGui::SliderVector("Scale", transform.scale, 0.0f, 2.0f);
			ImGui::ColorEditVector("Triangle Color", material.color);
			ImGui::ColorEditVector("Directional Light Color", dLightData.color);
			ImGui::SliderVector("Directional Light Direction", dLightData.direction, -1.0f, 1.0f);
			ImGui::SliderFloat("Directional Light Intensity", &dLightData.intensity, 0.0f, 10.0f);
			ImGui::Checkbox("EnableLight", &enableLighting);
			ImGui::Checkbox("IsDrawSphere", &isDrawSphere);
			ImGui::End();

			material.enableLighting = enableLighting;

			ImGui::Begin("Camera");
			ImGui::SliderVector("Position", camera.position, -10.0f, 10.0f);
			ImGui::SliderVector("Rotation", camera.rotation, -3.14f, 3.14f);
			ImGui::SliderVector("Scale", camera.scale, 0.1f, 10.0f);
			ImGui::End();

			Matrix4x4 viewMatrix = Inverse(MakeTransformMatrix(camera));

			Normalize(dLightData.direction);

			wvpMatrix = MakeTransformMatrix(transform) * viewMatrix * projectionMatrix;

			myDirectX->DrawPrism(MakeTransformMatrix(transform), wvpMatrix, material, dLightData, 1);

			tra = transform;
			tra.scale.x *= 1.0f / ratioSphereToPrism;
			tra.scale.y *= 1.0f / ratioSphereToPrism;
			tra.scale.z *= 1.0f / ratioSphereToPrism;

			Matrix4x4 worldMatrix = MakeTransformMatrix(tra);
			Matrix4x4 wvptraMatrix = worldMatrix * viewMatrix * projectionMatrix;

			myDirectX->DrawSphere(lineVertexlist[0] * ratioSphereToPrism, MakeTransformMatrix(tra), wvptraMatrix, MaterialData({ 0.0f, 0.0f, 0.0f, 0.0f }, enableLighting), dLightData, 1);

			myDirectX->DrawSphere(lineVertexlist[1] * ratioSphereToPrism, MakeTransformMatrix(tra), wvptraMatrix, {0.0f, 0.0f, 0.0f, 0.0f, enableLighting}, dLightData, 1);

			myDirectX->DrawSphere(lineVertexlist[2] * ratioSphereToPrism, MakeTransformMatrix(tra), wvptraMatrix, {0.0f, 0.0f, 0.0f, 0.0f, enableLighting}, dLightData, 1);

			myDirectX->DrawSphere(lineVertexlist[3] * ratioSphereToPrism, MakeTransformMatrix(tra), wvptraMatrix, {0.0f, 0.0f, 0.0f, 0.0f, enableLighting}, dLightData, 1);

			myDirectX->DrawSphere(lineVertexlist[4] * ratioSphereToPrism, MakeTransformMatrix(tra), wvptraMatrix, {0.0f, 0.0f, 0.0f, 0.0f, enableLighting}, dLightData, 1);

			myDirectX->EndFrame();
		}
	}

	delete myDirectX;

	return 0;
}
