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

//	�萔
const int ENEMY_NUM = 10;		//	�G�l�~�[�̐�
const int PLAYER_FLAG = 0;		//	�v���C���[�̃t���O
const int ENEMY_FLAG = 1;		//	�G�l�~�[�̃t���O
const int BULLET_FLAG = 2;
static const int FLAG_NUM = 3;	//	�t���O��
const int SHOT_INTERVAL = 60;

class Dx12Wrapper;
class PMDRenderer;
class PMDActor;
class PlayerActor;
//	�V���O���g���N���X
class Game
{
public:
	//	Game�̃V���O���g���C���X�^���X�𓾂�
	static Game& Instance();

	//	������
	bool Init();

	//	���[�v�N��
	void Run();

	//	�㏈��
	void Terminate();
	SIZE GetWindowSize()const;
	~Game();

private:
	//	�E�B���h�E�N���X����
	WNDCLASSEX m_windowClass = {};
	HWND m_hwnd;
	shared_ptr<Dx12Wrapper> m_dx12;
	shared_ptr<PMDRenderer> m_pmdRenderer;
	/*shared_ptr<PlayerActor> m_playerActor;*/
	shared_ptr<PMDActor> m_pmdActor[FLAG_NUM][ENEMY_NUM];

	//	�Q�[���p�E�B���h�E�̐���
	void CreateGameWindow(HWND& _hwnd, WNDCLASSEX& _windowClass);

	//	��ԋ߂��ʒu�ɂ���G�l�~�[�̔ԍ��𓾂�
	int M_closestEnemy(XMFLOAT3 _enemyPos[],XMFLOAT3 _playerPos);

	//	�Q�[���I�[�o�[
	void M_gameOver();

	ComPtr<ID3D12DescriptorHeap> m_heapForSpriteFont;

	GraphicsMemory* m_gmemory = nullptr;		//	�O���t�B�b�N�X�������I�u�W�F�N�g
	SpriteFont* m_spriteFont = nullptr;			//	�t�H���g�\���p�I�u�W�F�N�g
	SpriteBatch* m_spriteBatch = nullptr;		//	�X�v���C�g�\���p�I�u�W�F�N�g

	//	���W
	XMFLOAT3 m_playerPos;						//	�v���C���[
	XMFLOAT3 m_enemyPos[ENEMY_NUM];				//	�G�l�~�[

	//	��ԋ߂��G�l�~�[�̔ԍ�
	int m_closestEnemyNum;

	//	�����ꂽ�G�l�~�[�̔ԍ�
	bool m_erasedEnemyFlag[ENEMY_NUM];

	//	�����ꂽ�G�l�~�[�̐�
	int m_erasedEnemyNum;

	//	���[�v���Ɉ�x�����g��
	bool m_onceOnly;

	//	�e�������Ă��邩
	bool m_shotFlag;

	//	�Q�[���I�[�o�[����
	bool m_gameOverFlag;

	//	�e�������߂̎��ԊǗ�
	int m_shotInterval;

	class SceneMgr* m_scene;
	class KeyBoard* m_keyBoard;

	//	�V���O���g���̂��߂ɃR���X�g���N�^��private��
	//	����ɃR�s�[�Ƒ�����֎~����
	Game();
	Game(const Game&) = delete;
	void operator=(const Game&) = delete;
};