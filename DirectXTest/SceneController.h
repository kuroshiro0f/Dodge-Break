#pragma once

enum class SceneType;
//  各シーンへ動作を指示するクラス
//  シングルトンで扱う
class SceneController
{
public:
    SceneController();
    ~SceneController();

    //  初期化
    //  NOTE: コンストラクタで各シーンをnewすると無限ループが発生するため初期化関数を作成
    void Init();

    //  更新
    void Update(const float _deltaTime);

    //  シーンの変更
    void ChangeScene(const SceneType _type);
private:
    //  描画
    void DrawModel();
    void DrawSprite();
    void DrawString();

    //  シーンのタイプ
    SceneType m_type;

    //  クラスへのポインタ
    //  各シーン
    class Title* m_title;
    class GameScene* m_gameScene;
    class Result* m_result;

    //  シングルトンクラスへの参照
    class Device& m_device;     //  デバイスの管理
    class EffekseerManager& m_effect;   //  Effekseerから読み込んだエフェクトの管理
};