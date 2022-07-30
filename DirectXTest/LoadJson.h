#pragma once
#include <unordered_map>
#include <DirectXMath.h>
#include "rapidjson/document.h"

#include "JsonDataType.h"

using namespace DirectX;

//  jsonファイルをロードするクラス
//  NOTE: jsonファイル内のデータのネストの深さによって処理を変える必要があるため、
//        同じ型のデータを読み取る際の処理を分岐
//  NOTE: 外部ファイルからの読み取りは処理が重いため、
//        初期化時以外には読み取らないようにする。
class LoadJson
{
public:
    LoadJson();
    ~LoadJson();

    //  初期化
    void Init();

    //  ファイルのロード
    void Load();

    //  int型のデータを取得
    int GetIntData(JsonDataType _type, const char* _string);
    int GetIntData(JsonDataType _type, const char* _string, const int _indexNum);
    int GetIntData(JsonDataType _type, const char* _string, const int _indexNum, const char* _indexString);
    int GetIntData(JsonDataType _type, const char* _string, const int _indexNum, const char* _indexString, const int _secondIndexNum);
    int GetIntData(JsonDataType _type, const char* _string, const int _indexNum, const char* _indexString, const int _secondIndexNum, const char* _secondIndexString);
    int GetIntData(JsonDataType _type, const char* _string, const int _indexNum, const char* _indexString, const int _secondIndexNum, const char* _secondIndexString, const int _thirdIndexNum);

    //  float型のデータを取得
    float GetFloatData(JsonDataType _type, const char* _string);
    float GetFloatData(JsonDataType _type, const char* _string, const int _indexNum);
    float GetFloatData(JsonDataType _type, const char* _string, const int _indexNum, const char* _indexString);
    float GetFloatData(JsonDataType _type, const char* _string, const int _indexNum, const char* _indexString, const int _secondIndexNum);
    float GetFloatData(JsonDataType _type, const char* _string, const int _indexNum, const char* _indexString, const int _secondIndexNum, const char* _secondIndexString);
    float GetFloatData(JsonDataType _type, const char* _string, const int _indexNum, const char* _indexString, const int _secondIndexNum, const char* _secondIndexString, const int _thirdIndexNum);

    //  string型のデータを取得
    const char* GetStringData(JsonDataType _type, const char* _string);
    const char* GetStringData(JsonDataType _type, const char* _string, const int _indexNum);
    const char* GetStringData(JsonDataType _type, const char* _string, const int _indexNum, const char* _indexString);
    const char* GetStringData(JsonDataType _type, const char* _string, const int _indexNum, const char* _indexString, const int _secondIndexNum);
    const char* GetStringData(JsonDataType _type, const char* _string, const int _indexNum, const char* _indexString, const int _secondIndexNum, const char* _secondIndexString);
    const char* GetStringData(JsonDataType _type, const char* _string, const int _indexNum, const char* _indexString, const int _secondIndexNum, const char* _secondIndexString, const int _thirdIndexNum);

    //  XMFLOAT3型のデータを取得
    XMFLOAT3 GetXMFLOAT3Data(JsonDataType _type, const char* _string);
    XMFLOAT3 GetXMFLOAT3Data(JsonDataType _type, const char* _string, const int _indexNum);
    XMFLOAT3 GetXMFLOAT3Data(JsonDataType _type, const char* _string, const char* _indexString);
    XMFLOAT3 GetXMFLOAT3Data(JsonDataType _type, const char* _string, const int _indexNum, const char* _indexString);
    XMFLOAT3 GetXMFLOAT3Data(JsonDataType _type, const char* _string, const int _indexNum, const char* _indexString, const int _secondIndexNum);

    //  配列やオブジェクトのサイズを取得
    int GetArraySize(JsonDataType _type, const char* _string);
    int GetArraySize(JsonDataType _type, const char* _string, const int _indexNum);
    int GetArraySize(JsonDataType _type, const char* _string, const int _indexNum, const char* _indexString);
    int GetArraySize(JsonDataType _type, const char* _string, const int _indexNum, const char* _indexString, const int _secondIndexNum);
    int GetArraySize(JsonDataType _type, const char* _string, const int _indexNum, const char* _indexString, const int _secondIndexNum, const char* _secondIndexString);
    int GetArraySize(JsonDataType _type, const char* _string, const int _indexNum, const char* _indexString, const int _secondIndexNum, const char* _secondIndexString, const int _thirdIndexNum);

private:
    //  ファイルから読み取ったデータをファイルごとに格納する配列
    std::unordered_map<JsonDataType, rapidjson::Document> m_doc;

    //  jasonファイルへのファイルパス
    std::unordered_map<JsonDataType, const char*> m_jasonDataFilePath;
};