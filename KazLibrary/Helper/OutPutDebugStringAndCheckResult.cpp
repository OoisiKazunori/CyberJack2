#include "OutPutDebugStringAndCheckResult.h"
#include"DebugColor.h"

HRESULT CheckResult(std::string OUTPUT_DEBUG_STRING, HRESULT RESULT) {
	
	std::string String = OUTPUT_DEBUG_STRING;

	if (RESULT == S_OK) {
		std::string SucceedString = "Ç…ê¨å˜ÇµÇ‹ÇµÇΩ\n";
		String += SucceedString;
		std::cout << green;
	}
	else {
		std::string FailedString = "Ç…é∏îsÇµÇ‹ÇµÇΩ\n";
		String += FailedString;
		std::cout << red;
	}

	const char *OutPutString = String.c_str();
	std::cout << OutPutString;
	std::cout << white;

	return RESULT;
}

void OutPutStartDebugString(std::string OUTPUT_DEBUG_STRING) {

	std::string String = OUTPUT_DEBUG_STRING;
	std::string StartString = "ÇäJénÇµÇ‹Ç∑\n";
	
	String += StartString;


	const char *OutPutString = String.c_str();
	printf(OutPutString);
}

void OutPutEndDebugString(std::string OUTPUT_DEBUG_STRING) {
	std::string String = OUTPUT_DEBUG_STRING;
	std::string StartString = "ÇèIóπÇµÇ‹Ç∑\n";

	String += StartString;

	const char *OutPutString = String.c_str();
	printf(OutPutString);
}

HRESULT CheckResult(HRESULT RESULT, std::string SUCCEED_STRING, std::string FAIL_STRING)
{
	std::string string;
	if (RESULT == S_OK) 
	{	
		std::cout << green;
		string = SUCCEED_STRING;
	}
	else
	{
		std::cout << red;
		string = FAIL_STRING;
	}

	std::cout << string;
	std::cout << white;
	return RESULT;
}

void FailCheck(std::string FAIL_STRING)
{
	std::cout << red;
	std::cout << FAIL_STRING;
	std::cout << white;
}

void SucceedCheck(std::string SUCCEED_STRING)
{
	std::cout << green;
	std::cout << SUCCEED_STRING;
	std::cout << white;
}

void ErrorCheck(std::string ERROR_STRING)
{
	std::cout << yellow;
	std::cout << ERROR_STRING;
	std::cout << white;
}
