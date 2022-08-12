#pragma once
#include <DirectXMath.h>
#include <string>

using namespace DirectX;

//  Shader側に投げられるMaterialデータ
struct MaterialForHlsl
{
    XMFLOAT3 diffuse;     //  ディフューズ色
    float alpha;          //  ディフューズα
    XMFLOAT3 specular;    //  スペキュラ色
    float specularity;    //  スペキュラの強さ（乗算値）
    XMFLOAT3 ambient;     //  アンビエント色
};
//  それ以外のMaterialデータ
struct AdditionalMaterial
{
    std::string texPath;   //  テクスチャファイルパス
    int toonIdx;           //  トゥーン番号
    bool edgeFlg;          //  マテリアル毎の輪郭線フラグ
};
//  上記2つの構造体を含めて、Materialの情報をまとめた構造体
struct Material
{
    unsigned int indicesNum;    //  インデックス数
    MaterialForHlsl material;
    AdditionalMaterial additional;
};