#include "ModelData.h"
#include <Core/DXCommonFunction.h>
#include <Assets/AssetsLoader.h>
#include <cassert>
#include <fstream>
#include <sstream>

#include <assimp/postprocess.h>

namespace {
    void AnimationNullChecker(Animation& animation, const Node& node) {
        if (animation.nodeAnimations.find(node.name) == animation.nodeAnimations.end()) {
            animation.nodeAnimations[node.name].scale.push_back({ 0.0f, {1.0f, 1.0f, 1.0f} });
            animation.nodeAnimations[node.name].rotate.push_back({ 0.0f, {0.0f, 0.0f, 0.0f} });
            animation.nodeAnimations[node.name].translate.push_back({ 0.0f, {0.0f, 0.0f, 0.0f} });
        }

        for(const auto& child : node.children) {
            AnimationNullChecker(animation, child);
		}
    }
}

void ModelData::LoadModel(const std::string& directoryPath, const std::string& filename, AssetsLoader* assetsManager, DXDevice* device) {
    Assimp::Importer importer;
    std::string path = (directoryPath + "/" + filename);
    const aiScene* scene = nullptr;
    scene = importer.ReadFile(path.c_str(), aiProcess_MakeLeftHanded | aiProcess_FlipWindingOrder | aiProcess_FlipUVs | aiProcess_Triangulate);

	LoadMaterial(scene, directoryPath, assetsManager);

	rootNode_ = LoadNode(scene->mRootNode, scene);

	animation_ = LoadAnimationFile(directoryPath, filename);

	AnimationNullChecker(animation_, rootNode_);

	CreateID3D12Resource(device->GetDevice());
}

void ModelData::LoadMaterial(const aiScene* scene, std::string directoryPath, AssetsLoader* assetsManager) {
    for (uint32_t materialIndex = 0; materialIndex < scene->mNumMaterials; ++materialIndex) {
        aiMaterial* material = scene->mMaterials[materialIndex];

        this->material_.push_back(ModelMaterial());
        this->material_[materialIndex].name = material->GetName().C_Str();

        if (material->GetTextureCount(aiTextureType_DIFFUSE) != 0) {

            aiString textureFilePath;
            material->GetTexture(aiTextureType_DIFFUSE, 0, &textureFilePath);

            this->material_.back().textureHandle = 
                assetsManager->Load(std::filesystem::path(directoryPath + "/" + std::string(textureFilePath.C_Str()))).id;

        } else {

            this->material_.back().textureHandle = {}; //テクスチャがなかったら白テクスチャを使う

        }
    }

}

Node ModelData::LoadNode(aiNode* node, const aiScene* scene) {
    Node result{};
	result.name = node->mName.C_Str();
    result.nodeIndex = nodeCount_;
	aiMatrix4x4 mat = node->mTransformation;

    for(int i = 0; i < 4; ++i) {
        for(int j = 0; j < 4; ++j) {
            result.localMatrix.m[i][j] = mat[j][i];
        }
	}

    for (unsigned int i = 0; i < node->mNumMeshes; i++) {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];

        for (uint32_t v = 0; v < mesh->mNumVertices; ++v) {
            aiVector3D pos = mesh->mVertices[v];
            aiVector3D normal = mesh->HasNormals() ? mesh->mNormals[v] : aiVector3D(0, 1, 0);
            aiVector3D tex = mesh->HasTextureCoords(0) ? mesh->mTextureCoords[0][v] : aiVector3D(0, 0, 0);

            ModelVertexData vertexData{};
            vertexData.position = { pos.x,pos.y,pos.z,1.0f };
            vertexData.normal = { normal.x,normal.y,normal.z };
            vertexData.texcoord = { tex.x,tex.y };

            vertexData.textureIndex_ = material_[mesh->mMaterialIndex].textureHandle;
			vertexData.nodeIndex = result.nodeIndex;

            vertices_.push_back(vertexData);
        }

        for (uint32_t faceIndex = 0; faceIndex < mesh->mNumFaces; ++faceIndex) {
			aiFace& face = mesh->mFaces[faceIndex];
			assert(face.mNumIndices == 3); //三角形以外は非対応

            for (unsigned int v = 0; v < face.mNumIndices; v++) {
                int localIndex = face.mIndices[v];
				indices_.push_back(localIndex);
            }//vertex

        }//mesh

    }//node

    nodeCount_++;
    
    // --- 子ノードを再帰的に処理する ---
    for (unsigned int i = 0; i < node->mNumChildren; i++) {
        result.children.push_back(LoadNode(node->mChildren[i], scene));
    }

    return result;
}

void ModelData::CreateID3D12Resource(ID3D12Device* device) {
	//VertexBufferView
    vbv_.resource.Attach(CreateBufferResource(device, sizeof(ModelVertexData) * vertices_.size()));

	ModelVertexData* vertexData = nullptr;

	vbv_.resource->Map(0, nullptr, (void**)&vertexData);
	std::memcpy(vertexData, vertices_.data(), sizeof(ModelVertexData) * vertices_.size());
	vbv_.resource->Unmap(0, nullptr);

	vbv_.bufferView.BufferLocation = vbv_.resource->GetGPUVirtualAddress();
	vbv_.bufferView.SizeInBytes = sizeof(ModelVertexData) * static_cast<UINT>(vertices_.size());
	vbv_.bufferView.StrideInBytes = sizeof(ModelVertexData);

    vbv_.vertexNum = static_cast<int>(vertices_.size());

    //IndexBufferView
	ibv_.indexBuffer.Attach(CreateBufferResource(device, sizeof(uint32_t) * indices_.size()));

	uint32_t* indexData = nullptr;

	ibv_.indexBuffer->Map(0, nullptr, (void**)&indexData);
	std::memcpy(indexData, indices_.data(), sizeof(uint32_t) * indices_.size());
	ibv_.indexBuffer->Unmap(0, nullptr);

	ibv_.bufferView.BufferLocation = ibv_.indexBuffer->GetGPUVirtualAddress();
	ibv_.bufferView.SizeInBytes = sizeof(uint32_t) * static_cast<UINT>(indices_.size());
	ibv_.bufferView.Format = DXGI_FORMAT_R32_UINT;

	ibv_.indexNum = static_cast<int>(indices_.size());
}
