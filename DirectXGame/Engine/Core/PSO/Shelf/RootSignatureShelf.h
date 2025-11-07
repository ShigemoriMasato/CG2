#pragma once
#include <vector>
#include <cstdint>
#include <d3d12.h>
#include <wrl.h>
#include <xutility>
#include <Logger/Logger.h>
#include <Core/ECS/EntityID.h>
#include "ShaderShelf.h"

enum class ViewType {
	CBV,
	SRV,
	UAV,
	MaxCount
};

struct RootSignatureID : public EntityID {};

struct RSConfig {
	std::vector<std::pair<ViewType, ShaderType>> views;

	bool operator==(const RSConfig& other) const {
		return views == other.views;
	}
};

// RSConfig用のハッシュ関数
namespace std {
	template<>
	struct hash<RSConfig> {
		size_t operator()(const RSConfig& config) const noexcept {
			size_t seed = 0;
			for (const auto& view : config.views) {
				// ViewTypeとShaderTypeをハッシュ化して結合
				size_t h1 = hash<int>{}(static_cast<int>(view.first));
				size_t h2 = hash<int>{}(static_cast<int>(view.second));
				// ハッシュ結合 (boost::hash_combineと同様の手法)
				seed ^= h1 + 0x9e3779b9 + (seed << 6) + (seed >> 2);
				seed ^= h2 + 0x9e3779b9 + (seed << 6) + (seed >> 2);
			}
			return seed;
		}
	};
}

class RootSignatureShelf {
public:

	RootSignatureShelf(ID3D12Device* device);
	~RootSignatureShelf();

	RootSignatureID CreateRootSignature(ID3D12Device* device, std::vector<std::pair<D3D12_GPU_VIRTUAL_ADDRESS, ShaderType>> cbv,
		std::vector<std::pair<D3D12_GPU_DESCRIPTOR_HANDLE, ShaderType>> srv,
		std::vector<std::pair<D3D12_GPU_DESCRIPTOR_HANDLE, ShaderType>> uav);

	ID3D12RootSignature* GetRootSignature(RootSignatureID id) const;

private:

	std::unordered_map<RSConfig, RootSignatureID> configToIDMap_;

	std::map<RootSignatureID, ID3D12RootSignature*> rootSignatures_;

	std::shared_ptr<spdlog::logger> logger_;

	uint32_t nextID_ = 0;
};
