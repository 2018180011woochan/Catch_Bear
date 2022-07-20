#ifndef _DEFAULT_FX_
#define _DEFAULT_FX_

#include "params.fx"

// 기존에는 Transform에 있는걸 PushData()로 넣어줬음.
// Transform 정보는 params의 b1에 넣어줘서 사용했음
// 그런데 한번에 그려주는, 인스턴싱이 들어가는 애들은 b1이 아니라
// 따로 넣어준 VS_IN을 통해 이 부분에 받아줄것이다

struct VS_IN
{
    float3 pos : POSITION;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float4 weight : WEIGHT;
    float4 indices : INDICES;

    // Shader의 CreateGraphicsShader에서 D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA 로 설정해놨음
    // 즉 이 정보들은 인스턴스마다 고유하게 하나씩 배정이 된다.
    // 그렇기에 아래 아이들은 실질적으로 우리가 넘겨준 배열에 따라
    // 0번이면 0번째 정보를 찾을 것이고, 1번이면 1번 정보를 찾을 것임
    // 각각 우리가 넣어준 스트럭처를 찾아서 matWorld, matWV, matWVP 세개가 세팅될것임
    row_major matrix matWorld : W;
    row_major matrix matWV : WV;
    row_major matrix matWVP : WVP;
    uint instanceID : SV_InstanceID;
};

struct VS_OUT
{
    float4 pos : SV_Position;
    float2 uv : TEXCOORD;
    float3 viewPos : POSITION;
    float3 viewNormal : NORMAL;
    float3 viewTangent : TANGENT;
    float3 viewBinormal : BINORMAL;
};

VS_OUT VS_Main(VS_IN input)
{
    VS_OUT output = (VS_OUT)0;



    if (g_int_0 == 1)   // g_int_0을 1로 설정하면 인스턴싱이 적용된다고 받아들임
    {
        // global이 아니라 input을 통해서 input에 내가 몇번째로 받아주고 있는지 설정
        output.pos = mul(float4(input.pos, 1.f), input.matWVP); 
        output.uv = input.uv;

        output.viewPos = mul(float4(input.pos, 1.f), input.matWV).xyz;
        output.viewNormal = normalize(mul(float4(input.normal, 0.f), input.matWV).xyz);
        output.viewTangent = normalize(mul(float4(input.tangent, 0.f), input.matWV).xyz);
        output.viewBinormal = normalize(cross(output.viewTangent, output.viewNormal));
    }
    else    // 그게 아니면 기존 방식으로 렌더링
    {
        output.pos = mul(float4(input.pos, 1.f), g_matWVP);
        output.uv = input.uv;

        output.viewPos = mul(float4(input.pos, 1.f), g_matWV).xyz;
        output.viewNormal = normalize(mul(float4(input.normal, 0.f), g_matWV).xyz);
        output.viewTangent = normalize(mul(float4(input.tangent, 0.f), g_matWV).xyz);
        output.viewBinormal = normalize(cross(output.viewTangent, output.viewNormal));
    }

    return output;
}

VS_OUT VS_Player(VS_IN input)
{
    VS_OUT output = (VS_OUT)0;

    float4x4 mtxVertexToBoneWorld = (float4x4)0.0f;
    for (int i = 0; i < 4; i++)
    {
        mtxVertexToBoneWorld += input.weight[i] * mul(g_offset[input.indices[i]], g_boneTrans[input.indices[i]]);
    }

        output.uv = input.uv;

        output.viewPos = mul(float4(input.pos, 1.f), mtxVertexToBoneWorld).xyz;
        output.viewNormal = normalize(mul(float4(input.normal, 0.f), (float3x3)mtxVertexToBoneWorld).xyz);
        output.viewTangent = normalize(mul(float4(input.tangent, 0.f), (float3x3)mtxVertexToBoneWorld).xyz);
        output.viewBinormal = normalize(cross(output.viewTangent, output.viewNormal).xyz);

        output.pos = mul(float4(output.viewPos, 1.f), g_matWVP);

    return output;
}

struct PS_OUT
{
    float4 position : SV_Target0;
    float4 normal : SV_Target1;
    float4 color : SV_Target2;
};

PS_OUT PS_Main(VS_OUT input)
{
    PS_OUT output = (PS_OUT)0;

    float4 color = float4(1.f, 1.f, 1.f, 1.f);
    if (g_tex_on_0 == 1)
        color = g_tex_0.Sample(g_sam_0, input.uv);

    float3 viewNormal = input.viewNormal;
    if (g_tex_on_1 == 1)
    {
        // [0,255] 범위에서 [0,1]로 변환
        float3 tangentSpaceNormal = g_tex_1.Sample(g_sam_0, input.uv).xyz;
        // [0,1] 범위에서 [-1,1]로 변환
        tangentSpaceNormal = (tangentSpaceNormal - 0.5f) * 2.f;
        float3x3 matTBN = { input.viewTangent, input.viewBinormal, input.viewNormal };
        viewNormal = normalize(mul(tangentSpaceNormal, matTBN));
    }

    output.position = float4(input.pos.xyz, 0.f);
    output.normal = float4(viewNormal.xyz, 0.f);
    output.color = color;
    //output.color = float4(0.f, 0.f, 0.f, 1.f);

    return output;
}

PS_OUT PS_Treasure(VS_OUT input)
{
    PS_OUT output = (PS_OUT)0;

    float4 color = float4(1.f, 1.f, 0.f, 1.f);

    output.position = float4(input.pos.xyz, 0.f);
    output.normal = float4(input.viewNormal.xyz, 0.f);
    output.color = color;

    return output;
}

PS_OUT PS_TagMark(VS_OUT input) // 술래
{
    PS_OUT output = (PS_OUT)0;

    float4 color = float4(1.f, 0.f, 0.f, 1.f);

    output.position = float4(input.pos.xyz, 0.f);
    output.normal = float4(input.viewNormal.xyz, 0.f);
    output.color = color;
    return output;
}

PS_OUT PS_TagMarkNormalPlayer(VS_OUT input) // 술래
{
    PS_OUT output = (PS_OUT)0;

    float4 color = float4(0.f, 0.f, 1.f, 1.f);

    output.position = float4(input.pos.xyz, 0.f);
    output.normal = float4(input.viewNormal.xyz, 0.f);
    output.color = color;

    return output;
}

PS_OUT PS_LeafParticle(VS_OUT input)
{
    PS_OUT output = (PS_OUT)0;

    float4 color = float4(0.4f, 0.8f, 0.4f, 1.f);

    output.position = float4(input.pos.xyz, 0.f);
    output.normal = float4(input.viewNormal.xyz, 0.f);
    output.color = color;

    return output;
}

#endif