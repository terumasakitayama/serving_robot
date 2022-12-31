## serving_robot

数個の変更を加えました。

### 変更内容
> serving.inoで実行中にEPROMに状況を書き込むようにした

> getData.inoで動かした後にEPROMからシリアルボードへ値を出力できるようにした。

> test.inoでアームの動きやモーターの動きを確かめるだけのコードを書いた

### 懸念事項
*コンパイルは通ったが、getData.inoでarduinoのRAMを超過している可能性あり*
