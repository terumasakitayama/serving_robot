## serving_robot

### 2023/10/2に修正を行いました。
> 可読性の改善等直接的に関係のない部分の修正
> ドキュメントの追加


### **最新版はmodifiedブランチの中にあります。**
=======
### **最終協議会で使用したコードはnewブランチの中にあります。**

3rd commitの時点でいろいろ嘘を書いていたので修正しました。

**mainブランチが一番古いです**
> main→modified→newの順番です。
> それぞれのブランチでreadmeに書いてる学びや変更点、コードが大きく異なるのでマージはしないでおこうと思います。


#### 主な変更
> ヘッダファイルの使用を廃止
+ アナログピンの値(A0など)がcppファイルでうまいこと認識してくれなかったから


> ファイルごとにフォルダーを作成
+ arduinoではこうしないとうまくコンパイルしてくれない
+ ほかのファイルで使った関数が使えない(void loop()とかがうまく使えない)


> 関数の引数の間違い
+ POSITIONをstructと書いていたり、関数呼び出しの際に&を書いていなかったり
+ × robot_pos.x → 〇 robot_pos->x (アロー演算子に対する理解の浅さ)


> その他こまごました変更
+ グローバル変数の頭にg_をつける
+ 関数宣言でポインタの引数の頭にp_をつける
+ ドリンクやテーブルなどの座標の格納をsetup関数内に入れる(関数の外では定義しかできないので)
+ 書き間違いの修正



#### 結論
**コンパイルが通ったからヨシ‼**
> 他のブランチを見ればわかる通り、まったくヨシではない。


### 重要な追記
+ **Arduino UNOのアナログピンはデジタルピンの14~19として使える**
