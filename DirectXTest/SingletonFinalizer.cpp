#include "SingletonFinalizer.h"

#include <mutex>
#include <cassert>

namespace
{
    //  解放処理をおこなえるインスタンスの最大数
    constexpr int finalizersSizeMax = 256;
    std::mutex gMutex;
    //  解放処理に登録されているインスタンスの数
    int finalizersNum = 0;
    //  解放処理用の関数
    SingletonFinalizer::FinalizerFunc finalizerFunc[finalizersSizeMax];

}

//  シングルトンクラスの終了処理時に解放されるインスタンスの追加
void SingletonFinalizer::AddFinalizer(FinalizerFunc _func) {
    //  排他制御を行う
    std::lock_guard<std::mutex> lock(gMutex);
    //  登録できるインスタンス数を超えたらエラーを出す
    assert(finalizersNum < finalizersSizeMax);
    //  インスタンスを解放処理に登録
    finalizerFunc[finalizersNum++] = _func;
}

//  シングルトンクラスの終了処理
void SingletonFinalizer::Finalize() {
    //  排他制御を行う
    std::lock_guard<std::mutex> lock(gMutex);
    //  登録されているインスタンスを全て解放する
    for (int i = finalizersNum - 1; i >= 0; --i) {
        (*finalizerFunc[i])();
    }
    finalizersNum = 0;
}