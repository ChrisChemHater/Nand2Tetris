#pragma once
#include <iostream>
#include <string>
using namespace std;

class CompileError {
public:
	CompileError(const string& errorMsg, unsigned lineNum);
	//CompileError(const string& errorMsg);
	void printErrorMag();

	string errorMsg;
	unsigned lineNum;
};
class SyntaxError :public CompileError {
public:
	SyntaxError(const string& errorMsg, unsigned lineNum);
};
class OverflowError :public CompileError {
public:
	OverflowError(const string& errorMsg, unsigned lineNum);
};