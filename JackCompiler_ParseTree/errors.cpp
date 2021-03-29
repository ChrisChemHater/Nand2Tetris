#include "myutils.h"

CompileError::CompileError(const string& errorMsg, unsigned lineNum) :errorMsg(errorMsg), lineNum(lineNum) {}
void CompileError::printErrorMag() {
	cout << "line " << lineNum << ": " << errorMsg << endl;
}

SyntaxError::SyntaxError(const string& errorMsg, unsigned lineNum) : CompileError(errorMsg, lineNum) {
	cout << "syntax error" << endl;
	printErrorMag();
}
OverflowError::OverflowError(const string& errorMsg, unsigned lineNum): CompileError(errorMsg, lineNum) {
	cout << "int overflow" << endl;
	printErrorMag();
}