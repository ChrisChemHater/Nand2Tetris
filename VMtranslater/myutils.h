#pragma once
#include <iostream>
#include <vector>
using namespace std;

vector<string> splitCommand(const string& s);
string getfilename(const string& fullname);

class VMTranslateError {
public:
	VMTranslateError(const char* error_msg, unsigned line_num, const char* line_);
	VMTranslateError(const char* error_msg);
	const char* errorMsg;
	unsigned lineNum;
	const char* line;
};