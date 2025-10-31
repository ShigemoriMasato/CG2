#pragma once
#include <Core/DXDevice.h>
#include <unordered_map>
#include <string>
#include "OffScreenData.h"
#include <Resource/SRVManager.h>

enum class OffScreenIndex {
	SwapChain = -1,
	ImGuiDocking,
	PostPing,
	PostPong,
	GameWindow,
	Title,
	Select,  // セレクトシーン用

	//ステージのスクショ
	Level1,
	Level2,
	Level3,
	Level4,
	Level5,

	MaxCount
};

/// <summary>
/// offscreenの管理クラス。作成時の設定は初期化関数の中身をいじって変えること
/// </summary>
class OffScreenManager {
public:

	OffScreenManager();
	~OffScreenManager();

	/// <summary>
	/// OffScreenを初期化、作成する
	/// </summary>
	void Initialize(DXDevice* device, ID3D12GraphicsCommandList* commandList, SRVManager* srvManager);

	OffScreenData* GetOffScreenData(OffScreenIndex index);

private:

	int CreateOffScreen(int width, int height, float* clearColor);

	std::vector<std::unique_ptr<OffScreenData>> offScreens_;

	DXDevice* device_ = nullptr;
	ID3D12GraphicsCommandList* commandList_ = nullptr;

	//DescriptorHeap
	SRVManager* srvManager_ = nullptr;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvDescriptorHeap_ = nullptr;

	const int maxOffScreenCount_ = static_cast<int>(OffScreenIndex::MaxCount);
};

