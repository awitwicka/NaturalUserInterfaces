#pragma once

#include <DirectXMath.h>
#include <string>
#include <vector>
#include "mini_mesh.h"
#include "mini_sceneNode.h"
#include "mini_material.h"

namespace mini
{
	class SceneGraph
	{
	public:
		SceneGraph(std::vector<SceneNode>&& nodes, std::vector<Mesh>&& meshes, std::vector<Material>&& materials);
		SceneGraph(SceneGraph&& right);
		SceneGraph(const SceneGraph& right) = delete;

		SceneGraph& operator=(const SceneGraph& right) = delete;
		SceneGraph& operator=(SceneGraph&& right);

		int nodeByName(const std::string& name) const;
		DirectX::XMFLOAT4X4 getNodeTransform(unsigned nodeIndex) const;
		void setNodeTransform(unsigned int nodeIndex, const DirectX::XMFLOAT4X4& transform);
		int nodeFirstChild(unsigned int nodeIndex) const;
		int nodeNextSybling(unsigned int nodeIndex) const;
		int nodeParent(unsigned int nodeIndex) const;

		unsigned int meshCount() const { return m_meshes.size(); }
		Mesh& getMesh(unsigned int index) { return m_meshes.at(index); }
		Material& getMaterial(unsigned int index) { return m_materials.at(index); }
		
	private:
		void UpdateChildTransforms(unsigned int childIdx, DirectX::CXMMATRIX parentTransform);

		void Clear();

		std::vector<Mesh> m_meshes;
		std::vector<SceneNode> m_nodes;
		std::vector<Material> m_materials;

	};
}