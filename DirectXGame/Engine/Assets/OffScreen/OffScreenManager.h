#pragma once
#include <Core/DXDevice.h>
#include <unordered_map>
#include <string>
#include "OffScreenData.h"
#include <Core/SRVManager.h>

enum class ScreenID {
	SwapChain = -1,
	PostPing,
	PostPong,

	GameWindow,

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
	void Initialize(DXDevice* device, SRVManager* srvManager);

	OffScreenData* GetOffScreenData(ScreenID index);

private:

	int CreateOffScreen(int width, int height, float* clearColor);

	std::vector<std::unique_ptr<OffScreenData>> offScreens_;

	DXDevice* device_ = nullptr;

	//DescriptorHeap
	SRVManager* srvManager_ = nullptr;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvDescriptorHeap_ = nullptr;

	const int maxOffScreenCount_ = static_cast<int>(ScreenID::MaxCount);
};

