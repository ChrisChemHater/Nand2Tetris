#pragma once
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <map>
#include <set>
#include <vector>
#include <string>
#include <boost/format.hpp>
#include <utility>
#include <cmdline.h>
#include <filesystem>
using namespace std;
using namespace filesystem;

enum CommandType {
	C_ARITH,
	C_PUSH, C_POP,
	C_LABEL, C_GOTO, C_IF,
	C_FUNCTION, C_RETURN, C_CALL
};

class Parser {
public:
	Parser(const char*);
	bool hasMoreCommands();
	void advance();
	CommandType getCommandType();
	string arg1();
	unsigned arg2();
	string getClassName();
	string getFuncName();
	void close();
	string line;
	unsigned long lineNum;
private:
	string _fname;
	string _funcName;
	CommandType _cmdtype;
	string _arg1;
	unsigned _arg2;
	bool _moreCommands;
	ifstream _file;
};

class CodeWriter {
public:
	CodeWriter();
	CodeWriter(const char*);
	void open(const char*);
	void setClassName(const string&);
	void writeInit();
	void writeArithmetic(const string&);
	void writePushPop(CommandType cmdtype, const string& segment, unsigned index);
	void writeComment(const string& comment);
	void writeLabel(const string& funcName, const string& label);
	void writeGoto(const string& funcName, const string& label);
	void writeIf(const string& funcName, const string& label);
	void writeCall(const string& funcName, unsigned numArgs);
	void writeRet();
	void writeFunc(const string& funcName, unsigned numLocals);
	void close();
	unsigned long lineNum;
private:
	string _fname;
	ofstream _file;
	unsigned _returnIdx = 0;
	unsigned _eqIdx = 0;
	unsigned _gtIdx = 0;
	unsigned _ltIdx = 0;
};