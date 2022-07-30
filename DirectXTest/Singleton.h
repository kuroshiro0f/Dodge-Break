#pragma once
#include <mutex>
#include <cassert>

#include "SingletonFinalizer.h"

//  クラスをシングルトン化するクラス
//  Tにはクラスを指定する
template <typename T>
class Singleton final {
public:
    //  クラスTのインスタンスを取得
    static T& GetInstance()
    {
        //  一度だけCreate関数を呼ぶ
        call_once(m_initFlag, Create);
        //  インスタンスを正常に作成できたかチェックする
        assert(m_instance);
        //  インスタンスの値を返す
        return *m_instance;
    }

private:
    //  インスタンスの生成
    static void Create()
    {
        m_instance = new T;
        //  解放処理にインスタンスを登録
        SingletonFinalizer::AddFinalizer(&Singleton<T>::Delete);
    }

    //  インスタンスの解放
    static void Delete() 
    {
        delete m_instance;
        m_instance = nullptr;
    }

    //  call_once関数にて、引数に設定された関数を一度だけ呼ぶための変数
    static std::once_flag m_initFlag;
    //  クラスTのインスタンス
    static T* m_instance;
};

//  静的メンバ変数の実体を定義
template <typename T> std::once_flag Singleton<T>::m_initFlag;
template <typename T> T* Singleton<T>::m_instance = nullptr;