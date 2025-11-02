(現在大型アプデ中。よって以下に書かれているものは全て嘘です。)

 # 説明(2025/10/05更新)
 このエンジンは、PSOの変更に重きを置いたエンジンです。
- [何かを描画するためには](#h1)
- [Resource(Texture,Model)について](#h2)
- [Reosurce(ID3D12Resource)について](#h3)
- [新しいShaderの追加](#h4)

<h2 id="h1">何かを描画するためには</h2>

 とりあえず何かを描画したいあなたは、DrawResourceがおすすめです。

 DrawResourceを任意のEngine/Scene/Data/BaseSceneを継承したクラスにインスタンスを作成して、必要事項をpublicのメンバ変数に記載してください。

 その後、RenderでPreDrawした後Drawしてください。

 わかりづらいですよね。詳しくは以下のサンプルコードを見てください。
 ```c++
void TestScene::Initialize() {
    drawResource = std::make_unique<DrawResource>();
    //DrawResourceの初期化。引数は、[頂点数][インデックスの数][GPUで行列を計算するかどうか]
    drawResource->Initialize(3 /*, 0, true*/ );
    //任意のローカルポジション
    drawResource->localPosition = {
        {-0.5f, 0.0f, 0.0f},
        {0.0f, 0.5f, 0.0f},
        {0.5f, 0.0f, 0.0f}
    };
}

void TestScene::Update(){

}

void TestScene::Draw(){
    Render::PreDraw();
    render_->Draw([作成したBaseScene]);
}
 ```
 これは最低限の設定でswapchainに三角形を描画するコードです。

 これをいい感じに設定項目を増やしていってください。

<h2 id1="h2">Resource(Texture, Model, etc)について</h2>
 このエンジンには、プログラマともあろうことかResourceという単語に二つの意味が存在しています。

 一つはテクスチャやモデルなどのResource、もう一つはID3D12Resourceのことです。

 以下は、テクスチャやモデルなどのResourceについて説明します。

 ### 1.読み込み方
 BaseSceneのメンバ変数である--Managerでパスを入力してインデックスを取得してください。このインデックスをDrawResourceなどにつっこむと読み込むことができます。

 ### 2.詳細な情報
 Textureについての詳細な情報が、ImGui::Imageなどで要求されることがあると思います。そういう時に必要な情報も、--Managerが管理しています。

 Get--DataにIndexを突っ込むと--Dataというクラスが得られます。おそらくその中にあなたの望んでいる情報があることでしょう。

 以下、サンプルコードです
 ```c++
 //現在読み込めるのはpngのみ
 //Assets/Texture/は入力する必要なし
int textureHandle = textureManager->LoadTexture("DemoTexture.png");
//Texutreの詳細情報の取得
TextureData* textureData = textureManager->GetTextureData(textureHandle);

//Modelを追加するには、.obj,.mtl,.mtlに登録されているテクスチャをそれぞれ収める必要がある
//入力するのはフォルダ名のみ。Assets/Model/は入力する必要なし
int modelHandle = modelManager->LoadModel("DemoModel");
//モデルの詳細情報の取得
ModelData* modelData = modelManager->GetModelData(modelHandle);
 ```

 詳細情報はそれぞれポインタが渡されるので、この中の情報を変更すると実際の情報も変更されるので注意してください。

<h2 id="h3">Resource(ID3D12Resource)について</h2>
 このエンジンには、プログラマともあろうことかResourceという単語に二つの意味が存在しています。

 一つはテクスチャやモデルなどのResource、もう一つはID3D12Resourceのことです。

 以下は、このエンジンでのID3D12Resourcenの取り扱いについて説明します。

 ### 基本的な取り扱い方
 各特徴を表でまとめましたので、こちらを参照してください

| 名前          | 特徴                                                                 | 
|-------------|----------------------------------------------------------------------|
| DrawResource   | 頂点とインデックス,MatrixData,MaterialData,DirectionalLightDataを取り込める。迷ったらこれ  |
| ModelResource  | DrawResourceをモデルから作成できる。                                         |
| PerticleResource | ParticleDataを配列でVertexShaderに送信できる。 大量描画なら一番軽い          | 
| PostEffectResource | PostEffectをするだけ。画面いっぱいに画像を表示して、Shader効果をかける      |

これを参考にResourceを選んでください。指定できるパラメータはすべてpublicにありますので、それを参考に作成してください。ペアレントがしたければ仕様をいじらないといけないので薫森に声かけてください。

<h2 id="h3">新しいシェーダーの追加について</h2>
このエンジンで一番力入れてるのがこれです。できるだけ簡易化したので、使ってくれると嬉しいです。

1. ShaderをAssets/Shader/に追加する
1. RootSignatureIDにIDを追加して、Shader用のRootSignatureを作成して、登録したIDに代入する
1. InputLayoutIDにIDを追加して、Shader用のInputLayoutを作成して、登録したIDに代入する
1. Engine/Scene/SceneEditScene.cppのrootSignatures_に作成したRootSignatureの名前を登録する
1. main.cppのBootModeをSceneEditに変更して起動
1. シェーダーとRootSignature、InputLayoutの組み合わせを追加して「バツを押して」終了する
1. BootModeをGameに戻す

これでShaderの登録が完了し、新しくPSOが作成されるようになります。

使用する場合は、BaseResource::PSOConfigの中にあるシェーダー名とInputLayoutID、RootSignatureIDを変更してから描画してください。シェーダーのスペルミスには気を付けてください。

&nbsp;  

## Master Branch Build State

| 状態       | Debug                                                                 | Development                                                                 | Release                                                                 |
|------------|----------------------------------------------------------------------|-----------------------------------------------------------------------------|------------------------------------------------------------------------|
| CI / CD    | [![DebugBuild](https://github.com/ShigemoriMasato/CG2/actions/workflows/DebugBuild.yml/badge.svg)](https://github.com/ShigemoriMasato/CG2/actions/workflows/DebugBuild.yml) | [![DevelopmentBuild](https://github.com/ShigemoriMasato/CG2/actions/workflows/DevelopmentBuild.yml/badge.svg)](https://github.com/ShigemoriMasato/CG2/actions/workflows/DevelopmentBuild.yml) | [![ReleaseBuild](https://github.com/ShigemoriMasato/CG2/actions/workflows/ReleaseBuild.yml/badge.svg)](https://github.com/ShigemoriMasato/CG2/actions/workflows/ReleaseBuild.yml) |
