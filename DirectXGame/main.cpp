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

struct TriangleData {
	Vector4 left,
		top,
		right;
};

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
	myDirectX->Initialize();

	SpriteData sprite1 = {
	0.0f, 0.0f, 0.0f, 1.0f,
	640.0f, 0.0f, 0.0f, 1.0f,
	0.0f, 360.0f, 0.0f, 1.0f,
	640.0f, 360.0f, 0.0f, 1.0f,
	};

	Transform transform = { 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
	Transform tra = { 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
	Transform camera = { 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -5.0f };
	Matrix4x4 wvpMatrix;
	Vector4 triangleColor = { 1.0f, 1.0f, 1.0f, 1.0f };

	Matrix4x4 viewMatrix = Inverse(MakeTransformMatrix(camera));
	Matrix4x4 projectionMatrix = MakePerspectiveFovMatrix(0.45f, float(kWindowWidth) / float(kWindowHeight), 0.1f, 100.0f);

	myDirectX->ReadTexture("resources/uvChecker.png");
	myDirectX->ReadTexture("resources/cube.jpg");

	bool textureHandle = 1;

	MSG msg{};
	ImGui::SetNextWindowPos(ImVec2(900, 300));

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

			//windowの大きさ(てきとうに)
			ImGui::SetNextWindowSize(ImVec2(300, 400));
			ImGui::Begin("wvpMatrix");
			ImGui::SliderVector("Scale", transform.scale, 0.0f, 10.0f);
			ImGui::SliderVector("Rotation", transform.rotation, -3.14f, 3.14f);
			ImGui::SliderVector("Position", transform.position, -10.0f, 10.0f);
			ImGui::ColorEditVector("Color", triangleColor);
			ImGui::Checkbox("useCubeTexture", &textureHandle);
			ImGui::End();

			ImGui::SetNextWindowSize(ImVec2(300, 400));
			ImGui::Begin("wvp");
			ImGui::SliderVector("Scale", tra.scale, 0.0f, 3.0f);
			ImGui::SliderVector("Rotation", tra.rotation, -3.14f, 3.14f);
			ImGui::SliderVector("Position", tra.position, 0.0f, 1280.0f);
			ImGui::End();

			Matrix4x4 worldMatrix = MakeTransformMatrix(transform);
			wvpMatrix = worldMatrix * viewMatrix * projectionMatrix;
			Matrix4x4 wvp = MakeTransformMatrix(tra) * MakeIdentity4x4() * MakeOrthographicMatrix(0.0f, 0.0f, float(kWindowWidth), float(kWindowHeight), 0.0f, 100.0f);

			myDirectX->DrawTriangle3D(MakeIdentity4x4(), triangleColor, textureHandle);

			myDirectX->EndFrame();
		}
	}

	myDirectX->Finalize();
	delete myDirectX;

	return 0;
}
