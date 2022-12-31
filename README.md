## serving_robot

数個の変更を加えました。

### 変更内容
> serving.inoで実行中にEPROMに状況を書き込むようにした

> getData.inoで動かした後にEPROMからシリアルボードへ値を出力できるようにした。

> test.inoでアームの動きやモーターの動きを確かめるだけのコードを書いた

### 懸念事項
*コンパイルは通ったが、getData.inoでarduinoのRAMを超過している可能性あり*


## getData.inoはこんな感じでシリアルボードに可視化します。
Tinkercadは全角文字が文字化けするみたいですが、イメージはこんな感じです。

![image](https://user-images.githubusercontent.com/70757835/210130227-9fe7db6e-10a4-4004-b9bd-baed16bb4cc7.png)
