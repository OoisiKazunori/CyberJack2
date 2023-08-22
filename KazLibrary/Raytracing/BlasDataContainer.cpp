#include "BlasDataContainer.h"
#include "Blas.h"

std::vector<std::shared_ptr<Raytracing::Blas>> Raytracing::BlasDataContainer::SetBlas(bool arg_isOpaque, int arg_vertexHandle, std::vector<std::vector<KazBufferHelper::BufferData>> arg_materialBuffer)
{

    //データが保存されているかをチェック
    for (auto& index : m_meshDatas) {

        //すでに生成済みのデータか？
        bool isOpaque = arg_isOpaque == index.m_isOpaque;
        bool isVertexHandle = arg_vertexHandle == index.m_vertexHandle;
        if (isOpaque && isVertexHandle) {

            //生成済みだったらそのデータのBlasを返す。
            return index.m_mesh;

        }

    }

    //保存されていなかったら生成する。
    const int BLAS_COUNT = static_cast<int>(arg_materialBuffer.size());
    BlasMesh mesh;
    mesh.m_mesh.resize(BLAS_COUNT);
    for (int counter = 0; counter < BLAS_COUNT; ++counter) {
        mesh.m_mesh[counter] = std::make_shared<Raytracing::Blas>(arg_isOpaque, arg_vertexHandle, counter, arg_materialBuffer[counter][0].bufferWrapper->GetViewHandle(), static_cast<int>(m_meshDatas.size()));
    }
    mesh.m_isOpaque = arg_isOpaque;
    mesh.m_vertexHandle = arg_vertexHandle;
    m_meshDatas.emplace_back(mesh);

    return m_meshDatas.back().m_mesh;
}
