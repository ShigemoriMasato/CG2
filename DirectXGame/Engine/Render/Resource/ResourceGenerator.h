#pragma once
#include <d3d12.h>
#include <wrl.h>
#include <string>
#include <Core/SRVManager.h>

class Render;

enum class ViewType {
	CBV,
	SRV,
	UAV
};

class ResourceGenerator {
public:

	ResourceGenerator();
	~ResourceGenerator();

	static void StaticInitialize(ID3D12Device* device, SRVManager* srv);

	void Initialize();
	
	template<typename T>
	void CreateResource(ViewType view, T*& res);
	void Create();

private:

	static ID3D12Device* device_;
	static SRVManager* srvManager_;

};

template<typename T>
void ResourceGenerator::CreateResource(ViewType view, T*& res) {

}
