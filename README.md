# production-exhibition  

制作展展示予定作品  

●エラーで開けない場合は下記のように設定されているか見直してください  
プロジェクト→プロパティ  
・構成  
　Debug  
・プラットフォーム  
  x64  
・C/C++ → 全般 → 追加のインクルードディレクトリ  
 $(SolutionDir)inc\Debug  
 $(SolutionDir)inc\Debug\DirectXTex  
 $(SolutionDir)inc\Debug\DirectXTK12  
 $(SolutionDir)inc\Debug\Effekseer  
・リンカー → 全般 → 追加のライブラリディレクトリ  
 $(SolutionDir)lib\Debug  
 $(SolutionDir)lib\Debug\DirectXTex  
 $(SolutionDir)lib\Debug\DirectXTK12  
 $(SolutionDir)lib\Debug\Effekseer  
