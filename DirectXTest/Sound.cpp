#include "Sound.h"

#include <string>

#include "EnumIterator.h"
#include "SoundData.h"
#include "SoundType.h"

#include "Singleton.h"
#include "CheckError.h"

//  音量の倍率の変化量
static const float CHANGE_VOLUME_MAGNIFICATION = 0.2;

Sound::Sound()
    :m_error(Singleton<CheckError>::GetInstance())
    , m_volumeMagnification(1)
    , m_audioEngineflags(AudioEngine_Default)
    , m_wavData(0)
    , m_soundQue({})
{
}

Sound::~Sound()
{
    //  メモリの解放
    for (auto itr : m_soundQue)
    {
        itr.second.clear();
    }
}

//  初期化
void Sound::Init()
{
    CoInitializeEx(nullptr, COINIT_MULTITHREADED);

    //  列挙型SoundTypeのイテレータを作成
    typedef EnumIterator<SoundType, SoundType::TitleBGM, SoundType::AttackSE> typeItr;

    //  全てのサウンドデータをロード
    for (auto itr : typeItr())
    {
        //  二重ロードの防止
        for (auto secondItr : typeItr())
        {
            if (itr != secondItr && SoundData::PATH_MAP[itr] == SoundData::PATH_MAP[secondItr])
            {
                std::string str = "サウンド : " + std::to_string(static_cast<int>(itr)) + "のファイルは二重にロードされています。";
                m_error.CreateErrorMessage(str);
            }
        }
        Load(itr);
    }
}

//  ロード
void Sound::Load(const SoundType& _type)
{
    //  オーディオエンジンの設定
    m_audioEngine[_type] = std::make_unique<AudioEngine>(m_audioEngineflags);

    //  サウンドファイルの読み取り
    m_soundEffect[_type] = std::make_unique<SoundEffect>(m_audioEngine[_type].get(), SoundData::PATH_MAP[_type]);
    //  エラーチェック
    if (m_soundEffect[_type] == nullptr)
    {
        std::string str = "サウンド : " + std::to_string(static_cast<int>(_type)) + " の読み取り中にエラーが発生しました。";
        m_error.CreateErrorMessage(str);
    }
}

//  再生する
//  NOTE: 再生したキューの番号を返すため、int型にしています。 
//        途中で停止させたい場合はStop関数の引数_indexに返り値を渡してください。
int Sound::Play(const SoundType& _type, const bool _isLoop, const bool _isFromBegin)
{
    //  添え字カウント用
    int index = 0;

     //  指定されたタイプのサウンドキューを検索
     for (auto itr : m_soundQue[_type])
     {
         //  停止中のキューがあればそのキューを再生に変更
         if (itr->GetState() == SoundState::STOPPED)
         {
             itr->Play(_isLoop);
             return index;
         }
         index++;
     }
     //  指定されたタイプのサウンドキューがない、
     //  もしくは全て再生中なら新しくサウンドキューを作成して再生する
     PushSoundQue(_type, _isLoop);
     return index;
}

//  停止する
void Sound::Stop(const SoundType& _type, const int _index, const bool _immediate)
{
    m_soundQue[_type][_index]->Stop(_immediate);
}

//  ボリュームを変更する
void Sound::ChangeVolume(const bool _isUp)
{
    //  音量の倍率を変更
    if (_isUp)
    {
        m_volumeMagnification += CHANGE_VOLUME_MAGNIFICATION;
    }
    else
    {
        m_volumeMagnification -= CHANGE_VOLUME_MAGNIFICATION;
    }

    //  列挙型SoundTypeのイテレータを作成
    typedef EnumIterator<SoundType, SoundType::TitleBGM, SoundType::AttackSE> type;

    for (auto typeItr : type())
    {
        for (auto soundItr : m_soundQue[typeItr])
        {
            soundItr->SetVolume(SoundData::FIRST_VOLUME[typeItr] * m_volumeMagnification);
        }
    }
}

//  サウンドキューを作成
void Sound::PushSoundQue(const SoundType& _type, const bool _isLoop)
{
    //  指定されたタイプのサウンドのインスタンスを生成
    std::shared_ptr<SoundEffectInstance> instance = m_soundEffect[_type]->CreateInstance();
    //  ボリュームを現在のボリュームに合わせて設定
    instance->SetVolume(SoundData::FIRST_VOLUME[_type] * m_volumeMagnification);
    //  サウンドキュー内にインスタンスを格納
    m_soundQue[_type].emplace_back(instance);
    //  再生
    m_soundQue[_type].back()->Play(_isLoop);
}
