## serving_robot

数個の変更を加えました。

### 変更内容
> serving.inoで実行中にEPROMに状況を書き込むようにした

> getData.inoで動かした後にEPROMからシリアルボードへ値を出力できるようにした。

> test.inoでアームの動きやモーターの動きを確かめるだけのコードを書いた

### 懸念事項
~~コンパイルは通ったが、getData.inoでarduinoのRAMを超過している可能性あり~~

Flashから直接読み込める関数Fを使用することで、RAMを節約した。

## getData.inoはこんな感じでシリアルボードに可視化します。
いろいろ文字化けしてるけど雰囲気が分かるのでよし

![image](https://user-images.githubusercontent.com/70757835/210486453-d4419a81-4a3b-4cd6-b702-bd6b0b56bfa1.png)
