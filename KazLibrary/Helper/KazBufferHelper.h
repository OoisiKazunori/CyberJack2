#pragma once
#include"../DirectXCommon/Base.h"
#include"../KazLibrary/DirectXCommon/DirectX12CmdList.h"
#include"../KazLibrary/DirectXCommon/DirectX12Device.h"
#include"../KazLibrary/Pipeline/GraphicsRootSignature.h"

struct BufferMemorySize
{
	int startSize;
	int endSize;

	BufferMemorySize() :startSize(-1), endSize(-1)
	{};
};

enum BufferMemory
{
	DESCRIPTORHEAP_MEMORY_NONE = -1,
	DESCRIPTORHEAP_MEMORY_TEXTURE_IMGUI,
	DESCRIPTORHEAP_MEMORY_TEXTURE_SPRITE,
	DESCRIPTORHEAP_MEMORY_TEXTURE_OBJ,
	DESCRIPTORHEAP_MEMORY_TEXTURE_FBX,
	DESCRIPTORHEAP_MEMORY_TEXTURE_RENDERTARGET,
	DESCRIPTORHEAP_MEMORY_TEXTURE_COMPUTEBUFFER,
	DESCRIPTORHEAP_MEMORY_CBV,
	DESCRIPTORHEAP_MEMORY_SRV,
	DESCRIPTORHEAP_MEMORY_MOVIE,
	DESCRIPTORHEAP_MEMORY_MAX
};

namespace KazBufferHelper
{
	struct BufferResourceData
	{
		D3D12_HEAP_PROPERTIES heapProperties;
		D3D12_HEAP_FLAGS heapFlags;
		D3D12_RESOURCE_DESC resourceDesc;
		D3D12_RESOURCE_STATES resourceState;
		D3D12_CLEAR_VALUE *clearValue;
		std::string BufferName;

		BufferResourceData
		(
			const D3D12_HEAP_PROPERTIES &HEAP_PROP,
			const D3D12_HEAP_FLAGS &HEAP_FLAGS,
			const D3D12_RESOURCE_DESC &RESOURCE_DESC,
			const D3D12_RESOURCE_STATES &RESOURCE_STATE,
			D3D12_CLEAR_VALUE *CLEAR_VALUE,
			const std::string &BUFFER_NAME
		)
			:heapProperties(HEAP_PROP),
			heapFlags(HEAP_FLAGS),
			resourceDesc(RESOURCE_DESC),
			resourceState(RESOURCE_STATE),
			clearValue(CLEAR_VALUE),
			BufferName(BUFFER_NAME)
		{
		};
	};


	class ID3D12ResourceWrapper
	{
	public:
		ID3D12ResourceWrapper();

		/// <summary>
		/// �o�b�t�@����
		/// </summary>
		/// <param name="BUFFER_OPTION">�o�b�t�@�𐶐�����ׂɕK�v�ȍ\����</param>
		void CreateBuffer(const BufferResourceData &BUFFER_OPTION);

		/// <summary>
		/// �f�[�^���o�b�t�@�ɓ]�����܂�
		/// </summary>
		/// <param name="DATA">���肽���f�[�^�̃A�h���X</param>
		/// <param name="DATA_SIZE">���肽���f�[�^�̃T�C�Y</param>
		void TransData(void *DATA, const unsigned int &DATA_SIZE);
		void TransData(void *DATA, unsigned int START_DATA_SIZE, unsigned int DATA_SIZE);

		/// <summary>
		/// �o�b�t�@���J�����܂�
		/// </summary>
		/// <param name="HANDLE">�J���������o�b�t�@�̃n���h��</param>
		void Release();

		/// <summary>
		/// �o�b�t�@��GPU�A�h���X���󂯎��܂�
		/// </summary>
		/// <param name="HANDLE">�n���h��</param>
		/// <returns>�o�b�t�@��GPU�A�h���X</returns>
		D3D12_GPU_VIRTUAL_ADDRESS GetGpuAddress();

		void *GetMapAddres(int BB_INDEX = -1)const;

		void CopyBuffer(
			const Microsoft::WRL::ComPtr<ID3D12Resource> &SRC_BUFFER,
			D3D12_RESOURCE_STATES BEFORE_STATE,
			D3D12_RESOURCE_STATES AFTER_STATE
		)const
		{
			for (int i = 0; i < buffer.size(); ++i)
			{
				DirectX12CmdList::Instance()->cmdList->ResourceBarrier(
					1,
					&CD3DX12_RESOURCE_BARRIER::Transition(buffer[i].Get(),
						BEFORE_STATE,
						AFTER_STATE
					)
				);

				DirectX12CmdList::Instance()->cmdList->CopyResource(buffer[i].Get(), SRC_BUFFER.Get());

				DirectX12CmdList::Instance()->cmdList->ResourceBarrier(
					1,
					&CD3DX12_RESOURCE_BARRIER::Transition(buffer[i].Get(),
						AFTER_STATE,
						BEFORE_STATE
					)
				);
			}
		}

		void ChangeBarrier(
			D3D12_RESOURCE_STATES BEFORE_STATE,
			D3D12_RESOURCE_STATES AFTER_STATE
		)
		{
			for (int i = 0; i < buffer.size(); ++i)
			{
				DirectX12CmdList::Instance()->cmdList->ResourceBarrier(
					1,
					&CD3DX12_RESOURCE_BARRIER::Transition(buffer[i].Get(),
						BEFORE_STATE,
						AFTER_STATE
					)
				);
			}
		}


		const Microsoft::WRL::ComPtr<ID3D12Resource> &GetBuffer(int INDEX = -1) const
		{
			if (INDEX == -1)
			{
				return buffer[GetIndex()];
			}
			else
			{
				return buffer[INDEX];
			}
		}

		void operator=(const ID3D12ResourceWrapper &rhs)
		{
			for (int i = 0; i < buffer.size(); ++i)
			{
				rhs.buffer[i].CopyTo(&buffer[i]);
			}
		};

		void CreateViewHandle(std::vector<RESOURCE_HANDLE>HANDLE_ARRAY)
		{
			viewHandle = HANDLE_ARRAY;
		}
		void CreateViewHandle(RESOURCE_HANDLE HANDLE)
		{
			viewHandle.emplace_back(HANDLE);
		}
		const RESOURCE_HANDLE &GetViewHandle()const
		{
			return viewHandle[0];
		}

	private:
		std::array<Microsoft::WRL::ComPtr<ID3D12Resource>, 1>buffer;
		std::vector<RESOURCE_HANDLE> viewHandle;
		UINT GetIndex()const
		{
			return 0;
		}
	};



	struct BufferData
	{
		std::shared_ptr<ID3D12ResourceWrapper> bufferWrapper;
		std::shared_ptr<ID3D12ResourceWrapper> counterWrapper;
		GraphicsRangeType rangeType;
		GraphicsRootParamType rootParamType;
		UINT bufferSize;
		UINT elementNum;

		BufferData(const KazBufferHelper::BufferResourceData &BUFFER_DATA) :rangeType(GRAPHICS_RANGE_TYPE_NONE), rootParamType(GRAPHICS_PRAMTYPE_NONE), bufferSize(0), elementNum(0)
		{
			bufferWrapper = std::make_shared<ID3D12ResourceWrapper>();
			bufferWrapper->CreateBuffer(BUFFER_DATA);
		}
		BufferData() :rangeType(GRAPHICS_RANGE_TYPE_NONE), rootParamType(GRAPHICS_PRAMTYPE_NONE), bufferSize(0), elementNum(0)
		{
		}

		void operator=(const BufferData &rhs)
		{
			rangeType = rhs.rangeType;
			rootParamType = rhs.rootParamType;
			bufferSize = rhs.bufferSize;
			bufferWrapper = rhs.bufferWrapper;
			counterWrapper = rhs.counterWrapper;
			elementNum = rhs.elementNum;
		};
	private:
	};


	/// <summary>
	/// �萔�o�b�t�@�𐶐�����ۂɕK�v�Ȑݒ���ȈՂɓZ�߂���
	/// </summary>
	/// <param name="BUFFER_SIZE">�萔�o�b�t�@�̃T�C�Y</param>
	/// <returns>�萔�o�b�t�@�̐����ɕK�v�Ȑݒ�</returns>
	KazBufferHelper::BufferResourceData SetConstBufferData(const unsigned int &BUFFER_SIZE, const std::string &BUFFER_NAME = "ConstBuffer");

	/// <summary>
	/// �V�F�[�_�[���\�[�X�o�b�t�@�𐶐�����ۂɕK�v�Ȑݒ���ȈՂɓZ�߂���
	/// <param name="TEXTURE_DATA">�ǂݍ��񂾃e�N�X�`���̐ݒ�</param>
	/// <returns>�V�F�[�_�[���\�[�X�o�b�t�@�̐����ɕK�v�Ȑݒ�</returns>
	KazBufferHelper::BufferResourceData SetShaderResourceBufferData(const D3D12_RESOURCE_DESC &TEXTURE_DATA, const std::string &BUFFER_NAME = "ShaderResourceBuffer");


	KazBufferHelper::BufferResourceData SetVertexBufferData(BUFFER_SIZE BUFFER_SIZE, const std::string &BUFFER_NAME = "VertexBuffer");


	KazBufferHelper::BufferResourceData SetIndexBufferData(BUFFER_SIZE BUFFER_SIZE, const std::string &BUFFER_NAME = "IndexBuffer");


	KazBufferHelper::BufferResourceData SetRenderTargetData(const D3D12_HEAP_PROPERTIES &HEAP_PROPERTIES, const D3D12_RESOURCE_DESC &RESOURCE, D3D12_CLEAR_VALUE *CLEAR_COLOR, const std::string &BUFFER_NAME = "RenderTarget");

	KazBufferHelper::BufferResourceData SetStructureBuffer(BUFFER_SIZE BUFFER_SIZE, const std::string &BUFFER_NAME = "StructureBuffer");

	KazBufferHelper::BufferResourceData SetRWStructuredBuffer(BUFFER_SIZE BUFFER_SIZE, const std::string &BUFFER_NAME = "RWStructureBuffer");

	KazBufferHelper::BufferResourceData SetUAVTexBuffer(int width,int height, const std::string &BUFFER_NAME = "UAVTexture");

	KazBufferHelper::BufferResourceData SetOnlyReadStructuredBuffer(BUFFER_SIZE BUFFER_SIZE, const std::string &BUFFER_NAME = "OnlyReadStructureBuffer");


	KazBufferHelper::BufferResourceData SetCommandBufferData(const unsigned int &BUFFER_SIZE, const std::string &BUFFER_NAME = "CommandBuffer");

	D3D12_VERTEX_BUFFER_VIEW SetVertexBufferView(const D3D12_GPU_VIRTUAL_ADDRESS &GPU_ADDRESS, BUFFER_SIZE BUFFER_SIZE, const unsigned int &ONE_VERTICES_SIZE);

	D3D12_INDEX_BUFFER_VIEW SetIndexBufferView(const D3D12_GPU_VIRTUAL_ADDRESS &GPU_ADDRESS, BUFFER_SIZE BUFFER_SIZE);

	D3D12_UNORDERED_ACCESS_VIEW_DESC SetUnorderedAccessView(BUFFER_SIZE STRUCTURE_BYTE_SIZE, UINT NUM_ELEMENTS);

	KazBufferHelper::BufferResourceData SetGPUBufferData(BUFFER_SIZE BUFFER_SIZE, const std::string &BUFFER_NAME = "IndexBuffer");


	template<typename T>
	T GetBufferSize(size_t BUFFER_SIZE, unsigned long long STRUCTURE_SIZE)
	{
		return static_cast<T>(BUFFER_SIZE * static_cast<int>(STRUCTURE_SIZE));
	};
}

