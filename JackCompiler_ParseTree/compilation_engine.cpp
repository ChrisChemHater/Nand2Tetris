#include "myutils.h"
#include "part1.h"

map<TokenType, string> labelLib({
	pair<TokenType, string>(T_KEYWORD, "keyword"),
	pair<TokenType, string>(T_SYMBOL, "symbol"),
	pair<TokenType, string>(T_INT_CONST, "intConst"),
	pair<TokenType, string>(T_STRING_CONST, "stringConst"),
	pair<TokenType, string>(T_IDENTIFIER, "identifier")
});
set<char> operatorLib({
	'+', '-', '*', '/', '&', '|', '<', '>', '='
});
set<char> unaryOpLib({ '-', '~' });
set<KeyWord> keywordConstLib({ J_TRUE,J_FALSE,J_NULL,J_THIS });
void CompilationEngine::writeLine(const string& line) {
	for (unsigned short i = 0; i < _recurDepth; i++) _file << "  ";
	_file << line << endl;
}
void CompilationEngine::_verifyTtype(initializer_list<TokenType> validTypes) {
	bool valid = false;
	for (TokenType t : validTypes) {
		if (_tkparser.getTokenType() == t) {
			valid = true;
			break;
		}
	}
	if (!valid) throw SyntaxError("Invalid token Type: " + _tkparser.currentToken, _tkparser.lineNum);
}
void CompilationEngine::_verifyTtype(TokenType validType) {
	if (_tkparser.getTokenType() != validType)
		throw SyntaxError("Invalid token Type: " + _tkparser.currentToken, _tkparser.lineNum);
}
void CompilationEngine::_verifyToken(initializer_list<string> valids) {
	bool valid = false;
	for (const string& s : valids) {
		if (_tkparser.currentToken == s) {
			valid = true;
			break;
		}
	}
	if (!valid) throw SyntaxError("invalid token: " + _tkparser.currentToken, _tkparser.lineNum);
}
void CompilationEngine::_verifyToken(const string& valid) {
	if (_tkparser.currentToken != valid)
		throw SyntaxError("invalid token: " + _tkparser.currentToken, _tkparser.lineNum);
}
void CompilationEngine::_verifyKeyWord(KeyWord valid) {
	if (_tkparser.getKeyWord() != valid)
		throw SyntaxError("invalid keyword: " + _tkparser.currentToken, _tkparser.lineNum);
}
void CompilationEngine::_verifyKeyWord(initializer_list<KeyWord> valids) {
	bool valid = false;
	for (KeyWord s : valids) {
		if (_tkparser.getKeyWord() == s) {
			valid = true;
			break;
		}
	}
	if (!valid) throw SyntaxError("invalid keyword: " + _tkparser.currentToken, _tkparser.lineNum);
}
void CompilationEngine::writeRightLabel(const string& label) {
	stringstream slabel;
	slabel << "</" << label << '>';
	writeLine(slabel.str());
}
void CompilationEngine::writeLeftLabel(const string& label) {
	stringstream slabel;
	slabel << '<' << label << '>';
	writeLine(slabel.str());
}
CompilationEngine::CompilationEngine(const char* infile, const char* outfile) :
	_tkparser(infile), _file(outfile) {}
void CompilationEngine::compileClass() {
	writeLeftLabel("class");
	_recurDepth++;
	compileTerminal();
	_verifyTtype(T_IDENTIFIER);
	compileTerminal();
	_verifyToken("{");
	compileTerminal();
	while (_tkparser.getTokenType() == T_KEYWORD && (
		_tkparser.getKeyWord() == J_STATIC ||
		_tkparser.getKeyWord() == J_FIELD
		)) {
		compileVarDec();
	}
	while (_tkparser.getTokenType() == T_KEYWORD && (
		_tkparser.getKeyWord() == J_CONSTRUCTOR ||
		_tkparser.getKeyWord() == J_FUNCTION ||
		_tkparser.getKeyWord() == J_METHOD
		)) {
		compileSubroutine();
	}
	_verifyToken("}");
	compileTerminal();
	_recurDepth--;
	writeRightLabel("class");
}
void CompilationEngine::compileClassVarDec() {
	writeLeftLabel("classVarDec");
	_recurDepth++;
	compileTerminal();
	compileType();
	_verifyTtype(T_IDENTIFIER);
	compileTerminal();
	while (_tkparser.currentToken.front() == ',') {
		compileTerminal();
		_verifyTtype(T_IDENTIFIER);
		compileTerminal();
	}
	_verifyToken(";");
	compileTerminal();
	_recurDepth--;
	writeRightLabel("classVarDec");
}
void CompilationEngine::compileSubroutine() {
	writeLeftLabel("subroutineDec");
	_recurDepth++;
	compileTerminal();
	if (_tkparser.getTokenType() == T_KEYWORD && _tkparser.getKeyWord() == J_VOID)
		compileTerminal();
	else compileType();
	_verifyTtype(T_IDENTIFIER);
	compileTerminal();
	_verifyToken("(");
	compileTerminal();
	complieParamList();
	_verifyToken(")");
	compileTerminal();

	writeLeftLabel("subroutineBody");
	_recurDepth++;
	_verifyToken("{");
	compileTerminal();
	while (_tkparser.getTokenType() == T_KEYWORD && _tkparser.getKeyWord() == J_VAR) {
		compileVarDec();
	}
	compileStatements();
	_verifyToken("}");
	compileTerminal();
	_recurDepth--;
	writeRightLabel("subroutineBody");
	_recurDepth--;
	writeRightLabel("subroutineDec");
}
void CompilationEngine::complieParamList() {
	writeLeftLabel("parameterList");
	_recurDepth++;
	while (true) {
		switch (_tkparser.getTokenType()) {
		case T_KEYWORD:
			_verifyKeyWord({ J_INT, J_CHAR, J_BOOLEAN });
			break;
		case T_IDENTIFIER:
			break;
		default:
			_recurDepth--;
			writeRightLabel("parameterList");
			return;
		}
		compileTerminal();
		_verifyTtype(T_IDENTIFIER);
		compileTerminal();
		if (_tkparser.currentToken != ",") {
			_recurDepth--;
			writeRightLabel("parameterList");
			return;
		}
		compileTerminal();
	}
	_recurDepth--;
	writeRightLabel("parameterList");
}
void CompilationEngine::compileVarDec() {
	writeLeftLabel("varDec");
	_recurDepth++;
	compileTerminal();
	compileType();
	_verifyTtype(T_IDENTIFIER);
	compileTerminal();
	while (_tkparser.currentToken.front() == ',') {
		compileTerminal();
		_verifyTtype(T_IDENTIFIER);
		compileTerminal();
	}
	_verifyToken(";");
	compileTerminal();
	_recurDepth--;
	writeRightLabel("varDec");
}
void CompilationEngine::compileStatements() {
	writeLeftLabel("statements");
	_recurDepth++;
	while (_tkparser.getTokenType() == T_KEYWORD) {
		switch (_tkparser.getKeyWord())
		{
		case J_LET:
			compileLet();
			break;
		case J_IF:
			compileIf();
			break;
		case J_WHILE:
			compileWhile();
			break;
		case J_DO:
			compileDo();
			break;
		case J_RETURN:
			compileReturn();
			break;
		default:
			_recurDepth--;
			writeRightLabel("statements");
			return;
		}
	}
	_recurDepth--;
	writeRightLabel("statements");
}
void CompilationEngine::compileDo() {
	writeLeftLabel("doStatement");
	_recurDepth++;
	compileTerminal();
	_verifyTtype(T_IDENTIFIER);
	compileTerminal();
	if (_tkparser.currentToken == ".") {
		compileTerminal();
		_verifyTtype(T_IDENTIFIER);
		compileTerminal();
	}
	_verifyToken("(");
	compileTerminal();
	compileExprList();
	_verifyToken(")");
	compileTerminal();
	_verifyToken(";");
	compileTerminal();
	_recurDepth--;
	writeRightLabel("doStatement");
}
void CompilationEngine::compileLet() {
	writeLeftLabel("letStatement");
	_recurDepth++;
	compileTerminal();
	_verifyTtype(T_IDENTIFIER);
	compileTerminal();
	if (_tkparser.currentToken == "[") {
		compileTerminal();
		compileExpr();
		_verifyToken("]");
		compileTerminal();
	}
	_verifyToken("=");
	compileTerminal();
	compileExpr();
	_verifyToken(";");
	compileTerminal();
	_recurDepth--;
	writeRightLabel("letStatement");
}
void CompilationEngine::compileWhile() {
	writeLeftLabel("whileStatement");
	_recurDepth++;
	compileTerminal();
	_verifyToken("(");
	compileTerminal();
	compileExpr();
	_verifyToken(")");
	compileTerminal();
	_verifyToken("{");
	compileTerminal();
	compileStatements();
	_verifyToken("}");
	compileTerminal();
	_recurDepth--;
	writeRightLabel("whileStatement");
}
void CompilationEngine::compileReturn() {
	writeLeftLabel("returnStatement");
	_recurDepth++;
	compileTerminal();
	if (_tkparser.currentToken != ";") {
		compileExpr();
	}
	_verifyToken(";");
	compileTerminal();
	_recurDepth--;
	writeRightLabel("returnStatement");
}
void CompilationEngine::compileIf() {
	writeLeftLabel("ifStatement");
	_recurDepth++;
	compileTerminal();
	_verifyToken("(");
	compileTerminal();
	compileExpr();
	_verifyToken(")");
	compileTerminal();
	_verifyToken("{");
	compileTerminal();
	compileStatements();
	_verifyToken("}");
	compileTerminal();
	if (_tkparser.getTokenType() == T_KEYWORD && _tkparser.getKeyWord() == J_ELSE) {
		compileTerminal();
		_verifyToken("{");
		compileTerminal();
		compileStatements();
		_verifyToken("}");
		compileTerminal();
	}
	_recurDepth--;
	writeRightLabel("ifStatement");
}
void CompilationEngine::compileExpr() {
	writeLeftLabel("expression");
	_recurDepth++;
	compileTerm();
	while (_tkparser.getTokenType() == T_SYMBOL
		&& operatorLib.find(_tkparser.currentToken.front()) != operatorLib.end()) {
		compileTerminal();
		compileTerm();
	}
	_recurDepth--;
	writeRightLabel("expression");
}
void CompilationEngine::compileTerm() {
	switch (_tkparser.getTokenType())
	{
	case T_INT_CONST:
	case T_STRING_CONST:
		compileTerminal();
		break;
	case T_KEYWORD:
		if (keywordConstLib.find(_tkparser.getKeyWord()) == keywordConstLib.end())
			throw SyntaxError("Can't resolve term: " + _tkparser.currentToken, _tkparser.lineNum);
		compileTerminal();
		break;
	case T_SYMBOL:
		if (_tkparser.currentToken.front() == '(') {
			compileTerminal();
			compileExpr();
			_verifyToken(")");
			compileTerminal();
			break;
		}
		if (unaryOpLib.find(_tkparser.currentToken.front()) == unaryOpLib.end())
			throw SyntaxError("Can't resolve operator: " + _tkparser.currentToken, _tkparser.lineNum);
		compileTerminal();
		compileTerm();
		break;
	case T_IDENTIFIER:
		compileTerminal();
		if (_tkparser.currentToken == "[") {
			compileTerminal();
			compileExpr();
			_verifyToken("]");
			compileTerminal();
		}
		else if (_tkparser.currentToken == "(") {
			compileTerminal();
			compileExprList();
			_verifyToken(")");
			compileTerminal();
		}
		else if (_tkparser.currentToken == ".") {
			compileTerminal();
			_verifyTtype(T_IDENTIFIER);
			compileTerminal();
			_verifyToken("(");
			compileTerminal();
			compileExprList();
			_verifyToken(")");
			compileTerminal();
		}
		break;
	}
}
void CompilationEngine::compileExprList() {
	writeLeftLabel("expressionList");
	_recurDepth++;
	if (_tkparser.currentToken == ")") {
		_recurDepth--;
		writeRightLabel("expressionList");
		return;
	}
	while (true) {
		compileExpr();
		if (_tkparser.currentToken != ",") {
			_recurDepth--;
			writeRightLabel("expressionList");
			return;
		}
		compileTerminal();
	}
	_recurDepth--;
	writeRightLabel("expressionList");
}
void CompilationEngine::compileType() {
	switch (_tkparser.getTokenType()) {
	case T_KEYWORD:
		_verifyKeyWord({ J_INT, J_CHAR, J_BOOLEAN });
		break;
	case T_IDENTIFIER:
		break;
	default:
		throw SyntaxError("invalid token: " + _tkparser.currentToken, _tkparser.lineNum);
	}
	compileTerminal();
}
void CompilationEngine::compileTerminal() {
	if (!_tkparser.hasMoreCommand())
		throw SyntaxError("Unexpected ending", _tkparser.lineNum);
	string label = labelLib[_tkparser.getTokenType()];
	stringstream sline;
	sline << '<' << label << "> "
		<< _tkparser.currentToken
		<< " </" << label << '>';
	writeLine(sline.str());
	_tkparser.advance();
}
void CompilationEngine::compileTerminal(initializer_list<string> valids) {
	bool valid = false;
	for (const string& s : valids) {
		if (_tkparser.currentToken == s) {
			valid = true;
			break;
		}
	}
	if (!valid) throw SyntaxError("invalid token: " + _tkparser.currentToken, _tkparser.lineNum);
	compileTerminal();
}
void CompilationEngine::close() {
	_tkparser.close();
	_file.close();
}