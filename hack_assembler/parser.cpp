#include "head.h"

Parser::Parser(const char* filename) : file(ifstream(filename)), pc(0), lineNum(0), moreCommand(true) {
	if (!file) {
		cout << "File doesn't exist";
		abort();
	}
	advance();
}
void Parser::refreshCommand() {
	command.clear();
	for (string::const_iterator i = line.begin(); i != line.end(); i++) {
		if (!(*i >= 33 && *i <= 127)) continue;
		if (*i == '/') {
			if (i + 1 == line.end() || *(i + 1) != '/') throw AssembleError("comment syntax error", lineNum, line.c_str());
			else return;
		}
		command.push_back(*i);
	}
}
bool Parser::hasMoreCommands() {
	return moreCommand;
}
void Parser::advance() {
	while (getline(file, line)) {
		lineNum++;
		refreshCommand();
		if (!command.empty()) {
			_commandType();
			if (cmdtype == CType) resolveCcmd();
			else resolveALcmd();
			if (cmdtype != LType) pc++;
			return;
		}
	}
	moreCommand = false;
}
void Parser::reset() {
	file.clear();
	file.seekg(0);
	line.clear();
	command.clear();
	lineNum = 0;
	pc = 0;
	moreCommand = true;
	advance();
}
void Parser::close() {
	file.close();
}
void Parser::_commandType() {
	if (command.front() == '@') cmdtype = AType;
	else if (command.front() == '(') {
		if (command.back() == ')') cmdtype = LType;
		else throw AssembleError("label syntax error", lineNum, line.c_str());
	}
	else cmdtype = CType;
}
CommandType Parser::commandType() {
	return cmdtype;
}
void Parser::resolveALcmd(){
	sym.clear();
	if (cmdtype == AType) {
		copy(command.begin() + 1, command.end(), back_inserter(sym));
	}
	else if (cmdtype == LType) {
		copy(command.begin() + 1, command.end() - 1, back_inserter(sym));
	}
	else throw runtime_error("Wrong command type: command type must be AType or LType");
	// valify the symbol or address.
	if (sym.empty()) throw AssembleError("the symbol or address mustn't be empty", lineNum, line.c_str());
	// whether symbol or address
	bool is_address(true);
	for (char c : sym) {
		if (c < '0' || c > '9') {
			is_address = false;
			break;
		}
	}
	// A label mustn't be constant type
	if (is_address && cmdtype == LType) {
		throw AssembleError("the label mustn't be a constant", lineNum, line.c_str());
	}
	// A symbol must start with a letter and contains only letters, digits, '_', '.', '$' and ':'
	if (!is_address) {
		if (sym.front() >= '0' && sym.front() <= '9') throw AssembleError("the symbol mustn't begin with digits", lineNum, line.c_str());
		for (char i : sym) {
			if (!((i >= 'A' && i <= 'Z')
				|| (i >= 'a' && i <= 'z')
				|| (i >= '0' && i <= '9')
				|| i == '_'
				|| i == '.'
				|| i == '$'
				|| i == ':'
				))
				throw AssembleError("the symbol must contains only letters, digits, '_', '.', '$' and ':'", lineNum, line.c_str());
		}
	}
}
void Parser::resolveCcmd() {
	ccmd.dest.clear();
	ccmd.comp.clear();
	ccmd.jump.clear();
	string::iterator equal = find(command.begin(), command.end(), '=');
	string::iterator semi = find(command.begin(), command.end(), ';');
	if (equal == command.end()) {  // no '='
		if (semi == command.end()) {  // no '=' and no ';'
			ccmd.comp = command;
		}
		else {  // no '=' but ';' yes
			copy(command.begin(), semi, back_inserter(ccmd.comp));
			copy(semi + 1, command.end(), back_inserter(ccmd.jump));
		}
	}
	else {  // '=' yes
		if (semi == command.end()) {  // '=' yes but no ';'
			copy(command.begin(), equal, back_inserter(ccmd.dest));
			copy(equal + 1, command.end(), back_inserter(ccmd.comp));
		}
		else {  // '=' yes and ';' yes
			if (equal > semi) throw AssembleError("C command syntax error", lineNum, line.c_str());
			copy(command.begin(), equal, back_inserter(ccmd.dest));
			copy(equal + 1, semi, back_inserter(ccmd.comp));
			copy(semi + 1, command.end(), back_inserter(ccmd.jump));
		}
	}
}
string Parser::symbol() {
	return sym;
}
string Parser::dest() {
	return ccmd.dest;
}
string Parser::comp() {
	return ccmd.comp;
}
string Parser::jump() {
	return ccmd.jump;
}