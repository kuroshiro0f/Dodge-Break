#include "LoadJson.h"

#include <fstream>
#include <iostream>
#include "rapidjson/istreamwrapper.h"

#include "EnumIterator.h"

#include "Singleton.h"
#include "CheckError.h"

LoadJson::LoadJson()
    :m_jasonDataFilePath({})
{
}

LoadJson::~LoadJson()
{
    //  処理なし
}

//  初期化
void LoadJson::Init()
{
    //  ファイルパス
    m_jasonDataFilePath =
    {
        { JsonDataType::Player,"Data/Param/PlayerData.json" },
        { JsonDataType::Energy,"Data/Param/EnergyData.json" },
        { JsonDataType::Enemy,"Data/Param/EnemyData.json" },
        { JsonDataType::EnemyDefeater,"Data/Param/EnemyDefeaterData.json" },
        { JsonDataType::StraightAttack,"Data/Param/StraightAttackData.json" },
        { JsonDataType::FannelAttack,"Data/Param/FannelAttackData.json" },
        { JsonDataType::ZigzagAttack,"Data/Param/ZigzagAttackData.json" },
        { JsonDataType::SpreadAttack,"Data/Param/SpreadAttackData.json" },
        { JsonDataType::LineAttack,"Data/Param/LineAttackData.json" },
        { JsonDataType::BeamAttack,"Data/Param/BeamAttackData.json" },
        { JsonDataType::Wave,"Data/Param/WaveData.json" },
        { JsonDataType::Stage,"Data/Param/StageData.json" }
    };

    //  ファイルのロード
    Load();
}

//  jsonファイルのロード
void LoadJson::Load()
{
    //  エラーチェック用クラスのシングルトンインスタンスを得る
    CheckError& checkError = Singleton<CheckError>::GetInstance();

    //  列挙型JsonDataTypeのイテレータを作成
    typedef EnumIterator<JsonDataType, JsonDataType::Player, JsonDataType::Stage> typeItr;
    for (auto itr : typeItr())
    {
        //  二重ロードの防止
        for (auto secondItr : typeItr())
        {
            if (itr != secondItr && m_jasonDataFilePath[itr] == m_jasonDataFilePath[secondItr])
            {
                std::string str = "jsonファイル : " + std::to_string(static_cast<int>(itr)) + "は二重にロードされています。";
                checkError.CreateErrorMessage(str);
            }
        }
        std::ifstream ifs(m_jasonDataFilePath[itr]);
        rapidjson::IStreamWrapper isw(ifs);
        m_doc[itr].ParseStream(isw);
    }
}

//  int型のデータを取得
int LoadJson::GetIntData(JsonDataType _type, const char* _string)
{
    int ret;

    ret = m_doc[_type][_string].GetInt();

    return ret;
}
int LoadJson::GetIntData(JsonDataType _type, const char* _string, const int _indexNum)
{
    int ret;

    ret = m_doc[_type][_string][_indexNum].GetInt();

    return ret;
}
int LoadJson::GetIntData(JsonDataType _type, const char* _string, const int _indexNum, const char* _indexString)
{
    int ret;

    ret = m_doc[_type][_string][_indexNum][_indexString].GetInt();

    return ret;
}
int LoadJson::GetIntData(JsonDataType _type, const char* _string, const int _indexNum, const char* _indexString, const int _secondIndexNum)
{
    int ret;

    ret = m_doc[_type][_string][_indexNum][_indexString][_secondIndexNum].GetInt();

    return ret;
}
int LoadJson::GetIntData(JsonDataType _type, const char* _string, const int _indexNum, const char* _indexString, const int _secondIndexNum, const char* _secondIndexString)
{
    int ret;

    ret = m_doc[_type][_string][_indexNum][_indexString][_secondIndexNum][_secondIndexString].GetInt();

    return ret;
}
int LoadJson::GetIntData(JsonDataType _type, const char* _string, const int _indexNum, const char* _indexString, const int _secondIndexNum, const char* _secondIndexString, const int _thirdIndexNum)
{
    int ret;

    ret = m_doc[_type][_string][_indexNum][_indexString][_secondIndexNum][_secondIndexString][_thirdIndexNum].GetInt();

    return ret;
}

//  float型のデータを取得
float LoadJson::GetFloatData(JsonDataType _type, const char* _string)
{
    float ret;

    ret = m_doc[_type][_string].GetFloat();

    return ret;
}
float LoadJson::GetFloatData(JsonDataType _type, const char* _string, const int _indexNum)
{
    float ret;

    ret = m_doc[_type][_string][_indexNum].GetFloat();

    return ret;
}
float LoadJson::GetFloatData(JsonDataType _type, const char* _string, const int _indexNum, const char* _indexString)
{
    float ret;

    ret = m_doc[_type][_string][_indexNum][_indexString].GetFloat();

    return ret;
}
float LoadJson::GetFloatData(JsonDataType _type, const char* _string, const int _indexNum, const char* _indexString, const int _secondIndexNum)
{
    float ret;

    ret = m_doc[_type][_string][_indexNum][_indexString][_secondIndexNum].GetFloat();

    return ret;
}
float LoadJson::GetFloatData(JsonDataType _type, const char* _string, const int _indexNum, const char* _indexString, const int _secondIndexNum, const char* _secondIndexString)
{
    float ret;

    ret = m_doc[_type][_string][_indexNum][_indexString][_secondIndexNum][_secondIndexString].GetFloat();

    return ret;
}
float LoadJson::GetFloatData(JsonDataType _type, const char* _string, const int _indexNum, const char* _indexString, const int _secondIndexNum, const char* _secondIndexString, const int _thirdIndexNum)
{
    float ret;

    ret = m_doc[_type][_string][_indexNum][_indexString][_secondIndexNum][_secondIndexString][_thirdIndexNum].GetFloat();

    return ret;
}

//  string型のデータを取得
const char* LoadJson::GetStringData(JsonDataType _type, const char* _string)
{
    const char* ret;

    ret = m_doc[_type][_string].GetString();

    return ret;
}
const char* LoadJson::GetStringData(JsonDataType _type, const char* _string, const int _indexNum)
{
    const char* ret;

    ret = m_doc[_type][_string][_indexNum].GetString();

    return ret;
}
const char* LoadJson::GetStringData(JsonDataType _type, const char* _string, const int _indexNum, const char* _indexString)
{
    const char* ret;

    ret = m_doc[_type][_string][_indexNum][_indexString].GetString();

    return ret;
}
const char* LoadJson::GetStringData(JsonDataType _type, const char* _string, const int _indexNum, const char* _indexString, const int _secondIndexNum)
{
    const char* ret;

    ret = m_doc[_type][_string][_indexNum][_indexString][_secondIndexNum].GetString();

    return ret;
}
const char* LoadJson::GetStringData(JsonDataType _type, const char* _string, const int _indexNum, const char* _indexString, const int _secondIndexNum, const char* _secondIndexString)
{
    const char* ret;

    ret = m_doc[_type][_string][_indexNum][_indexString][_secondIndexNum][_secondIndexString].GetString();

    return ret;
}
const char* LoadJson::GetStringData(JsonDataType _type, const char* _string, const int _indexNum, const char* _indexString, const int _secondIndexNum, const char* _secondIndexString, const int _thirdIndexNum)
{
    const char* ret;

    ret = m_doc[_type][_string][_indexNum][_indexString][_secondIndexNum][_secondIndexString][_thirdIndexNum].GetString();

    return ret;
}

//  XMFLOAT3型のデータを取得
XMFLOAT3 LoadJson::GetXMFLOAT3Data(JsonDataType _type, const char* _string)
{
    XMFLOAT3 ret;

    ret.x = m_doc[_type][_string]["x"].GetFloat();
    ret.y = m_doc[_type][_string]["y"].GetFloat();
    ret.z = m_doc[_type][_string]["z"].GetFloat();

    return ret;
}
XMFLOAT3 LoadJson::GetXMFLOAT3Data(JsonDataType _type, const char* _string, const int _indexNum)
{
    XMFLOAT3 ret;

    ret.x = m_doc[_type][_string][_indexNum]["x"].GetFloat();
    ret.y = m_doc[_type][_string][_indexNum]["y"].GetFloat();
    ret.z = m_doc[_type][_string][_indexNum]["z"].GetFloat();

    return ret;
}
XMFLOAT3 LoadJson::GetXMFLOAT3Data(JsonDataType _type, const char* _string, const char* _indexString)
{
    XMFLOAT3 ret;

    ret.x = m_doc[_type][_string][_indexString]["x"].GetFloat();
    ret.y = m_doc[_type][_string][_indexString]["y"].GetFloat();
    ret.z = m_doc[_type][_string][_indexString]["z"].GetFloat();

    return ret;
}
XMFLOAT3 LoadJson::GetXMFLOAT3Data(JsonDataType _type, const char* _string, const int _indexNum, const char* _indexString)
{
    XMFLOAT3 ret;

    ret.x = m_doc[_type][_string][_indexNum][_indexString]["x"].GetFloat();
    ret.y = m_doc[_type][_string][_indexNum][_indexString]["y"].GetFloat();
    ret.z = m_doc[_type][_string][_indexNum][_indexString]["z"].GetFloat();

    return ret;
}
XMFLOAT3 LoadJson::GetXMFLOAT3Data(JsonDataType _type, const char* _string, const int _indexNum, const char* _indexString, const int _secondIndexNum)
{
    XMFLOAT3 ret;

    ret.x = m_doc[_type][_string][_indexNum][_indexString][_secondIndexNum]["x"].GetFloat();
    ret.y = m_doc[_type][_string][_indexNum][_indexString][_secondIndexNum]["y"].GetFloat();
    ret.z = m_doc[_type][_string][_indexNum][_indexString][_secondIndexNum]["z"].GetFloat();

    return ret;
}

//  配列やオブジェクトのサイズを取得
int LoadJson::GetArraySize(JsonDataType _type, const char* _string)
{
    int ret;

    ret = m_doc[_type][_string].Size();

    return ret;
}
int LoadJson::GetArraySize(JsonDataType _type, const char* _string, const int _indexNum)
{
    int ret;

    ret = m_doc[_type][_string][_indexNum].Size();

    return ret;
}
int LoadJson::GetArraySize(JsonDataType _type, const char* _string, const int _indexNum, const char* _indexString)
{
    int ret;

    ret = m_doc[_type][_string][_indexNum][_indexString].Size();

    return ret;
}
int LoadJson::GetArraySize(JsonDataType _type, const char* _string, const int _indexNum, const char* _indexString, const int _secondIndexNum)
{
    int ret;

    ret = m_doc[_type][_string][_indexNum][_indexString][_secondIndexNum].Size();

    return ret;
}
int LoadJson::GetArraySize(JsonDataType _type, const char* _string, const int _indexNum, const char* _indexString, const int _secondIndexNum, const char* _secondIndexString)
{
    int ret;

    ret = m_doc[_type][_string][_indexNum][_indexString][_secondIndexNum][_secondIndexString].Size();

    return ret;
}
int LoadJson::GetArraySize(JsonDataType _type, const char* _string, const int _indexNum, const char* _indexString, const int _secondIndexNum, const char* _secondIndexString, const int _thirdIndexNum)
{
    int ret;

    ret = m_doc[_type][_string][_indexNum][_indexString][_secondIndexNum][_secondIndexString][_thirdIndexNum].Size();

    return ret;
}