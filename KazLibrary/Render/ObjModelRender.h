#pragma once
#include"../DirectXCommon/Base.h"
#include"../Render/IRender.h"
#include"../Math/KazMath.h"
#include"../Helper/DirtyFlag.h"
#include"../Render/RenderData.h"
#include"../Loader/ObjResourceMgr.h"
#include<unordered_map>

class ObjModelRender :public IRender
{
public:
	ObjModelRender(bool INSTANCE_FLAG = false, int INSTANCE_NUM = 1, bool MATERIAL_NONE_FALG = false);
	~ObjModelRender();

	DirectX::XMMATRIX* GetMotherMatrixPtr();
	void Draw();

	bool demoFlag;
	Obj3DData data;
private:

	UINT vertByte;
	UINT indexByte;


	short vertexBufferHandle;
	short indexBufferHandle;

	ObjResourceData modelData;
	std::array<RESOURCE_HANDLE, 2> constBufferHandle;

	DirectX::XMMATRIX motherMat;
	bool instanceFlag;
	int instanceNum;


	unordered_map<unsigned short, vector<unsigned short>> smoothData;
	void AddSmoothData(const unsigned short &INDEX_POSITION,const unsigned short &INDEX_VERTEX)
	{
		smoothData[INDEX_POSITION].emplace_back(INDEX_VERTEX);
	};
	void CaluculateSmoothedVertexNormals(vector<Vertex> &DATA)
	{
		auto itr = smoothData.begin();
		for (; itr != smoothData.end(); ++itr)
		{
			vector<unsigned short> &v = itr->second;
			KazMath::Vec3<float> normal = {};
			for (unsigned short index : v)
			{
				unsigned short tmp = index;
				normal += KazMath::Vec3<float>(DATA[tmp].normal.x, DATA[tmp].normal.y, DATA[tmp].normal.z);
			}
			normal /= static_cast<float>(v.size());
			normal.Normalize();

			for (unsigned short index : v)
			{
				DATA[index].normal = { normal.x,normal.y,normal.z };
			}
		}
	}
};

