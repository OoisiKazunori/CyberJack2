#include"UavViewHandleMgr.h"
#include"DescriptorHeapMgr.h"

UavViewHandleMgr::UavViewHandleMgr()
{

}

void UavViewHandleMgr::Init()
{
	handle.SetHandleSize(DescriptorHeapMgr::Instance()->GetSize(DESCRIPTORHEAP_MEMORY_TEXTURE_COMPUTEBUFFER));
}

RESOURCE_HANDLE UavViewHandleMgr::GetHandle()
{
	return handle.GetHandle();
}