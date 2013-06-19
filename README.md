JoystickInput
=============

## 概要

主に C# からジョイスティック (ゲームパッド) の入力を取得するライブラリです。
取得部分を C++/CLI で記述しています。Windows のみ動作可能です。
x86/x64 アーキテクチャでの動作を確認しています。

## 解説

HID デバイスの生入力データ (RawInput) 取得を利用しています。
このため DirectInput 等追加のライブラリは不必要です。
また、複数のジョイスティック接続にも対応しています。切断感知はできません。
ボタン数はジョイスティックごとに変化します。

## 元となったプログラム

Using the Raw Input API to Process Joystick Input

By Alexander Böcken, 23 Apr 2011

http://www.codeproject.com/Articles/185522/Using-the-Raw-Input-API-to-Process-Joystick-Input

(元のライセンス: CPOL)

## ライセンス

MIT License
