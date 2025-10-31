#pragma once
#include <d3d12.h>
#include <wrl.h>
#include <string>
#include <Resource/SRVManager.h>

class ResourceGenerator {
public:

	ResourceGenerator();
	~ResourceGenerator();

	static void StaticInitialize(ID3D12Device* device, SRVManager* srv);

	void Initialize();

private:



};
