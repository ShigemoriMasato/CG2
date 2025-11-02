#include "ResourceGenerator.h"
ID3D12Device* ResourceGenerator::device_ = nullptr;
SRVManager* ResourceGenerator::srvManager_ = nullptr;

void ResourceGenerator::StaticInitialize(ID3D12Device* device, SRVManager* srv) {
	device_ = device;
	srvManager_ = srv;
}

ResourceGenerator::ResourceGenerator() {
}

ResourceGenerator::~ResourceGenerator() {
}

void ResourceGenerator::Initialize() {
}

void ResourceGenerator::Create() {
}
