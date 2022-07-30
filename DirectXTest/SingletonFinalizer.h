#pragma once

//  シングルトンの解放を行うクラス
class SingletonFinalizer {
public:
    //  このクラスの関数を利用するための宣言
    using FinalizerFunc = void(*)();
    //  シングルトンクラスの終了処理時に解放されるインスタンスの登録
    static void AddFinalizer(FinalizerFunc _func);
    //  シングルトンクラスの終了処理
    static void Finalize();
};