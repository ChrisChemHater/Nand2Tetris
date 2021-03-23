#include "myutils.h"
#include "head.h"

vector<string> splitCommand(const string& s) {
	// find the position of comment
	size_t comPos = s.find("//");
	if (comPos == string::npos) comPos = s.size();
	vector<string> result;
	bool newWord = true;
	for (size_t i=0; i < comPos; i++) {
		if (isblank(s[i])) {
			newWord = true;
		}
		else {
			if (newWord) {
				newWord = false;
				result.push_back("");
				result.back().push_back(s[i]);
			}
			else {
				result.back().push_back(s[i]);
			}
		}
	}
	return result;
}
string getfilename(const string& fullname) {
	size_t pos_back = fullname.rfind('\\');
	size_t pos_slash = fullname.rfind("/");
	if (pos_back == string::npos) pos_back = 0;
	if (pos_slash == string::npos) pos_slash = 0;
	pos_slash = pos_slash > pos_back ? pos_slash : pos_back;
	return fullname.substr(pos_slash + 1, fullname.size() - pos_slash - 1);
}
bool isDigitString(const string& s) {
	for (char c : s) {
		if (!isdigit(c)) return false;
	}
	return true;
}

Parser::Parser(const char* file) :_file(ifstream(file)), _fname(file), lineNum(0), _moreCommands(true) {
	if (!_file) {
		cout << file << ": file doesn't exist!";
		abort();
	}
	_fname = getfilename(_fname);
	_fname.resize(_fname.size() - 3);
	advance();
}
bool Parser::hasMoreCommands() {
	return _moreCommands;
}
void Parser::advance() {
	while (getline(_file, line)) {
		lineNum++;
		vector<string> args = splitCommand(line);
		switch (args.size())
		{
		case 0:
			continue;
		case 1:
			if (args[0] == "return") {
				_cmdtype = C_RETURN;
			}
			else {
				_cmdtype = C_ARITH;
				_arg1 = args[0];
			}
			return;
		case 2:
			if (args[0] == "label")_cmdtype = C_LABEL;
			else if (args[0] == "goto")_cmdtype = C_GOTO;
			else if (args[0] == "if-goto")_cmdtype = C_IF;
			else throw VMTranslateError("syntax error", lineNum, line.c_str());
			_arg1 = args[1];
			return;
		case 3:
			if (args[0] == "push") _cmdtype = C_PUSH;
			else if (args[0] == "pop")_cmdtype = C_POP;
			else if (args[0] == "function") { 
				_cmdtype = C_FUNCTION;
				_funcName.assign(args[1]);
			}
			else if (args[0] == "call")_cmdtype = C_CALL;
			else throw VMTranslateError("syntax error", lineNum, line.c_str());
			_arg1 = args[1];
			if (!isDigitString(args[2])) throw VMTranslateError("3rd argument must be non-negtive int", lineNum, line.c_str());
			_arg2 = atoi(args[2].c_str());
			return;
		default:
			throw VMTranslateError("syntax error", lineNum, line.c_str());
		}
	}
	_moreCommands = false;
}
CommandType Parser::getCommandType() {
	return _cmdtype;
}
string Parser::getClassName() {
	return _fname;
}
string Parser::getFuncName() {
	return _funcName;
}
string Parser::arg1() {
	return _arg1;
}
unsigned Parser::arg2() {
	return _arg2;
}
void Parser::close() {
	_file.close();
}