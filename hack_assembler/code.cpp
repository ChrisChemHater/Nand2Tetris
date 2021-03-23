#include "head.h"

Code::Code() {
	destTable[""] = 0b0000000000000000;
	destTable["M"] = 0b0000000000001000;
	destTable["D"] = 0b0000000000010000;
	destTable["MD"] = 0b0000000000011000;
	destTable["A"] = 0b0000000000100000;
	destTable["AM"] = 0b0000000000101000;
	destTable["AD"] = 0b0000000000110000;
	destTable["AMD"] = 0b0000000000111000;

	compTable["0"] = 0b1110101010000000;
	compTable["1"] = 0b1110111111000000;
	compTable["-1"] = 0b1110111010000000;
	compTable["D"] = 0b1110001100000000;
	compTable["A"] = 0b1110110000000000;
	compTable["!D"] = 0b1110001101000000;
	compTable["!A"] = 0b1110110001000000;
	compTable["-D"] = 0b1110001111000000;
	compTable["-A"] = 0b1110110011000000;
	compTable["D+1"] = 0b1110011111000000;
	compTable["A+1"] = 0b1110110111000000;
	compTable["D-1"] = 0b1110001110000000;
	compTable["A-1"] = 0b1110110010000000;
	compTable["D+A"] = 0b1110000010000000;
	compTable["D-A"] = 0b1110010011000000;
	compTable["A-D"] = 0b1110000111000000;
	compTable["D&A"] = 0b1110000000000000;
	compTable["D|A"] = 0b1110010101000000;

	compTable["M"] = 0b1111110000000000;
	compTable["!M"] = 0b1111110001000000;
	compTable["-M"] = 0b1111110011000000;
	compTable["M+1"] = 0b1111110111000000;
	compTable["M-1"] = 0b1111110010000000;
	compTable["D+M"] = 0b1111000010000000;
	compTable["D-M"] = 0b1111010011000000;
	compTable["M-D"] = 0b1111000111000000;
	compTable["D&M"] = 0b1111000000000000;
	compTable["D|M"] = 0b1111010101000000;

	jumpTable[""] = 0b0000000000000000;
	jumpTable["JGT"] = 0b0000000000000001;
	jumpTable["JEQ"] = 0b0000000000000010;
	jumpTable["JGE"] = 0b0000000000000011;
	jumpTable["JLE"] = 0b0000000000000100;
	jumpTable["JNE"] = 0b0000000000000101;
	jumpTable["JLE"] = 0b0000000000000110;
	jumpTable["JMP"] = 0b0000000000000111;
}
string Code::errorHandle(const string& errorMsg, const string& part) {
	return errorMsg + part;
}
code_16 Code::dest(const string& dest_) {
	map<string, code_16>::iterator p = destTable.find(dest_);
	if (p == destTable.end()) throw runtime_error(errorHandle("can't resolve dest: ", dest_));
	return p->second;
}
code_16 Code::comp(const string& comp_) {
	map<string, code_16>::iterator p = compTable.find(comp_);
	if (p == compTable.end()) throw runtime_error(errorHandle("can't resolve comp: ", comp_));
	return p->second;
}
code_16 Code::jump(const string& jump_) {
	map<string, code_16>::iterator p = jumpTable.find(jump_);
	if (p == jumpTable.end()) throw runtime_error(errorHandle("can't resolve jump: ", jump_));
	return p->second;
}