#pragma once
#include"../KazLibrary/Render/KazRender.h"
#include"../Game/Interface/IEnemy.h"
#include"../Game/Effect/RocketEffect.h"

class VirusEnemy:public IEnemy
{
public:
	VirusEnemy();

	void Init(const EnemyGenerateData& GENERATE_DATA, bool DEMO_FLAG);
	void Finalize();
	void Update();
	void Draw(DrawingByRasterize& arg_rasterize);

private:
};

