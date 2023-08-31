#pragma once
#include"../Helper/ISinglton.h"
#include<string>
#include"../KazLibrary/Helper/ISinglton.h"
#include"../KazLibrary/Buffer/CreateMeshBuffer.h"

struct MovieLoadData
{
	std::string filePass;
	UINT16 x, y;
};
class MovieLoader:public ISingleton<MovieLoader>
{
public:
	MovieLoader();

	RESOURCE_HANDLE Load(const MovieLoadData &MOVIE_DATA);
	HANDLE GetShareHandle(RESOURCE_HANDLE HANDLE);
	RESOURCE_HANDLE GetDescriptorAddress(RESOURCE_HANDLE HANDLE);
	Microsoft::WRL::ComPtr<ID3D12Resource> GetBuffer(RESOURCE_HANDLE HANDLE);

	void Relese(RESOURCE_HANDLE HANDLE);

private:
	HandleMaker handle;
	std::vector<std::string> fileNameArray;
	std::vector<HANDLE> shareHandleArray;
	CreateGpuBuffer buffers;
};

