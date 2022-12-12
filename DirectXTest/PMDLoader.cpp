#include "PMDLoader.h"

#include <d3dx12.h>

#include "Dx12Wrapper.h"
#include "PMDRenderer.h"

#include "Singleton.h"
#include "Device.h"

namespace
{
    ///  Textureのパスをセパレータ文字で分離する
    ///  @param path 対象のパス文字列
    ///  @param splitter 区切り文字
    ///  @return 分離前後の文字列ペア
    std::pair<std::string, std::string>
        SplitFileName(const std::string& _path, const char _splitter = '*') {
        int idx = _path.find(_splitter);
        std::pair<std::string, std::string> ret;
        ret.first = _path.substr(0, idx);
        ret.second = _path.substr(idx + 1, _path.length() - idx - 1);
        return ret;
    }
    ///  ファイル名から拡張子を取得する
    ///  @param path 対象のパス文字列
    ///  @return 拡張子
    std::string
        GetExtension(const std::string& _path) {
        int idx = _path.rfind('.');
        return _path.substr(idx + 1, _path.length() - idx - 1);
    }
    ///  モデルのパスとTextureのパスから合成パスを得る
    ///  @param modelPath アプリケーションから見たpmdモデルのパス
    ///  @param texPath PMDモデルから見たTextureのパス
    ///  @return アプリケーションから見たTextureのパス
    std::string GetTexturePathFromModelAndTexPath(const std::string& _modelPath, const char* _texPath) {
        //  ファイルのフォルダ区切りは\と/の二種類が使用される可能性があり
        //  ともかく末尾の\か/を得られればいいので、双方のrfindをとり比較する
        //  int型に代入しているのは見つからなかった場合はrfindがepos(-1→0xffffffff)を返すため
        int pathIndex1 = _modelPath.rfind('/');
        int pathIndex2 = _modelPath.rfind('\\');
        auto pathIndex = max(pathIndex1, pathIndex2);
        auto folderPath = _modelPath.substr(0, pathIndex + 1);
        return folderPath + _texPath;
    }
}

PMDLoader::PMDLoader()
    :m_device(Singleton<Device>::GetInstance())
    , m_vb(nullptr)
    , m_ib(nullptr)
    , m_vbView({})
    , m_ibView({})
    , m_materials({})
    , m_materialBuff(nullptr)
    , m_textureResources({})
    , m_sphResources({})
    , m_spaResources({})
    , m_toonResources({})
    , m_materialHeap(nullptr)
{
    //  処理なし
}

PMDLoader::~PMDLoader()
{
    //  処理なし
}

//  モデルのロード
void PMDLoader::LoadModel(const char* _filepath)
{
    //  PMDをロード
    LoadPMDFile(_filepath);
    //  Materialの情報を作成
    CreateMaterialData();
    //  MaterialとTextureのViewを作成
    CreateMaterialAndTextureView();
}

//  Material構造体のゲッター
std::vector<Material> PMDLoader::GetMaterials()const
{
    return m_materials;
}

//  読み込んだMaterialをもとにMaterialBufferを作成
HRESULT PMDLoader::CreateMaterialData()
{
    //  MaterialBufferのサイズは1つあたり256アライメント区切りでまとめる
    auto materialBuffSize = sizeof(MaterialForHlsl);
    materialBuffSize = (materialBuffSize + 0xff) & ~0xff;
    //  HeapPropertiesはUpload用に設定
    auto temp1 = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
    //  ResourceDescを設定
    auto temp2 = CD3DX12_RESOURCE_DESC::Buffer(materialBuffSize * m_materials.size());
    //  MaterialBufferを作成
    auto result = m_device.dx12->GetDevice()->CreateCommittedResource(
        &temp1,
        D3D12_HEAP_FLAG_NONE,   //  特に指定なし
        &temp2,
        D3D12_RESOURCE_STATE_GENERIC_READ,  //  Upload設定のHeapPropertiesに合わせる
        nullptr,
        IID_PPV_ARGS(m_materialBuff.ReleaseAndGetAddressOf())
    );
    if (FAILED(result)) {
        assert(SUCCEEDED(result));
        return result;
    }

    //  MaterialBufferをGPUに渡して、その仮想ポインタをmapMaterialに格納(map)
    char* mapMaterial = nullptr;
    result = m_materialBuff->Map(0, nullptr, (void**)&mapMaterial);
    if (FAILED(result)) {
        assert(SUCCEEDED(result));
        return result;
    }
    //  実データとアライメントサイズが違うため、次のアライメント位置まで進める処理を行う
    for (auto& m : m_materials) {
        *((MaterialForHlsl*)mapMaterial) = m.material;    //  データコピー
        mapMaterial += materialBuffSize;    //  次のアライメント位置まで進める
    }
    //  mapMaterialを介して行うMaterialBufferの変更を終了
    m_materialBuff->Unmap(0, nullptr);

    return S_OK;
}

//  MaterialとTextureのViewを生成
HRESULT PMDLoader::CreateMaterialAndTextureView()
{
    //  MaterialのDiscriptorHeap作成
    D3D12_DESCRIPTOR_HEAP_DESC materialDescHeapDesc = {};
    materialDescHeapDesc.NumDescriptors = m_materials.size() * 5;    //  Material数ぶん(定数1つ、Texture3つ)
    materialDescHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE; //  Shaderから見えるように
    materialDescHeapDesc.NodeMask = 0;      //  操作するAdoptorは単一
    materialDescHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;    //  DiscriptorHeap種別
    //  MaterialのDiscriptorHeap生成
    auto result = m_device.dx12->GetDevice()->CreateDescriptorHeap(&materialDescHeapDesc, IID_PPV_ARGS(m_materialHeap.ReleaseAndGetAddressOf()));
    if (FAILED(result))
    {
        assert(SUCCEEDED(result));
        return result;
    }

    //  MaterialBufferのサイズは1つあたり256アライメント区切りでまとめる
    auto materialBuffSize = sizeof(MaterialForHlsl);
    materialBuffSize = (materialBuffSize + 0xff) & ~0xff;
    //  Material用定数BufferViewの設定
    D3D12_CONSTANT_BUFFER_VIEW_DESC matCBVDesc = {};
    matCBVDesc.BufferLocation = m_materialBuff->GetGPUVirtualAddress();
    matCBVDesc.SizeInBytes = materialBuffSize;

    //  通常TextureView作成
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;    //  2DTexture
    srvDesc.Texture2D.MipLevels = 1;    //  ミップマップは使用しないので1

    //  MaterialのDiscriptorHeapポインタを取得
    CD3DX12_CPU_DESCRIPTOR_HANDLE matDescHeapH(m_materialHeap->GetCPUDescriptorHandleForHeapStart());
    auto incSize = m_device.dx12->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

    //  Materialの数だけ処理を行う
    auto materialsSize = m_materials.size();
    for (int i = 0; i < materialsSize; ++i)
    {
        //  MaterialのBufferView作成
        m_device.dx12->GetDevice()->CreateConstantBufferView(&matCBVDesc, matDescHeapH);
        //  Materialのディスクリプタヒープの指定先を次の情報へ
        matDescHeapH.ptr += incSize;
        matCBVDesc.BufferLocation += materialBuffSize;

        //  Material内の情報ごとにShaderResourceViewを作成
        //  Texture
        //  Texture情報がない場合は白Textureを指定
        if (m_textureResources[i] == nullptr) {
            srvDesc.Format = m_device.pmdRenderer->m_whiteTex->GetDesc().Format;
            m_device.dx12->GetDevice()->CreateShaderResourceView(m_device.pmdRenderer->m_whiteTex.Get(), &srvDesc, matDescHeapH);
        }
        //  Texture情報がある場合はそれを指定
        else {
            srvDesc.Format = m_textureResources[i]->GetDesc().Format;
            m_device.dx12->GetDevice()->CreateShaderResourceView(m_textureResources[i].Get(), &srvDesc, matDescHeapH);
        }
        matDescHeapH.Offset(incSize);
        //  sph
        //  sph情報がない場合は白Textureを指定
        if (m_sphResources[i] == nullptr) {
            srvDesc.Format = m_device.pmdRenderer->m_whiteTex->GetDesc().Format;
            m_device.dx12->GetDevice()->CreateShaderResourceView(m_device.pmdRenderer->m_whiteTex.Get(), &srvDesc, matDescHeapH);
        }
        //  sph情報がある場合はそれを指定
        else {
            srvDesc.Format = m_sphResources[i]->GetDesc().Format;
            m_device.dx12->GetDevice()->CreateShaderResourceView(m_sphResources[i].Get(), &srvDesc, matDescHeapH);
        }
        matDescHeapH.ptr += incSize;
        //  spa
        //  spa情報がない場合は黒Textureを指定
        if (m_spaResources[i] == nullptr) {
            srvDesc.Format = m_device.pmdRenderer->m_blackTex->GetDesc().Format;
            m_device.dx12->GetDevice()->CreateShaderResourceView(m_device.pmdRenderer->m_blackTex.Get(), &srvDesc, matDescHeapH);
        }
        //  spa情報がある場合はそれを指定
        else {
            srvDesc.Format = m_spaResources[i]->GetDesc().Format;
            m_device.dx12->GetDevice()->CreateShaderResourceView(m_spaResources[i].Get(), &srvDesc, matDescHeapH);
        }
        matDescHeapH.ptr += incSize;
        //  toon
        //  toon情報がない場合はグラデーションTextureを指定
        if (m_toonResources[i] == nullptr) {
            srvDesc.Format = m_device.pmdRenderer->m_gradTex->GetDesc().Format;
            m_device.dx12->GetDevice()->CreateShaderResourceView(m_device.pmdRenderer->m_gradTex.Get(), &srvDesc, matDescHeapH);
        }
        //  toon情報がある場合はそれを指定
        else {
            srvDesc.Format = m_toonResources[i]->GetDesc().Format;
            m_device.dx12->GetDevice()->CreateShaderResourceView(m_toonResources[i].Get(), &srvDesc, matDescHeapH);
        }
        matDescHeapH.ptr += incSize;
    }
}

//  PMDファイルのロード
HRESULT PMDLoader::LoadPMDFile(const char* _path)
{
    //  PMDヘッダ構造体
    struct PMDHeader {
        float version;    //  例：00 00 80 3F == 1.00
        char model_name[20];    //  モデル名
        char comment[256];        //  モデルコメント
    };
    //  PMDファイルの先頭にある"Pmd"という3バイトの文字列
    char signature[3];
    //  PMDヘッダ構造体を変数として宣言
    PMDHeader pmdheader = {};

    //  モデルのファイル名
    std::string strModelPath = _path;

    //  ファイル名読み取り
    auto fp = fopen(strModelPath.c_str(), "rb");
    if (fp == nullptr) {
        //  エラー処理
        assert(0);
        return ERROR_FILE_NOT_FOUND;
    }
    //  "Pmd"読み取り
    fread(signature, sizeof(signature), 1, fp);
    //  PMDヘッダ構造体読み取り
    fread(&pmdheader, sizeof(pmdheader), 1, fp);

    //  頂点数
    unsigned int vertNum;
    //  頂点数読み取り
    fread(&vertNum, sizeof(vertNum), 1, fp);

#pragma pack(1)    //  ここから1バイトパッキング…アライメントは発生しない
    //  PMDMaterial構造体
    struct PMDMaterial {
        XMFLOAT3 diffuse;     //  ディフューズ色
        float alpha;          //  ディフューズα
        float specularity;    //  スペキュラの強さ(乗算値)
        XMFLOAT3 specular;    //  スペキュラ色
        XMFLOAT3 ambient;     //  アンビエント色
        unsigned char toonIdx;    //  トゥーン番号
        unsigned char edgeFlg;    //  Material毎の輪郭線フラグ
        //  2バイトのパディングが発生！！
        unsigned int indicesNum;    //  このMaterialが割り当たるIndex数
        char texFilePath[20];       //  Textureファイル名
    };//  70バイトのはず…でもパディングが発生するため72バイト
#pragma pack()    //  1バイトパッキング解除

    //  頂点一つ当たりのサイズ
    constexpr unsigned int pmdvertex_size = 38;
    //  頂点情報
    std::vector<unsigned char> vertices(vertNum * pmdvertex_size);
    //  頂点情報の読み取り
    fread(vertices.data(), vertices.size(), 1, fp);

    //  頂点Index数
    unsigned int indicesNum;
    //  頂点Index数読み取り
    fread(&indicesNum, sizeof(indicesNum), 1, fp);

    //  HeapPropertiesはUploadに
    auto temp1 = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
    //  ResourceDescを設定
    auto temp2 = CD3DX12_RESOURCE_DESC::Buffer(vertices.size());
    //  頂点Bufferの作成
    auto result = m_device.dx12->GetDevice()->CreateCommittedResource(
        &temp1,
        D3D12_HEAP_FLAG_NONE,       //  特に指定なし
        &temp2,
        D3D12_RESOURCE_STATE_GENERIC_READ,   //  Upload設定のHeapPropertiesに合わせる
        nullptr,
        IID_PPV_ARGS(m_vb.ReleaseAndGetAddressOf()));
    
    //  頂点BufferをGPUに渡して、その仮想アドレスをvertMapに格納(map)
    unsigned char* vertMap = nullptr;
    result = m_vb->Map(0, nullptr, (void**)&vertMap);
    copy(vertices.begin(), vertices.end(), vertMap);
    //  仮想アドレスを介して頂点Bufferを変更できないように
    m_vb->Unmap(0, nullptr);

    //  頂点BufferViewを設定
    m_vbView.BufferLocation = m_vb->GetGPUVirtualAddress();    //  Bufferの仮想アドレス
    m_vbView.SizeInBytes = vertices.size();     //  全バイト数
    m_vbView.StrideInBytes = pmdvertex_size;    //  1頂点あたりのバイト数

    //  頂点Index情報
    std::vector<unsigned short> indices(indicesNum);
    //  頂点Index情報の読み取り
    fread(indices.data(), indices.size() * sizeof(indices[0]), 1, fp);

    //  頂点IndexBufferの設定と情報の転送
    auto temp3 = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
    auto temp4 = CD3DX12_RESOURCE_DESC::Buffer(indices.size() * sizeof(indices[0]));
    result = m_device.dx12->GetDevice()->CreateCommittedResource(
        &temp3,
        D3D12_HEAP_FLAG_NONE,
        &temp4,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(m_ib.ReleaseAndGetAddressOf()));

    //  作成したIndexBufferをGPUに渡して、その仮想アドレスをmappedIdxに格納(map)
    unsigned short* mappedIdx = nullptr;
    m_ib->Map(0, nullptr, (void**)&mappedIdx);
    copy(indices.begin(), indices.end(), mappedIdx);
    //  仮想アドレスを介してIndexBufferを変更できないように
    m_ib->Unmap(0, nullptr);

    //  IndexBufferViewを設定
    m_ibView.BufferLocation = m_ib->GetGPUVirtualAddress();
    m_ibView.Format = DXGI_FORMAT_R16_UINT;
    m_ibView.SizeInBytes = indices.size() * sizeof(indices[0]);

    //  Material数
    unsigned int materialNum;
    //  Material数の読み取り
    fread(&materialNum, sizeof(materialNum), 1, fp);
    //  Material数に合わせて各データのサイズを変更
    m_materials.resize(materialNum);
    m_textureResources.resize(materialNum);
    m_sphResources.resize(materialNum);
    m_spaResources.resize(materialNum);
    m_toonResources.resize(materialNum);

    //  Material情報
    std::vector<PMDMaterial> pmdMaterials(materialNum);
    auto pmdMaterialsSize = pmdMaterials.size();
    //  Material情報の読み取り
    fread(pmdMaterials.data(), pmdMaterialsSize * sizeof(PMDMaterial), 1, fp);
    //  読み取った情報をコピー
    for (int i = 0; i < pmdMaterialsSize; ++i) {
        m_materials[i].indicesNum = pmdMaterials[i].indicesNum;
        m_materials[i].material.diffuse = pmdMaterials[i].diffuse;
        m_materials[i].material.alpha = pmdMaterials[i].alpha;
        m_materials[i].material.specular = pmdMaterials[i].specular;
        m_materials[i].material.specularity = pmdMaterials[i].specularity;
        m_materials[i].material.ambient = pmdMaterials[i].ambient;
        m_materials[i].additional.toonIdx = pmdMaterials[i].toonIdx;
    }

    //  Materialのサイズ分ループ処理
    for (int i = 0; i < pmdMaterialsSize; ++i) {
        //  ToonResourceの読み込み
        char toonFilePath[32];
        sprintf(toonFilePath, "Data/Toon/toon%02d.bmp", pmdMaterials[i].toonIdx + 1);
        m_toonResources[i] = m_device.dx12->GetTextureByPath(toonFilePath);

        //  Textureの情報がない場合は処理を飛ばす
        if (strlen(pmdMaterials[i].texFilePath) == 0) {
            m_textureResources[i] = nullptr;
            continue;
        }

        //  Texture、乗算スフィアマップ、加算スフィアマップの読み込み
        std::string texFileName = pmdMaterials[i].texFilePath;
        std::string sphFileName = "";
        std::string spaFileName = "";
        if (count(texFileName.begin(), texFileName.end(), '*') > 0) {    //  スプリッタがある
            auto namepair = SplitFileName(texFileName);
            if (GetExtension(namepair.first) == "sph") {
                texFileName = namepair.second;
                sphFileName = namepair.first;
            }
            else if (GetExtension(namepair.first) == "spa") {
                texFileName = namepair.second;
                spaFileName = namepair.first;
            }
            else {
                texFileName = namepair.first;
                if (GetExtension(namepair.second) == "sph") {
                    sphFileName = namepair.second;
                }
                else if (GetExtension(namepair.second) == "spa") {
                    spaFileName = namepair.second;
                }
            }
        }
        else {
            if (GetExtension(pmdMaterials[i].texFilePath) == "sph") {
                sphFileName = pmdMaterials[i].texFilePath;
                texFileName = "";
            }
            else if (GetExtension(pmdMaterials[i].texFilePath) == "spa") {
                spaFileName = pmdMaterials[i].texFilePath;
                texFileName = "";
            }
            else {
                texFileName = pmdMaterials[i].texFilePath;
            }
        }
        //  モデルとTextureパスからアプリケーションからのTextureパスを得る
        if (texFileName != "") {
            auto texFilePath = GetTexturePathFromModelAndTexPath(strModelPath, texFileName.c_str());
            m_textureResources[i] = m_device.dx12->GetTextureByPath(texFilePath.c_str());
        }
        if (sphFileName != "") {
            auto sphFilePath = GetTexturePathFromModelAndTexPath(strModelPath, sphFileName.c_str());
            m_sphResources[i] = m_device.dx12->GetTextureByPath(sphFilePath.c_str());
        }
        if (spaFileName != "") {
            auto spaFilePath = GetTexturePathFromModelAndTexPath(strModelPath, spaFileName.c_str());
            m_spaResources[i] = m_device.dx12->GetTextureByPath(spaFilePath.c_str());
        }
    }
    fclose(fp);
}
