#include "object3d.hlsli"
//三角形の入力から、点を一つ出力するサンプル
/*
[maxvertexcount(1)]
void main(
	triangle VertexShaderOutput input[3], 
	//点ストリーム
	inout PointStream< GSOutput > output
)
{
	
	{
		GSOutput element;
		element.position = input[0].position;
        element.normal = input[0].normal;
        element.texcord = input[0].texcord;
        element.worldPosition = input[0].worldPosition;
		output.Append(element);
	}
}
*/

//三角形の入力から、点を3つ出力するサンプル
/*
[maxvertexcount(3)]
void main(
	triangle VertexShaderOutput input[3], 
	//点ストリーム
	inout PointStream< GSOutput > output
)
{
    for (uint i = 0; i < 3; i++)
    {
        GSOutput element;
        element.position = input[i].position;
        element.normal = input[i].normal;
        element.texcord = input[i].texcord;
        element.worldPosition = input[i].worldPosition;
        output.Append(element);
    }
}
*/

//三角形の入力から、線分を1つ出力するサンプル
/*
[maxvertexcount(2)]
void main(
	triangle VertexShaderOutput input[3],
	//線分ストリーム
	inout LineStream<GSOutput> output
)
{
    
    GSOutput element;
    
    //線分の始点
    element.position = input[0].position;
    element.normal = input[0].normal;
    element.texcord = input[0].texcord;
    element.worldPosition = input[0].worldPosition;
    output.Append(element);
    
    //線分の終点
    element.position = input[1].position;
    element.normal = input[1].normal;
    element.texcord = input[1].texcord;
    element.worldPosition = input[1].worldPosition;
    output.Append(element);
}
*/

//三角形の入力から、線分を3つ出力するサンプル
/*
[maxvertexcount(6)]
void main(
	triangle VertexShaderOutput input[3],
	//線分ストリーム
	inout LineStream<GSOutput> output
)
{
    
    GSOutput element;
    //三角形の頂点1つずつ扱う
    for (uint i = 0; i < 3; i++)
    {
        
        //線分の始点
        element.position = input[i].position;
        element.normal = input[i].normal;
        element.texcord = input[i].texcord;
        element.worldPosition = input[i].worldPosition;
        output.Append(element);
        
        //線分の終点
        if (i == 2)
        {
            // + 1すると溢れるので、最初に戻る
            element.position = input[0].position;
            element.normal = input[0].normal;
            element.texcord = input[0].texcord;
            element.worldPosition = input[0].worldPosition;
        }
        else
        {
            element.position = input[i + 1].position;
            element.normal = input[i + 1].normal;
            element.texcord = input[i + 1].texcord;
            element.worldPosition = input[i + 1].worldPosition;
        }
        output.Append(element);
        //現在のストリップを終了し、次のストリップを開始
        output.RestartStrip();

    }
    
}
*/

//三角形の入力から、線分を3つ出力するサンプル
/*
[maxvertexcount(4)]
void main(
	triangle VertexShaderOutput input[3],
	//線分ストリーム
	inout LineStream<GSOutput> output
)
{
    
    GSOutput element;
    //三角形の頂点1つずつ扱う
    for (uint i = 0; i < 3; i++)
    {
        
        element.position = input[i].position;
        element.normal = input[i].normal;
        element.texcord = input[i].texcord;
        element.worldPosition = input[i].worldPosition;
        output.Append(element);
        
    }
    
    //最初の点をもう一度追加
    element.position = input[0].position;
    element.normal = input[0].normal;
    element.texcord = input[0].texcord;
    element.worldPosition = input[0].worldPosition;
    output.Append(element);
    
}
*/

//三角形の入力から、三角形を出力するが、
//途中でUVを加工してタイリング
/*
[maxvertexcount(3)]
void main(
	triangle VertexShaderOutput input[3],
	inout TriangleStream<GSOutput> output
)
{
    
    GSOutput element;
    //三角形の頂点1つずつ扱う
    for (uint i = 0; i < 3; i++)
    {
        
        element.position = input[i].position;
        element.normal = input[i].normal;
        //UVを2倍に
        element.texcord = input[i].texcord * 2.0f;
        element.worldPosition = input[i].worldPosition;
        output.Append(element);
        
    }
}
*/

//三角形の入力から、三角形を2つ出力
/*
[maxvertexcount(6)]
void main(
	triangle VertexShaderOutput input[3],
	inout TriangleStream<GSOutput> output
)
{
    
    GSOutput element;
    //1つ目の三角形
    for (uint i = 0; i < 3; i++)
    {
        
        element.position = input[i].position;
        element.normal = input[i].normal;
        element.texcord = input[i].texcord;
        element.worldPosition = input[i].worldPosition;
        output.Append(element);
        
    }
    //現在のストリップを終了
    output.RestartStrip();
    //2つ目の三角形
    for (uint j = 0; j < 3; j++)
    {
        //X方向に2ずらす
        element.position = input[j].position + float4(10.0f, 0.0f, 0.0f, 0.0f);
        element.normal = input[j].normal;
        //UVを5倍に
        element.texcord = input[j].texcord * 5.0f;
        element.worldPosition = input[j].worldPosition;
        output.Append(element);
        
    }
    
}
*/

//点の入力から、点を出力
/*
[maxvertexcount(1)]
void main(
	point VertexShaderOutput input[1],
	inout PointStream<GSOutput> output
)
{
    
    GSOutput element;
    
    element.position = input[0].position;
    element.normal = input[0].normal;
    element.texcord = input[0].texcord;
    element.worldPosition = input[0].worldPosition;
    output.Append(element);
    
}
*/

//点の入力から、三角形を出力
/*
[maxvertexcount(3)]
void main(
	point VertexShaderOutput input[1],
	inout TriangleStream<GSOutput> output
)
{
    
    GSOutput element;
    
    //共通
    element.normal = input[0].normal;
    element.texcord = input[0].texcord;
    //1点目
    element.position = input[0].position;
    element.worldPosition = input[0].worldPosition;
    output.Append(element);
    //2点目
    element.position = input[0].position + float4(10.0f, 10.0f, 0.0f, 0.0f);
    element.worldPosition = input[0].worldPosition + float3(10.0f, 10.0f, 0.0f);
    output.Append(element);
    //3点目
    element.position = input[0].position + float4(10.0f, 0.0f, 0.0f, 0.0f);
    element.worldPosition = input[0].worldPosition + float3(10.0f, 0.0f, 0.0f);
    output.Append(element);
    
}
*/

//四角形の頂点数
static const uint vnum = 4;

//センターからのオフセット
static const float4 offset_array[vnum] =
{
    float4(-0.5f, -0.5f, 0.0f, 0.0f), //左下
    float4(-0.5f, 0.5f, 0.0f, 0.0f), //左上
    float4(0.5f, -0.5f, 0.0f, 0.0f), //右下
    float4(0.5f, 0.5f, 0.0f, 0.0f), //右上
};

//左上が0,0 右下が1,1
static const float2 uv_array[vnum] =
{
    float2(0.0f, 1.0f), //左下
    float2(0.0f,0.0f), //左上
    float2(1.0f,1.0f), //右下
    float2(1.0f, 0.0f), //右上
};

//点の入力から、四角形を出力
[maxvertexcount(vnum)]
void main(
	point VertexShaderOutput input[1],
	inout TriangleStream<GSOutput> output
)
{
    
    GSOutput element;
    
    //共通
    element.normal = input[0].normal;
    
    //4点分まわす
    for (uint i = 0; i < vnum; i++)
    {
        
        //ワールド座標ベースで、ずらす
        element.position = input[0].position + offset_array[i];
        element.position = mul(element.position, WVP);
        element.worldPosition = input[0].worldPosition + offset_array[i].xyz;
        //element.worldPosition = mul(offset_array[i], input[i].World).xyz;
        element.texcord = uv_array[i];
    
        output.Append(element);
    
    }

    
}