//https://docs.microsoft.com/en-us/windows/win32/api/d3d12/ns-d3d12-d3d12_draw_arguments
struct D3D12_DRAW_ARGUMENTS
{
    uint vertexCountPerInstance;
    uint instanceCount;
    uint startVertexLocation;
    uint startInstanceLocation;
};

//https://docs.microsoft.com/en-us/windows/win32/api/d3d12/ns-d3d12-d3d12_draw_indexed_arguments
struct D3D12_DRAW_INDEXED_ARGUMENTS 
{
   uint indexCountPerInstance;
   uint instanceCount;
   uint startIndexLocation;
   uint baseVertexLocation;
   uint startInstanceLocation;
};