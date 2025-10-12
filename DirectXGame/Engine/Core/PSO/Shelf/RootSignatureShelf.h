#pragma once
#include <vector>
#include <cstdint>
#include <d3d12.h>
#include <wrl.h>
#include <Logger/Logger.h>

enum class RootSignatureID : uint8_t {
	Default,
	NonMatrix,
	Particle,
	Model,

	Count
};

class RootSignatureShelf {
public:

	RootSignatureShelf(ID3D12Device* device);
	~RootSignatureShelf();

	ID3D12RootSignature* GetRootSignature(RootSignatureID id) const;
	std::vector<ID3D12RootSignature*> GetRootSignatures() const;

private:

	void CreateRootSignature(D3D12_ROOT_SIGNATURE_DESC& desc, RootSignatureID id, ID3D12Device* device);

	std::vector<ID3D12RootSignature*> rootSignatures_;

	std::unique_ptr<Logger> logger_ = nullptr;

};
