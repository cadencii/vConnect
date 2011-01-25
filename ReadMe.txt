v.Connect -STAND

 WORLD を使用した歌声合成ツール by HAL@修羅場P

[無印との違い]
合成にWORLDを使用するため、FFTWが必須になりました。

http://www.fftw.org/

からDLしてください。


WORLDはv.0.0.1を使用しています。詳しくは以下。

http://www.aspl.is.ritsumei.ac.jp/morise/world/


パス指定はUTAU音源と同じパスになります。
またテストの都合上analyzedが含まれているとそれより前のパスと判断されます。

それ以外はVSQメタテキストファイルへのパス、出力ファイルのパスで合成が行わ
れます。なお、WaveのI/Oはリトルエンディアン依存、FRQファイルはエンディアン
間での互換はありません。また、WORLDがWin依存なのでこちらもWin依存です。

[対応（予定）パラメータ::メモ用]
○ノートのプロパティ
　・PortamentoLength/Depth
　・Velocity
　・Lyric/VibratoHandle
　・Preutterance/VoiceOverlap
　・Attack/Decay
　　… 合成後の波形から音量の正規化を行うので破綻する可能性あり
○コントロールカーブ
　・DYN
　・BRE
　・GEN
　・PIT/PBS

○未実装
　・声質変換システム
　・BRI
　・CLE … これら二つはフィルターを周波数変換することで実装したい
　・OPE … F0スペクトル補償制御パラメータになる予定
　・Attack/Decayによる声質変化の補償

[細かな仕様]
UTAU音源下に母音表と、それに対応したvtd（VoiceTexturingData）ファイルがあ
る場合、そのファイルにしたがって声質変換が行われます。