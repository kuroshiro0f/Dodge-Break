#include "Ground.h"

#include "StageData.h"
#include "PMDModelType.h"

#include "Singleton.h"
#include "PMDModel.h"
#include "LoadJson.h"

Ground::Ground()
    :m_model(Singleton<PMDModel>::GetInstance())
    , m_mapData(new MapData())
{
}

Ground::~Ground()
{
    //  メモリの解放
    delete m_mapData;
}

void Ground::Init()
{
    //  外部ファイルのデータを取得
    LoadJson& fileData = Singleton<LoadJson>::GetInstance();
    //  取得したデータを元に地面の座標を決定
    m_groundPos = fileData.GetXMFLOAT3Data(JsonDataType::Stage, "CenterPos");
}

//  描画
void Ground::DrawGround()
{
    //  地面の描画
    m_model.Draw(m_groundPos, PMDModelType::Ground);
    //  背景の描画
    m_model.Draw({ m_groundPos.x,m_groundPos.y - 1.0f,m_groundPos.z }, PMDModelType::BackGround);
}