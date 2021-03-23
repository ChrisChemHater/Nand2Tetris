#pragma once
#include <string>
#include <fstream>
#include <set>
#include <map>
#include <algorithm>
#include <utility>
#include <stdexcept>
#include <iostream>
#include <ios>
#include <iomanip>
#include <boost/format.hpp>
using namespace std;

typedef unsigned short int code_16;
enum CommandType { AType, CType, LType };

struct Ccmd {
	string dest;
	string comp;
	string jump;
};

class AssembleError {
public:
	AssembleError(const char *error_msg, unsigned line_num, const char *line_);
	AssembleError(const char* error_msg);
	const char* errorMsg;
	unsigned lineNum;
	const char* line;
};

class Parser {
public:
	Parser(const char* filename);
	bool hasMoreCommands();
	void advance();
	void reset();
	void close();
	CommandType commandType();
	string symbol();
	string dest();
	string comp();
	string jump();
	string line;
	string command;
	unsigned long lineNum;
	unsigned long pc;  // = realPC + 1
private:
	void refreshCommand();
	void _commandType();
	void resolveCcmd();
	void resolveALcmd();
	ifstream file;
	CommandType cmdtype;
	string sym;
	Ccmd ccmd;
	bool moreCommand;
};

class Code {
public:
	Code();
	code_16 dest(const string&);
	code_16 comp(const string&);
	code_16 jump(const string&);
private:
	string errorHandle(const string&, const string&);
	map<string, code_16> destTable;
	map<string, code_16> compTable;
	map<string, code_16> jumpTable;
};

class SymbolTable {
public:
	SymbolTable();
	void addEntry(const string& symbol, code_16 address);
	void addAddr(code_16 address);
	bool containSym(const string& symbol);
	bool containAddr(const code_16 addr);
	code_16 getAddr(const string& symbol);
private:
	set<code_16> addrSet;
	map<string, code_16> symTable;
};
