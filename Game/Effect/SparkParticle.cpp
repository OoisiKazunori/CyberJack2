#include "SparkParticle.h"
#include"../KazLibrary/Helper/ResourceFilePass.h"

SparkParticle::SparkParticle()
{
	initFlag = false;
}

void SparkParticle::Init(const KazMath::Vec3<float> &POS, int ANGLE)
{
	initFlag = true;
	std::array<int, 2>lAngle;
	lAngle[0] = ANGLE - 10;
	lAngle[1] = ANGLE + 10;

	for (int i = 0; i < particleR.size(); ++i)
	{
		particleR[i].data.handleData = TextureResourceMgr::Instance()->LoadGraph(KazFilePathName::EffectPath + "Spark.png");
		particleR[i].data.transform.pos = POS;
		particleR[i].data.pipelineName = PIPELINE_NAME_SPRITE_Z_OFF;
		particleR[i].data.transform.scale = { 0.3f,0.3f,0.5f };
		particleR[i].data.transform.rotation.y = 90.0f;
		particleR[i].data.colorData.color.a = 255;
		particleR[i].data.transform.rotation.z = static_cast<float>(90 - lAngle[i]);

		vel[i].z = cosf(KazMath::AngleToRadian(lAngle[i]));
		vel[i].y = sinf(KazMath::AngleToRadian(lAngle[i]));
	}
	timer = 0;
}

void SparkParticle::Update()
{
	if (initFlag)
	{
		for (int i = 0; i < particleR.size(); ++i)
		{
			particleR[i].data.transform.pos += vel[i] * 1.0f;
		}

		++timer;
		if (15 <= timer)
		{
			timer = 0;
			initFlag = false;
		}
	}
}

void SparkParticle::Draw()
{
	if (initFlag)
	{
		for (int i = 0; i < particleR.size(); ++i)
		{
			particleR[i].Draw();
		}
	}
}

bool SparkParticle::IsActive()
{
	return initFlag;
}
