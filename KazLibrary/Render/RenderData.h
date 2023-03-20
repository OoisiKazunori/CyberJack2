#pragma once
#include"../Math/KazMath.h"
#include"../Pipeline/GraphicsPipeLineMgr.h"

struct AddTextureData
{
	std::array<RESOURCE_HANDLE, 10> handle;
	std::array<GraphicsRootParamType, 10> paramType;

	AddTextureData()
	{
		for (int i = 0; i < handle.size(); ++i)
		{
			handle[i] = -1;
			paramType[i] = GRAPHICS_PRAMTYPE_NONE;
		}
	}
};

struct FlipData
{
	bool x;
	bool y;

	FlipData() :x(false), y(false), xDirtyFlag(&x, false), yDirtyFlag(&y, false)
	{
	}

	void Record()
	{
		xDirtyFlag.Record();
		yDirtyFlag.Record();
	}

	DirtyFlag<bool>xDirtyFlag;
	DirtyFlag<bool>yDirtyFlag;
};

struct ResourceHandle
{
	RESOURCE_HANDLE handle;
	DirtyFlag<RESOURCE_HANDLE>flag;

	ResourceHandle() :handle(-1), flag(&handle)
	{}

	void operator=(RESOURCE_HANDLE HANDLE)
	{
		handle = HANDLE;
	}
	bool operator==(RESOURCE_HANDLE HANDLE)
	{
		return handle == HANDLE;
	}
};

struct MatMotherData
{
	DirectX::XMMATRIX mat;
	DirtySet dirty;

	MatMotherData() :mat(DirectX::XMMatrixIdentity()), dirty(mat)
	{
	}

	void operator=(const DirectX::XMMATRIX &MATRIX)
	{
		mat = MATRIX;
	}
	void operator=(DirectX::XMMATRIX *MATRIX)
	{
		mat = *MATRIX;
	}
};

struct CameraIndexData
{
	int id;
	DirtyFlag<int> dirty;

	CameraIndexData() :id(0), dirty(&id)
	{
	}
	void operator=(int HANDLE)
	{
		id = HANDLE;
	}
};


struct Sprite2DData
{
	KazMath::Transform2D transform;
	ResourceHandle handleData;
	ResourceHandle animationHandle;
	FlipData flip;
	PipeLineNames pipelineName;

	KazMath::Color colorData;
	AddTextureData addHandle;
	KazMath::Vec2<float>leftScale;
	KazMath::Vec2<float>rightScale;

	DirtyFlag<KazMath::Vec2<float>> leftScaleDirtyFlag;
	DirtyFlag<KazMath::Vec2<float>> rightScaleDirtyFlag;

	Sprite2DData() :colorData(0, 0, 0, 255), pipelineName(PIPELINE_NAME_SPRITE),
		leftScale({ 1.0f,1.0f }), rightScale({ 1.0f,1.0f }),
		leftScaleDirtyFlag(&leftScale, true), rightScaleDirtyFlag(&rightScale, true)
	{
	}

	void Record()
	{
		transform.Record();
		handleData.flag.Record();
		animationHandle.flag.Record();
		flip.Record();
		colorData.Record();
		leftScaleDirtyFlag.Record();
		rightScaleDirtyFlag.Record();
	};
};


struct MovieData
{
	KazMath::Transform2D transform;
	ResourceHandle handleData;
	FlipData flip;
	PipeLineNames pipelineName;

	KazMath::Color colorData;
	AddTextureData addHandle;

	Microsoft::WRL::ComPtr<ID3D12Resource>buff;

	MovieData() :colorData(0, 0, 0, 255), pipelineName(PIPELINE_NAME_SPRITE_NOBLEND)
	{
	}

	void Record()
	{
		transform.Record();
		handleData.flag.Record();
		flip.Record();
		colorData.Record();
	};
};



struct Sprite3DData
{
	KazMath::Transform3D transform;
	ResourceHandle handleData;
	ResourceHandle animationHandle;
	AddTextureData addHandle;
	FlipData flip;
	bool billBoardFlag;
	MatMotherData motherMat;
	PipeLineNames pipelineName;
	KazMath::Color colorData;
	CameraIndexData cameraIndex;

	Sprite3DData() :pipelineName(PIPELINE_NAME_SPRITE), billBoardDirtyFlag(&billBoardFlag), colorData(0, 0, 0, 255)
	{
	}

	void Record()
	{
		transform.Record();
		handleData.flag.Record();
		animationHandle.flag.Record();
		billBoardDirtyFlag.Record();
		colorData.Record();
		flip.Record();
		motherMat.dirty.Record();
		cameraIndex.dirty.Record();
	};

	DirtyFlag<bool>billBoardDirtyFlag;
};

struct Pera3DData
{
	KazMath::Transform3D transform;
	bool billBoardFlag;
	MatMotherData motherMat;
	PipeLineNames pipelineName;
	KazMath::Color color;
	CameraIndexData cameraIndex;
	DirtyFlag<bool>billBoardDirtyFlag;

	Pera3DData() :pipelineName(PIPELINE_NAME_SPRITE), color(0, 0, 0, 255), billBoardDirtyFlag(&billBoardFlag)
	{
	}
	void Record()
	{
		transform.Record();
		color.Record();
		billBoardDirtyFlag.Record();
		motherMat.dirty.Record();
		cameraIndex.dirty.Record();
	}
};

struct Obj3DData
{
	KazMath::Transform3D transform;
	ResourceHandle handle;
	KazMath::Vec3<float> upVector;
	KazMath::Vec3<float> frontVector;
	MatMotherData motherMat;
	PipeLineNames pipelineName;
	KazMath::Color colorData;
	bool removeMaterialFlag;
	AddTextureData addHandle;

	DirtyFlag<KazMath::Vec3<float>>upVecDirtyFlag;
	DirtyFlag<KazMath::Vec3<float>>frontVecDirtyFlag;
	CameraIndexData cameraIndex;

	Obj3DData() :pipelineName(PIPELINE_NAME_OBJ), colorData(0, 0, 0, 255), upVecDirtyFlag(&upVector), frontVecDirtyFlag(&frontVector), upVector({ 0,1,0 }), frontVector({ 0,0,1 }),
		removeMaterialFlag(false)
	{
	}

	void Record()
	{
		transform.Record();
		handle.flag.Record();
		colorData.Record();
		upVecDirtyFlag.Record();
		frontVecDirtyFlag.Record();
		motherMat.dirty.Record();
		cameraIndex.dirty.Record();
	}
};

struct LineDrawData
{
	KazMath::Vec3<float> startPos;
	KazMath::Vec3<float> endPos;
	KazMath::Color colorData;
	MatMotherData motherMat;
	PipeLineNames pipelineName;
	CameraIndexData cameraIndex;

	DirtyFlag<KazMath::Vec3<float>> startPosDirtyFlag;
	DirtyFlag<KazMath::Vec3<float>> endPosDirtyFlag;

	LineDrawData() :pipelineName(PIPELINE_NAME_LINE), colorData(255, 255, 255, 255), startPosDirtyFlag(&startPos), endPosDirtyFlag(&endPos)
	{
	}
	void Record()
	{
		startPosDirtyFlag.Record();
		endPosDirtyFlag.Record();
		colorData.Record();
		motherMat.dirty.Record();
		cameraIndex.dirty.Record();
	}
};

struct PolygonDrawData
{
	KazMath::Transform3D transform;
	KazMath::Color color;
	MatMotherData motherMat;
	PipeLineNames pipelineName;
	CameraIndexData cameraIndex;

	PolygonDrawData() :color(255, 255, 255, 255), pipelineName(PIPELINE_NAME_COLOR)
	{
	}

	void Record()
	{
		transform.Record();
		color.Record();
		motherMat.dirty.Record();
		cameraIndex.dirty.Record();
	};

};

struct FbxModelData
{
	KazMath::Transform3D transform;
	ResourceHandle handle;
	MatMotherData motherMat;
	bool isPlayFlag;
	bool isReverseFlag;
	bool stopAnimationFlag;
	bool removeMaterialFlag;
	int animationNumber;
	PipeLineNames pipelineName;
	CameraIndexData cameraIndex;
	KazMath::Color colorData;

	FbxModelData() :pipelineName(PIPELINE_NAME_FBX), animationNumber(0), isPlayFlag(false), isReverseFlag(false), colorData({ 255,255,255,255 }), stopAnimationFlag(false)
	{
	}

	void Record()
	{
		transform.Record();
		handle.flag.Record();
		motherMat.dirty.Record();
		cameraIndex.dirty.Record();
	}
};

struct CircleDrawData
{
	KazMath::Transform3D transform;
	KazMath::Color colorData;
	float radius;
	PipeLineNames pipelineName;
	bool change3DFlag;
	CameraIndexData cameraIndex;
	bool fillFlag;
	bool billBoardFlag;

	CircleDrawData() :colorData(255, 255, 255, 255), radius(0.0f), pipelineName(PIPELINE_NAME_COLOR_NOCARING), change3DFlag(false), change3DDirtyFlag(&change3DFlag), radiusDirtyFlag(&radius), fillFlag(true), billBoardFlag(false)
	{
	}

	void Record()
	{
		transform.Record();
		colorData.Record();
		radiusDirtyFlag.Record();
		change3DDirtyFlag.Record();
		cameraIndex.dirty.Record();
	};
	DirtyFlag<float> radiusDirtyFlag;
	DirtyFlag<bool> change3DDirtyFlag;
};

struct Box2DRenderData
{
	KazMath::Vec2<float> leftUpPos;
	KazMath::Vec2<float> rightDownPos;
	KazMath::Vec2<float> leftDownPos;
	KazMath::Vec2<float> rightUpPos;
	KazMath::Color color;
	PipeLineNames pipelineName;

	DirtyFlag<KazMath::Vec2<float>> leftUpPosDirtyFlag;
	DirtyFlag<KazMath::Vec2<float>> rightDownPosDirtyFlag;
	DirtyFlag<KazMath::Vec2<float>> leftDownPosDirtyFlag;
	DirtyFlag<KazMath::Vec2<float>> rightUpPosDirtyFlag;


	Box2DRenderData() :
		leftUpPosDirtyFlag(&leftUpPos), rightDownPosDirtyFlag(&rightDownPos),
		leftDownPosDirtyFlag(&leftDownPos), rightUpPosDirtyFlag(&rightUpPos),
		color(255, 255, 255, 255), pipelineName(PIPELINE_NAME_COLOR_LINE)
	{};

	bool VertDirty()
	{
		return leftUpPosDirtyFlag.Dirty() || leftDownPosDirtyFlag.Dirty() || rightUpPosDirtyFlag.Dirty() || rightDownPosDirtyFlag.Dirty();
	}

	void Record()
	{
		color.Record();
		leftUpPosDirtyFlag.Record();
		leftDownPosDirtyFlag.Record();
		rightUpPosDirtyFlag.Record();
		rightDownPosDirtyFlag.Record();
	};
};