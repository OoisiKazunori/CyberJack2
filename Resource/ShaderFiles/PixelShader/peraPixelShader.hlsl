#include"../ShaderHeader/peraHeader.hlsli"

//float WhiteNoise(float2 coord) {
//	return frac(sin(dot(coord, float2(8.7819, 3.255))) * 437.645);
//}


float4 PSmain(Output input) : SV_TARGET
{

	/*float2 samplePoint = input.uv;
	float4 Tex = tex.Sample(smp, input.uv);
	float noise = WhiteNoise(input.uv * Time) - 0.5;
	Tex.rgb += float3(noise, noise, noise);
	return Tex;*/


	float w, h, levels;
	tex.GetDimensions(0, w, h, levels);

	float dx = 1.0f / w;
	float dy = 1.0f / h;
	float blur = 15;
	float4 ret = float4(0, 0, 0, 0);
	float4 col = tex.Sample(smp,input.uv);

	return tex.Sample(smp, input.uv);
	//return float4(input.uv, 1, 1);

	//モノクロ化
	//RGB極ぶりだと上手くモノクロに出来ない

	//float y = dot(col.rgb, float3(0.299, 0.587, 0.114));
	//return float4(y, y, y, 1);

	//色の反転
	//return float4(1.0f - col.rgb, col.a);

	//色の階調を落とす
	//float4 down = float4(col.rgb - fmod(col.rgb, 0.55f), col.a);
	//return down;

	//ぼかし処理



	//ret += tex.Sample(smp, input.uv + float2(-blur * dx, -blur * dy));
	//ret += tex.Sample(smp, input.uv + float2(0, -blur * dy));
	//ret += tex.Sample(smp, input.uv + float2(blur * dx, -blur * dy));

	//ret += tex.Sample(smp, input.uv + float2(-blur * dx, 0));
	//ret += tex.Sample(smp, input.uv);
	//ret += tex.Sample(smp, input.uv + float2(blur * dx, 0));

	//ret += tex.Sample(smp, input.uv + float2(-blur * dx, blur * dy));
	//ret += tex.Sample(smp, input.uv + float2(0, blur * dy));
	//ret += tex.Sample(smp, input.uv + float2(blur * dx, blur * dy));

	//return ret / 9.0f;



	////エンボス加工
	//float emboss = 2;
	//ret += tex.Sample(smp, input.uv + float2(-emboss * dx, -emboss * dy)) * 2;
	//ret += tex.Sample(smp, input.uv + float2(0, -emboss * dy));
	//ret += tex.Sample(smp, input.uv + float2(emboss * dx, -emboss * dy)) * 0;

	//ret += tex.Sample(smp, input.uv + float2(-emboss * dx, 0));
	//ret += tex.Sample(smp, input.uv);
	//ret += tex.Sample(smp, input.uv + float2(emboss * dx, 0)) * -1;

	//ret += tex.Sample(smp, input.uv + float2(-emboss * dx, emboss * dy)) * 0;
	//ret += tex.Sample(smp, input.uv + float2(0, emboss * dy)) * -1;
	//ret += tex.Sample(smp, input.uv + float2(emboss * dx, emboss * dy)) * -2;

	////モノクロ化してそれっぽく見せる
	//float y = dot(ret.rgb, float3(0.299, 0.587, 0.114));
	//return float4(y, y, y, 1);



	//シャープネス(エッジの強調)
	//float shapeness = 10;
	//ret += tex.Sample(smp, input.uv + float2(-shapeness * dx, -shapeness * dy)) * 0;
	//ret += tex.Sample(smp, input.uv + float2(0, -shapeness * dy)) * -1;
	//ret += tex.Sample(smp, input.uv + float2(shapeness * dx, -shapeness * dy)) * 0;

	//ret += tex.Sample(smp, input.uv + float2(-shapeness * dx, 0)) * -1;
	//ret += tex.Sample(smp, input.uv) * 5;
	//ret += tex.Sample(smp, input.uv + float2(shapeness * dx, 0)) * -1;

	//ret += tex.Sample(smp, input.uv + float2(-shapeness * dx, shapeness * dy)) * 0;
	//ret += tex.Sample(smp, input.uv + float2(0, shapeness * dy)) * -1;
	//ret += tex.Sample(smp, input.uv + float2(shapeness * dx, shapeness * dy)) * 0;

	//return ret;



	//float shapeness = 10;
	//ret += tex.Sample(smp, input.uv + float2(0, -2 * dy)) * -1;
	//ret += tex.Sample(smp, input.uv + float2(-2 * dx, 0)) * -1;
	//ret += tex.Sample(smp, input.uv) * 4;
	//ret += tex.Sample(smp, input.uv + float2(2 * dx, 0)) * -1;
	//ret += tex.Sample(smp, input.uv + float2(0, 2 * dy)) * -1;

	//float y = dot(ret.rgb, float3(0.299, 0.587, 0.114));
	////エッジの強調
	//y = pow(1.0f - y, 10.0f);

	////余計な輪郭線を消してくれる
	////第一引数が第二引数より小さければ1,それ以外なら0とする
	////y = step(0.2, y);

	//return float4(y, y, y, col.a);



	////ガウシアンぼかし(簡易版)
	////最上段
	//ret += tex.Sample(smp, input.uv + float2(-2 * dx, 2 * dy)) * 1 / 256;
	//ret += tex.Sample(smp, input.uv + float2(-1 * dx, 2 * dy)) * 4 / 256;
	//ret += tex.Sample(smp, input.uv + float2( 0 * dx, 2 * dy)) * 6 / 256;
	//ret += tex.Sample(smp, input.uv + float2( 1 * dx, 2 * dy)) * 4 / 256;
	//ret += tex.Sample(smp, input.uv + float2( 2 * dx, 2 * dy)) * 1 / 256;

	////一つ上段
	//ret += tex.Sample(smp, input.uv + float2(-2 * dx, 1 * dy)) * 4 / 256;
	//ret += tex.Sample(smp, input.uv + float2(-1 * dx, 1 * dy)) * 16 / 256;
	//ret += tex.Sample(smp, input.uv + float2( 0 * dx, 1 * dy)) * 24 / 256;
	//ret += tex.Sample(smp, input.uv + float2( 1 * dx, 1 * dy)) * 16 / 256;
	//ret += tex.Sample(smp, input.uv + float2( 2 * dx, 1 * dy)) * 4 / 256;

	////中段
	//ret += tex.Sample(smp, input.uv + float2(-2 * dx, 0 * dy)) * 6 / 256;
	//ret += tex.Sample(smp, input.uv + float2(-1 * dx, 0 * dy)) * 24 / 256;
	//ret += tex.Sample(smp, input.uv + float2( 0 * dx, 0 * dy)) * 36 / 256;
	//ret += tex.Sample(smp, input.uv + float2( 1 * dx, 0 * dy)) * 24 / 256;
	//ret += tex.Sample(smp, input.uv + float2( 2 * dx, 0 * dy)) * 6 / 256;

	////一つ下段
	//ret += tex.Sample(smp, input.uv + float2(-2 * dx, -1 * dy)) * 4 / 256;
	//ret += tex.Sample(smp, input.uv + float2(-1 * dx, -1 * dy)) * 16 / 256;
	//ret += tex.Sample(smp, input.uv + float2( 0 * dx, -1 * dy)) * 24 / 256;
	//ret += tex.Sample(smp, input.uv + float2( 1 * dx, -1 * dy)) * 16 / 256;
	//ret += tex.Sample(smp, input.uv + float2( 2 * dx, -1 * dy)) * 4 / 256;

	////最下段
	//ret += tex.Sample(smp, input.uv + float2(-2 * dx, -2 * dy)) * 1 / 256;
	//ret += tex.Sample(smp, input.uv + float2(-1 * dx, -2 * dy)) * 4 / 256;
	//ret += tex.Sample(smp, input.uv + float2( 0 * dx, -2 * dy)) * 6 / 256;
	//ret += tex.Sample(smp, input.uv + float2( 1 * dx, -2 * dy)) * 4 / 256;
	//ret += tex.Sample(smp, input.uv + float2( 2 * dx, -2 * dy)) * 1 / 256;

	//return ret;


	//ガウシアンぼかし(横)
	//ret += bkweights[0] * col;

	for (int i = 1; i < 8; ++i)
	{
		ret += bkweights[i >> 2][i % 4] * tex.Sample(smp, input.uv + float2(dx * i, 0));
		ret += bkweights[i >> 2][i % 4] * tex.Sample(smp, input.uv + float2(dx * -i,0));
	}

	//return float4(ret.rgb, col.a);



	//歪み
	/*float2 nmTex = effectTex.Sample(smp, input.uv).xy;
	nmTex = nmTex * 2.0f - 1.0f;

	return tex.Sample(smp, input.uv + nmTex * 0.1f);*/
}

float4 VerticalBokehPS(Output input) : SV_TARGET
{
	float w, h, levels;
	tex.GetDimensions(0, w, h, levels);




	float dx = 1.0f / w;
	float dy = 1.0f / h;
	float4 ret = float4(0, 0, 0, 0);
	float4 col = tex.Sample(smp,input.uv);


	ret += bkweights[0] * col;

	for (int i = 1; i < 8; ++i)
	{
		ret += bkweights[i >> 2][i % 4] * tex.Sample(smp, input.uv + float2(0, dy * i));

		ret += bkweights[i >> 2][i % 4] * tex.Sample(smp, input.uv + float2(0, dy * -i));
	}

	return float4(ret.rgb, col.a);
	return col;

}