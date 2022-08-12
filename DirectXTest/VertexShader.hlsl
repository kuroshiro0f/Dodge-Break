struct Output
{
    float4 svpos:SV_POSITION;   //  システム用頂点座標
    float4 pos:POSITION;        //  システム用頂点座標
    float4 normal:NORMAL0;      //  法線ベクトル
    float4 vnormal:NORMAL1;     //  法線ベクトル
    float2 uv:TEXCOORD;         //  UV値
    float3 ray:VECTOR;          //  ベクトル
};

//  定数バッファ
cbuffer SceneData : register(b0)
{
    matrix view;
    matrix proj;    //  ビュープロジェクション行列
    float3 eye;
};
//  ワールド変換行列をインスタンシング描画用に配列で宣言
//  NOTE: 最大描画数を要素数とする静的配列として宣言し、
//        この配列の中から描画数分だけ処理をしてインスタンシング描画を行っています。
//        そのため、最大描画数に満たない描画数の場合は、その差だけメモリの無駄がありますが、現状はこのシステムのまま進めます。
cbuffer Transform : register(b1) 
{
    matrix world[500];
}

Output BasicVS(float4 pos : POSITION, float4 normal : NORMAL, float2 uv : TEXCOORD, uint instanceID : SV_InstanceID)
{
    Output output;    //  ピクセルシェーダへ渡す値
    pos = mul(world[instanceID], pos);
    output.svpos = mul(mul(proj, view), pos);    //  シェーダでは列優先なので注意
    output.pos = mul(view, pos);
    normal.w = 0;    //  ここ重要(平行移動成分を無効にする)
    output.normal = mul(world[instanceID], normal);    //  法線にもワールド変換を行う
    output.vnormal = mul(view, output.normal);
    output.uv = uv;
    output.ray = normalize(pos.xyz - mul(view, eye));    //  視線ベクトル

    return output;
}