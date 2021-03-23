#include "myutils.h"

VMTranslateError::VMTranslateError(const char* error_msg, unsigned line_num, const char* line_) : errorMsg(error_msg), lineNum(line_num), line(line_) {
	cout << "VMTranslate Error:" << endl;
	cout << errorMsg << " in line " << lineNum << ": " << line << endl;
}
VMTranslateError::VMTranslateError(const char* error_msg) : errorMsg(error_msg) {
	cout << "VMTranslate Error:" << endl;
	cout << errorMsg << endl;
}