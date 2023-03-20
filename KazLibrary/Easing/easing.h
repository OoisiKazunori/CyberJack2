#pragma once
#include<math.h>

enum EasingType 
{
	In, Out, InOut
};

enum EaseInType
{
	Linear,Smooth,Quad,Cubic,Quart,Quint,Sine,Circ,Exp,Back,SoftBack,Elastic,Bounce
};

//EasingType...In,Out,InOut、EaseInType...InLinear,InSmooth...、t...変化率

/// <summary>
/// 始まり方と種類を合わせてイージングのグラフを作り、変化率を入れて値を返します
/// </summary>
/// <param name="ChooseType">イージングの始まり方</param>
/// <param name="ChooseEase">イージングの種類</param>
/// <param name="t">変化率</param>
/// <returns></returns>
float EasingMaker(int ChooseType, int ChooseEase, float t);

//EaseInの種類

float InLinear(float t);
float InSmooth(float t);
float InQuad(float t);
float InCubic(float t);
float InQuart(float t);
float InQuint(float t);
float InSine(float t);
float InCirc(float t);
float InExp(float t);
float InBack(float t);
float InSoftBack(float t);
float InElastic(float t);
float InBounce(float t);
float Max(float a, float b);

/// <summary>
/// 変化率を計算します
/// </summary>
/// <param name="*t">変化率を入れる変数</param>
/// <param name="rate">1Flameにどれくらい増加させるか</param>
/// <param name="maxRate">最大</param>
/// <returns></returns>
void Rate(float *T, float RATE, float MAX_RATE);