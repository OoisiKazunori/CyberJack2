#include "LensFlare.h"
#include "Buffer/GBufferMgr.h"

PostEffect::LensFlare::LensFlare()
{

	/*===== �R���X�g���N�^ =====*/

}

void PostEffect::LensFlare::Setting()
{

	/*===== �������� =====*/

	//�P�x���o�p�V�F�[�_�[
	{

		std::vector<KazBufferHelper::BufferData>extraBuffer =
		{
			 GBufferMgr::Instance()->GetRaytracingBuffer(),
			 m_noiseParamData
		};
		extraBuffer[0].rangeType = GRAPHICS_RANGE_TYPE_UAV_DESC;
		extraBuffer[0].rootParamType = GRAPHICS_PRAMTYPE_TEX;
		extraBuffer[1].rangeType = GRAPHICS_RANGE_TYPE_CBV_VIEW;
		extraBuffer[1].rootParamType = GRAPHICS_PRAMTYPE_DATA;
		//m_luminanceShader.Generate(ShaderOptionData(KazFilePathName::RelativeShaderPath + "Raytracing/" + "Write3DNoise.hlsl", "CSmain", "cs_6_4", SHADER_TYPE_COMPUTE), extraBuffer);

	}

}
