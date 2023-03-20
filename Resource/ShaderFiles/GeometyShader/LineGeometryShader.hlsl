#include"../ShaderHeader/BasicShaderHeader.hlsli"

struct GSOutput
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD;
};

//線の太さを変えるシェーダー
[maxvertexcount(6)]
void GSmain(
	line VSOutputLine input[2],
	inout TriangleStream<GSOutput> output
)
{
    
    //0始点,1終点
    
    
    for (int i = 0; i < 2; i++)
    {
        float offset = input[i].tickness.x / 2.0f;

        //複数の点と連動している
        {
            //右上
            GSOutput element;
            element.pos = input[i].svpos + float4(offset, 0.0f, 0.0f, 0.0f);
            element.uv = 0;
            
            output.Append(element);
        }
        {
            //左上...下も上がった?
            GSOutput element;
            element.pos = input[i].svpos + float4(-offset, 0.0f, 0.0f, 0.0f);
            element.uv = 0;
            output.Append(element);
        }
        
            //左下
        if (i == 0)
        {
            GSOutput element;
            element.pos = input[(i + 1) % 2].svpos + float4(offset * sign(i - 1), 0.0f, 0.0f, 0.0f);
            element.uv = 0;
            output.Append(element);
        }
        else
        {
            GSOutput element;
            element.pos = input[(i + 1) % 2].svpos + float4(offset * sign(i - 1) + offset, 0.0f, 0.0f, 0.0f);
            element.uv = 0;
            output.Append(element);
        }
        

        output.RestartStrip();
    }
}