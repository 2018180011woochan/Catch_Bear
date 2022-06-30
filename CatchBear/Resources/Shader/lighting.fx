#ifndef _LIGHTING_FX_
#define _LIGHTING_FX_

#include "params.fx"
#include "utils.fx"

struct VS_IN
{
    float3 pos : POSITION;
    float2 uv : TEXCOORD;
};

struct VS_OUT
{
    float4 pos : SV_Position;
    float2 uv : TEXCOORD;
};

struct PS_OUT
{
    // diffuse, specular light에 대한 텍스처를 추가로 만들거라서 중간 결과물을 저장
    float4 diffuse : SV_Target0;
    float4 specular : SV_Target1;
};

// [Directional Light]
// g_int_0 : Light index        내가 몇번째 빛에 해당하는지
// g_tex_0 : Position RT
// g_tex_1 : Normal RT
// g_tex_2 : Shadow RT          shadow shader에서 넘겨받음
// g_mat_0 : ShadowCamera VP
// Mesh : Rectangle             Directional Light의 영역을 표시하는 메쉬

VS_OUT VS_DirLight(VS_IN input)
{
    VS_OUT output = (VS_OUT)0;

    // Resources::LoadRectangleMesh()에서 크기를 0.5씩 잡음, 2를 곱해줌 -> 전체화면 영역 [-1, 1]
    output.pos = float4(input.pos * 2.f, 1.f);
    output.uv = input.uv;

    return output;
}

//PS_OUT PS_DirLight(VS_OUT input)
//{
//    PS_OUT output = (PS_OUT)0;
//
//    // 0번 정보에 uv 좌표를 이용해서 해당 지점에 물체가 있는지 없는지 체크, 뷰 포지션을 꺼내옴
//    float3 viewPos = g_tex_0.Sample(g_sam_0, input.uv).xyz;
//    if (viewPos.z <= 0.f)   // 물체가 없었다면 z값이 음수로 나옴, 스킵해줌
//        clip(-1);
//
//    float3 viewNormal = g_tex_1.Sample(g_sam_0, input.uv).xyz;
//
//    LightColor color = CalculateLightColor(g_int_0, viewNormal, viewPos);
//
//    // 그림자
//    if (length(color.diffuse) != 0)
//    {
//        matrix shadowCameraVP = g_mat_0;
//
//        float4 worldPos = mul(float4(viewPos.xyz, 1.f), g_matViewInv);  // view의 역행렬, 해당 물체의 world 포지션이 나옴
//        float4 shadowClipPos = mul(worldPos, shadowCameraVP);   // 빨간 카메라를 기준으로 한 클립 포지션이 완성됨
//        float depth = shadowClipPos.z / shadowClipPos.w;    // 우리가 그리려고 하는 해당 지점의 depth
//
//        // x [-1 ~ 1] -> u [0 ~ 1]
//        // y [1 ~ -1] -> v [0 ~ 1]
//        float2 uv = shadowClipPos.xy / shadowClipPos.w;
//        uv.y = -uv.y;
//        uv = uv * 0.5 + 0.5;
//
//        if (0 < uv.x && uv.x < 1 && 0 < uv.y && uv.y < 1)
//        {
//            float shadowDepth = g_tex_2.Sample(g_sam_0, uv).x;
//            if (shadowDepth > 0 && depth > shadowDepth + 0.00001f)
//            {
//                color.diffuse *= 0.5f;
//                color.specular = (float4) 0.f;
//            }
//        }
//    }
//
//
//
//    output.diffuse = color.diffuse + color.ambient;
//    output.specular = color.specular;
//
//    return output;
//}


PS_OUT PS_DirLight(VS_OUT input)
{
    PS_OUT output = (PS_OUT)0;

    // 0번 정보에 uv 좌표를 이용해서 해당 지점에 물체가 있는지 없는지 체크, 뷰 포지션을 꺼내옴
    float3 viewPos = g_tex_0.Sample(g_sam_0, input.uv).xyz;
    //if (viewPos.z <= 0.f)   // 물체가 없었다면 z값이 음수로 나옴, 스킵해줌
    //    clip(-1);

    float3 viewNormal = g_tex_1.Sample(g_sam_0, input.uv).xyz;

    LightColor color = CalculateLightColor(g_int_0, viewNormal, viewPos);

    // 그림자
    if (length(color.diffuse) != 0)
    {
        matrix shadowCameraVP = g_mat_0;

        float4 worldPos = mul(float4(viewPos.xyz, 1.f), g_matViewInv);  // view의 역행렬, 해당 물체의 world 포지션이 나옴
        float4 shadowClipPos = mul(worldPos, shadowCameraVP);   // 빨간 카메라를 기준으로 한 클립 포지션이 완성됨
        float depth = shadowClipPos.z / shadowClipPos.w;    // 우리가 그리려고 하는 해당 지점의 depth

        // x [-1 ~ 1] -> u [0 ~ 1]
        // y [1 ~ -1] -> v [0 ~ 1]
        float2 uv = shadowClipPos.xy / shadowClipPos.w;
        uv.y = -uv.y;
        uv = uv * 0.5 + 0.5;

        if (0 < uv.x && uv.x < 1 && 0 < uv.y && uv.y < 1)
        {
            float shadowDepth = g_tex_2.Sample(g_sam_0, uv).x;
            if (shadowDepth > 0 && depth > shadowDepth + 0.00001f)
            {
                color.diffuse *= 0.5f;
                color.specular = (float4) 0.f;
            }
        }
    }



    output.diffuse = color.diffuse + color.ambient;
    output.specular = color.specular;

    return output;

}


// [Point Light]
// g_int_0 : Light index
// g_tex_0 : Position RT
// g_tex_1 : Normal RT
// g_vec2_0 : RenderTarget Resolution   // RT의 해상도 (800*600)
// Mesh : Sphere

VS_OUT VS_PointLight(VS_IN input)
{
    VS_OUT output = (VS_OUT)0;

    output.pos = mul(float4(input.pos, 1.f), g_matWVP);
    output.uv = input.uv;

    return output;
}

PS_OUT PS_PointLight(VS_OUT input)
{
    PS_OUT output = (PS_OUT)0;

    // input.pos = SV_Position = Screen 좌표
    float2 uv = float2(input.pos.x / g_vec2_0.x, input.pos.y / g_vec2_0.y);
    float3 viewPos = g_tex_0.Sample(g_sam_0, uv).xyz;   // 물체가 있는지 없는지 체크
    if (viewPos.z <= 0.f)
        clip(-1);

    // point light의 구 영역 안에 물체가 있는지 체크하기 위해서 둘 사이의 거리 체크
    int lightIndex = g_int_0;
    float3 viewLightPos = mul(float4(g_light[lightIndex].position.xyz, 1.f), g_matView).xyz;
    float distance = length(viewPos - viewLightPos);
    if (distance > g_light[lightIndex].range)
        clip(-1);

    float3 viewNormal = g_tex_1.Sample(g_sam_0, uv).xyz;

    LightColor color = CalculateLightColor(g_int_0, viewNormal, viewPos);

    output.diffuse = color.diffuse + color.ambient;
    output.specular = color.specular;

    return output;
}

// 지금까지 만들어놓은 결과물을 조립해서 인게임에 보일 화면을 만들어주는 셰이더
// [Final]
// g_tex_0 : Diffuse Color Target
// g_tex_1 : Diffuse Light Target
// g_tex_2 : Specular Light Target
// Mesh : Rectangle     전체화면에 그릴거임, rectangle로 받아줌

VS_OUT VS_Final(VS_IN input)
{
    VS_OUT output = (VS_OUT)0;

    output.pos = float4(input.pos * 2.f, 1.f);
    output.uv = input.uv;

    return output;
}

float4 PS_Final(VS_OUT input) : SV_Target
{
    float4 output = (float4)0;

    float4 lightPower = g_tex_1.Sample(g_sam_0, input.uv);
    if (lightPower.x == 0.f && lightPower.y == 0.f && lightPower.z == 0.f)
        clip(-1);

    float4 color = g_tex_0.Sample(g_sam_0, input.uv);
    float4 specular = g_tex_2.Sample(g_sam_0, input.uv);

    output = (color * lightPower) + specular;
    return output;
}


#endif