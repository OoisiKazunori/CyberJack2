#pragma once
#include"../DirectXCommon/Base.h"
#include<string>
/*
*主な役割
文字を簡単に、出力元に表示させる
*/
//出す結果によって、引数に入った文字列と結果の文字列をつなげて出力する
HRESULT CheckResult(std::string OUTPUT_DEBUG_STRING, HRESULT RESULT);

void OutPutStartDebugString(std::string OUTPUT_DEBUG_STRING);
void OutPutEndDebugString(std::string OUTPUT_DEBUG_STRING);

HRESULT CheckResult(HRESULT RESULT, std::string SUCCEED_STRING, std::string FAIL_STRING);
void FailCheck(std::string FAIL_STRING);
void SucceedCheck(std::string SUCCEED_STRING);
void ErrorCheck(std::string ERROR_STRING);