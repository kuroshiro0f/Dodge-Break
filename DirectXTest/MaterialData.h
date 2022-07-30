#pragma once
#include <DirectXMath.h>
#include <string>

using namespace DirectX;

//  シェーダー側に投げられるマテリアルデータ
struct MaterialForHlsl
{
    XMFLOAT3 diffuse;     //  ディフューズ色
    float alpha;          //  ディフューズα
    XMFLOAT3 specular;    //  スペキュラ色
    float specularity;    //  スペキュラの強さ（乗算値）
    XMFLOAT3 ambient;     //  アンビエント色
};
//  それ以外のマテリアルデータ
struct AdditionalMaterial
{
    std::string texPath;   //  テクスチャファイルパス
    int toonIdx;           //  トゥーン番号
    bool edgeFlg;          //  マテリアル毎の輪郭線フラグ
};
//  まとめたもの
struct Material
{
    unsigned int indicesNum;    //  インデックス数
    MaterialForHlsl material;
    AdditionalMaterial additional;
};