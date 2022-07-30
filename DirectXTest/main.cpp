#include<Windows.h>
#include "GameLoop.h"

//  円周率を定義
#define PI 3.1415926f

//  TODO: 処理が重くなっている原因を探す
//  TODO: クラス内にstatic const変数が多数ある箇所のファイル分け

//  main関数
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
    class GameLoop* gameLoop = new GameLoop();
    gameLoop->Init();
    gameLoop->Run();
    delete gameLoop;
    return 0;
}