#include "myutils.h"
#include "part1.h"

set<char> symbolLib({ '{','}','(',')','[',']','.'
	,',',';','+','-','*','/','&','|','<','>','=','~'
});
map<string, KeyWord> keywordLib({
	pair<string, KeyWord>("class", J_CLASS),
	pair<string, KeyWord>("method", J_METHOD),
	pair<string, KeyWord>("function", J_FUNCTION),
	pair<string, KeyWord>("constructor", J_CONSTRUCTOR),
	pair<string, KeyWord>("int", J_INT),
	pair<string, KeyWord>("boolean", J_BOOLEAN),
	pair<string, KeyWord>("char", J_CHAR),
	pair<string, KeyWord>("void", J_VOID),
	pair<string, KeyWord>("var", J_VAR),
	pair<string, KeyWord>("static", J_STATIC),
	pair<string, KeyWord>("field", J_FIELD),
	pair<string, KeyWord>("let", J_LET),
	pair<string, KeyWord>("do", J_DO),
	pair<string, KeyWord>("if", J_IF),
	pair<string, KeyWord>("else", J_ELSE),
	pair<string, KeyWord>("while", J_WHILE),
	pair<string, KeyWord>("return", J_RETURN),
	pair<string, KeyWord>("true", J_TRUE),
	pair<string, KeyWord>("false", J_FALSE),
	pair<string, KeyWord>("null", J_NULL),
	pair<string, KeyWord>("this", J_THIS),
});
bool isDigitString(const string& s) {
	for (char c : s) {
		if (!isdigit(c)) return false;
	}
	return true;
}
JackTokenizer::JackTokenizer(const char* filename) :_file(filename, ios::binary) {
	if (!_file) {
		cout << filename << ": file doesn't exist!";
		abort();
	}
	advance();
}
void JackTokenizer::_resolveTtype() {
	//T_IDENTIFIER, T_SYMBOL and T_STRING_CONST is firstly roughly handled in advance()
	if (_ttype == T_SYMBOL) {
		if (symbolLib.find(currentToken.front()) == symbolLib.end()) throw SyntaxError("Unresolved symbol: " + currentToken, lineNum);
	}
	else if (_ttype == T_IDENTIFIER) {
		if (isDigitString(currentToken)) {
			if (!_verifyInt()) throw OverflowError("Invalid int const: " + currentToken, lineNum);
			_ttype = T_INT_CONST;
			return;
		}
		map<string, KeyWord>::iterator p = keywordLib.find(currentToken);
		if (p != keywordLib.end()) {
			_ttype = T_KEYWORD;
			_ktype = p->second;
			return;
		}
		else {
			if (!_verifyIdentifier()) throw SyntaxError("Invalid identifier: " + currentToken, lineNum);
		}
	}
}
bool JackTokenizer::_verifyIdentifier() {
	if (isdigit(currentToken.front())) return false;
	return true;
}
bool JackTokenizer::_verifyInt() {
	currentToken.erase(0, currentToken.find_first_not_of('0'));
	if (currentToken.empty()) currentToken.push_back('0');
	if (currentToken.size() > 5) return false;
	_int_const = atoi(currentToken.c_str());
	if (_int_const >= 0x8000) return false;
	return true;
}
bool JackTokenizer::hasMoreCommand() {
	return _moreCommand;
}
void JackTokenizer::advance() {
	currentToken.clear();
	char c = _file.get();
	char nextChar;
	bool lineComment = false;
	bool multilineComment = false;
	bool wordMode = false;
	bool stringMode = false;
	while (c != EOF) {
		nextChar = _file.get();
		// handle comment state
		if (lineComment) {
			if (c == '\n') {
				lineComment = false;
			}
		}
		else if (multilineComment) {
			if (c == '*' && nextChar == '/') {
				multilineComment = false;
				nextChar = _file.get();
			}
		}
		else if (wordMode) {
			if (isalnum(c) || c == '_') {
				currentToken.push_back(c);
				if (nextChar == EOF) {
					_resolveTtype();
					return;
				}
			}
			else {
				if (nextChar == EOF) {
					_file.clear();
					_file.seekg(-1, ios::end);
				}
				else {
					_file.seekg(-2, ios::cur);
					//_file.unget();
					//_file.unget();
				}
				_resolveTtype();
				return;
			}
		}
		else if (stringMode) {
			if (c == '\n') throw SyntaxError("missing double quote", lineNum);
			if (c == '"') {
				if (nextChar != EOF) _file.seekg(-1, ios::cur);
				return;
			}
			currentToken.push_back(c);
		}
		else {
			if (isalnum(c) || c == '_') {
				_ttype = T_IDENTIFIER; //roughly handled, might be identifier, int_const or keyword
				wordMode = true;
				currentToken.push_back(c);
			}
			else if (ispunct(c)) {
				if (c == '/' && nextChar == '/') {
					nextChar = _file.get();
					lineComment = true;
				}
				else if (c == '/' && nextChar == '*') {
					multilineComment = true;
					nextChar = _file.get();
				}
				else if (c == '"') {
					_ttype = T_STRING_CONST;
					stringMode = true;
				}
				else {
					_ttype = T_SYMBOL;
					currentToken.push_back(c);
					if (nextChar != EOF) _file.seekg(-1, ios::cur);
					_resolveTtype();
					return;
				}
			}
		}
		if (c == '\n') lineNum++;
		c = nextChar;
	}
	_moreCommand = false;
	if (multilineComment) throw SyntaxError("Multi-line comment must end with '*/'", lineNum);
	if (stringMode) throw SyntaxError("string const missing '\"'", lineNum);
}
TokenType JackTokenizer::getTokenType() {
	return _ttype;
}
KeyWord JackTokenizer::getKeyWord() {
	return _ktype;
}
char JackTokenizer::getSymbol() {
	return currentToken.front();
}
unsigned short JackTokenizer::getIntConst() {
	return _int_const;
}
string JackTokenizer::getStringConst() {
	return currentToken;
}
void JackTokenizer::close() {
	_file.close();
}