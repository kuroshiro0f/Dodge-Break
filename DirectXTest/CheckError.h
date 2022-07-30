#pragma once
#include <string>

//  エラーチェックをするクラス
class CheckError
{
public:
    CheckError();
    ~CheckError();

    //  エラーメッセージの作成
    void CreateErrorMessage(const std::string& _str);

    //  エラーは発生しているか
    bool IsOccurError();
private:
    //  エラーが発生しているか
    bool m_isOccurError;
    //  エラーメッセージ
    std::string m_errorMessage;

  //  シングルトンクラスへの参照
    class Device& m_device; //  デバイスの管理
};