#include "CheckError.h"

#include "Singleton.h"
#include "Device.h"

CheckError::CheckError()
    :m_device(Singleton<Device>::GetInstance())
    , m_isOccurError(false)
{
    //  処理なし
}

CheckError::~CheckError()
{
    //  処理なし
}

//  エラーメッセージの作成
void CheckError::CreateErrorMessage(const std::string& _str)
{
    //  エラーメッセージをコピー
    m_errorMessage = _str;
    //  エラーが発生
    m_isOccurError = true;
}

//  エラーが起きているか
bool CheckError::IsOccurError()
{
    //  エラーが発生していれば
    if (m_isOccurError)
    {
        //  エラーメッセージを出す
        MessageBox(m_device.hwnd, m_errorMessage.c_str(), "エラー", MB_OK);
        return m_isOccurError;
    }
    return m_isOccurError;
}
