#pragma once
#include "Shelf/ShaderShelf.h"
#include "Shelf/BlendStateShelf.h"
#include "Shelf/DepthStencilShelf.h"
#include "Shelf/RasterizerShelf.h"
#include "Shelf/RootSignatureShelf.h"
#include "Shelf/InputLayoutShelf.h"


struct PSOConfig {
	std::string vs = "Object3d.VS.hlsl";
	std::string ps = "Object3d.PS.hlsl";
	RootSignatureID rootID = RootSignatureID::Default;
	InputLayoutID inputLayoutID = InputLayoutID::Default;
	BlendStateID blendID = BlendStateID::Normal;
	DepthStencilID depthStencilID = DepthStencilID::Default;
	RasterizerID rasterizerID = RasterizerID::Fill;
	D3D12_PRIMITIVE_TOPOLOGY topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	bool isSwapChain = false;

	bool operator==(const PSOConfig& other) const {
		return vs == other.vs &&
			ps == other.ps &&
			blendID == other.blendID &&
			depthStencilID == other.depthStencilID &&
			rasterizerID == other.rasterizerID &&
			rootID == other.rootID &&
			inputLayoutID == other.inputLayoutID &&
			topology == other.topology &&
			isSwapChain == other.isSwapChain;
	}

	bool operator!=(const PSOConfig& other) const {
		return !(*this == other);
	}

	/// <summary>
	/// 整合性確認用関数
	/// </summary>
	bool Validate(ShaderShelf& shaderShelf, InputLayoutShelf& inputLayoutShelf, RootSignatureShelf& rootShelf, Logger* logger) const {
		bool valid = true;

		// VS / PS のバイトコード確認
		auto vsBytecode = shaderShelf.GetShaderBytecode(ShaderType::VERTEX_SHADER, vs);
		auto psBytecode = shaderShelf.GetShaderBytecode(ShaderType::PIXEL_SHADER, ps);

		if (!vsBytecode.pShaderBytecode || vsBytecode.BytecodeLength == 0) {
			logger->Log(std::format("Invalid VS bytecode: {}", vs));
			valid = false;
		}

		if (!psBytecode.pShaderBytecode || psBytecode.BytecodeLength == 0) {
			logger->Log(std::format("Invalid PS bytecode: {}", ps));
			valid = false;
		}

		// InputLayout の整合性
		auto inputLayout = inputLayoutShelf.GetInputLayoutDesc(inputLayoutID);
		if (inputLayout.NumElements == 0) {
			logger->Log(std::format("InputLayout is empty: {}", static_cast<int>(inputLayoutID)));
			valid = false;
		}

		// RootSignature の存在確認
		auto rootSig = rootShelf.GetRootSignature(rootID);
		if (!rootSig) {
			logger->Log(std::format("RootSignature is null: {}", static_cast<int>(rootID)));
			valid = false;
		}

		// RTVFormat の妥当性
		DXGI_FORMAT format = isSwapChain ? DXGI_FORMAT_R8G8B8A8_UNORM_SRGB : DXGI_FORMAT_R8G8B8A8_UNORM;
		if (format == DXGI_FORMAT_UNKNOWN) {
			logger->Log("RTVFormat is DXGI_FORMAT_UNKNOWN");
			valid = false;
		}

		// トポロジーの妥当性
		if (topology != D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE &&
			topology != D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE) {
			logger->Log(std::format("Unsupported topology: {}", static_cast<int>(topology)));
			valid = false;
		}

		return valid;
	}
};

namespace std {
	template<>
	struct hash<PSOConfig> {
		size_t operator()(const PSOConfig& cfg) const {
			size_t h = 0;
			hash_combine(h, hash<string>()(cfg.vs));
			hash_combine(h, hash<string>()(cfg.ps));
			hash_combine(h, hash<int>()(static_cast<int>(cfg.blendID)));
			hash_combine(h, hash<int>()(static_cast<int>(cfg.depthStencilID)));
			hash_combine(h, hash<int>()(static_cast<int>(cfg.rasterizerID)));
			hash_combine(h, hash<int>()(static_cast<int>(cfg.rootID)));
			hash_combine(h, hash<int>()(static_cast<int>(cfg.inputLayoutID)));
			hash_combine(h, hash<int>()(static_cast<int>(cfg.topology)));
			hash_combine(h, hash<bool>()(cfg.isSwapChain));
			return h;
		}

	private:
		// シンプルな hash_combine 実装（Boost風）
		static void hash_combine(size_t& seed, size_t value) {
			seed ^= value + 0x9e3779b9 + (seed << 6) + (seed >> 2);
		}
	};
}
