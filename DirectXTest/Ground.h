#pragma once
#include <DirectXMath.h>

using namespace DirectX;

//  地面の描画を行うクラス
class Ground
{
public:
    Ground();
    ~Ground();

    //  初期化
    void Init();

    //  描画
    void DrawGround();
private:
    //  モデルハンドル
    int m_groundModelHandle;
    int m_backGroundModelHandle;

    //  地面の座標
    XMFLOAT3 m_groundPos;

    //  構造体へのポインタ
    struct MapData* m_mapData;  //  マップのデータ

    //  シングルトンクラスへの参照
    class PMDModel& m_model;    //  PMDモデルの管理
};