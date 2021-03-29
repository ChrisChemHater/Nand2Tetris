#pragma once
#include <string>
#include <fstream>
#include <sstream>
#include <set>
#include <map>
#include <filesystem>
//#include <boost/format.hpp>
using namespace std;
using namespace filesystem;

enum TokenType {
	T_KEYWORD, T_SYMBOL, T_IDENTIFIER, T_INT_CONST, T_STRING_CONST
};

enum KeyWord {
	J_CLASS, J_METHOD, J_FUNCTION, J_CONSTRUCTOR,
	J_INT, J_BOOLEAN, J_CHAR, J_VOID,
	J_VAR, J_STATIC, J_FIELD,
	J_LET, J_DO,
	J_IF, J_ELSE, J_WHILE,
	J_RETURN,
	J_TRUE, J_FALSE, J_NULL, J_THIS
};

extern set<char> symbolLib;
extern map<string, KeyWord> keywordLib;

class JackTokenizer {
public:
	JackTokenizer(const char* filename);
	void advance();
	bool hasMoreCommand();
	TokenType getTokenType();
	KeyWord getKeyWord();
	char getSymbol();
	string getIdentifier();
	unsigned short getIntConst();
	string getStringConst();
	void close();

	string currentToken;
	string line;
	unsigned lineNum = 1;
private:
	void _resolveTtype();
	bool _verifyIdentifier();
	bool _verifyInt();

	ifstream _file;
	bool _moreCommand = true;
	unsigned _int_const;
	TokenType _ttype;
	KeyWord _ktype;
};

class CompilationEngine {
public:
	CompilationEngine(const char* infile, const char* outfile);
	void compileClass();
	void compileClassVarDec();
	void compileSubroutine();
	void complieParamList();
	void compileVarDec();
	void compileStatements();
	void compileDo();
	void compileLet();
	void compileWhile();
	void compileReturn();
	void compileIf();
	void compileExpr();
	void compileTerm();
	void compileExprList();
	void compileType();
	void compileTerminal();
	void compileTerminal(initializer_list<string>);
	void close();

	JackTokenizer _tkparser;
private:
	void writeLine(const string&);
	void writeRightLabel(const string& label);
	void writeLeftLabel(const string& label);
	void _verifyTtype(initializer_list<TokenType>);
	void _verifyTtype(TokenType);
	void _verifyToken(initializer_list<string>);
	void _verifyToken(const string&);
	void _verifyKeyWord(KeyWord);
	void _verifyKeyWord(initializer_list<KeyWord>);

	short unsigned _recurDepth = 0;
	ofstream _file;
};