#pragma once

#include<Windows.h>
#include<tchar.h>
#include<d3d12.h>
#include<dxgi1_6.h>
#include<DirectXMath.h>
#include<vector>
#include<map>
#include<d3dcompiler.h>
#include<DirectXTex.h>
#include<d3dx12.h>
#include<wrl.h>
#include<memory>

#include "SceneMgr.h"
#include "GameScene.h"
#include "KeyBoard.h"

//	DXTK12
#include<SpriteFont.h>
#include<ResourceUploadBatch.h>

#pragma comment(lib,"DirectXTK12.lib")
#pragma comment(lib,"dxguid.lib")

using namespace std;
using namespace DirectX;
using namespace Microsoft::WRL;

//	定数
const int ENEMY_NUM = 10;		//	エネミーの数
const int PLAYER_FLAG = 0;		//	プレイヤーのフラグ
const int ENEMY_FLAG = 1;		//	エネミーのフラグ
const int BULLET_FLAG = 2;
static const int FLAG_NUM = 3;	//	フラグ数
const int SHOT_INTERVAL = 60;

class Dx12Wrapper;
class PMDRenderer;
class PMDActor;
class PlayerActor;
//	シングルトンクラス
class Game
{
public:
	//	Gameのシングルトンインスタンスを得る
	static Game& Instance();

	//	初期化
	bool Init();

	//	ループ起動
	void Run();

	//	後処理
	void Terminate();
	SIZE GetWindowSize()const;
	~Game();

private:
	//	ウィンドウクラス周り
	WNDCLASSEX m_windowClass = {};
	HWND m_hwnd;
	shared_ptr<Dx12Wrapper> m_dx12;
	shared_ptr<PMDRenderer> m_pmdRenderer;
	/*shared_ptr<PlayerActor> m_playerActor;*/
	shared_ptr<PMDActor> m_pmdActor[FLAG_NUM][ENEMY_NUM];

	//	ゲーム用ウィンドウの生成
	void CreateGameWindow(HWND& _hwnd, WNDCLASSEX& _windowClass);

	//	一番近い位置にいるエネミーの番号を得る
	int M_closestEnemy(XMFLOAT3 _enemyPos[],XMFLOAT3 _playerPos);

	//	ゲームオーバー
	void M_gameOver();

	ComPtr<ID3D12DescriptorHeap> m_heapForSpriteFont;

	GraphicsMemory* m_gmemory = nullptr;		//	グラフィックスメモリオブジェクト
	SpriteFont* m_spriteFont = nullptr;			//	フォント表示用オブジェクト
	SpriteBatch* m_spriteBatch = nullptr;		//	スプライト表示用オブジェクト

	//	座標
	XMFLOAT3 m_playerPos;						//	プレイヤー
	XMFLOAT3 m_enemyPos[ENEMY_NUM];				//	エネミー

	//	一番近いエネミーの番号
	int m_closestEnemyNum;

	//	消されたエネミーの番号
	bool m_erasedEnemyFlag[ENEMY_NUM];

	//	消されたエネミーの数
	int m_erasedEnemyNum;

	//	ループ中に一度だけ使う
	bool m_onceOnly;

	//	弾を撃っているか
	bool m_shotFlag;

	//	ゲームオーバー判定
	bool m_gameOverFlag;

	//	弾を撃つための時間管理
	int m_shotInterval;

	class SceneMgr* m_scene;
	class KeyBoard* m_keyBoard;

	//	シングルトンのためにコンストラクタをprivateに
	//	さらにコピーと代入を禁止する
	Game();
	Game(const Game&) = delete;
	void operator=(const Game&) = delete;
};