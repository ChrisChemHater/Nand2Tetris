#include "head.h"

AssembleError::AssembleError(const char* error_msg, unsigned line_num, const char* line_) : errorMsg(error_msg), lineNum(line_num), line(line_) {
	cout << "Assemble Error:" << endl;
	cout << errorMsg << " in line " << lineNum << ": " << line << endl;
}
AssembleError::AssembleError(const char* error_msg) : errorMsg(error_msg) {
	cout << "Assemble Error:" << endl;
	cout << errorMsg << endl;
}
