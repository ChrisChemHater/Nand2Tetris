#include "head.h"

const char* help =
"Hack machine language assembler\n"
"v 1.1\n"
"Author: Chris\n\n"
"Usage:\n"
"-------------------------------------------------------------------------------------------\n"
"\thackasm XXX.asm [-c]\n"
"Auguments:\n"
"\t-c\tallow collision between constant adresses and variables."
"And the binary code (in text mode) will be saved under the current working directory\n\n"
"-------------------------------------------------------------------------------------------\n"
"Change Log:\n"
"2021-03-14 v1.1:\tadd an augument -c to alow collision between constant adresses and variables.";

string code_16Tostring(code_16 code) {
	string s("0000000000000000");
	for (short int i = 15; i >= 0; i--) {
		if (code & 0x0001U) s[i] = '1';
		code >>= 1;
	}
	return s;
}

int main(unsigned argn, char *argv[]) {
	bool allow_collide(false);
	if (argn == 2) 0;
	else if (argn == 3) {
		if (string(argv[2]) == "-c") allow_collide = true;
		else cout << help;
	}
	else cout << help;
	// valify the filename
	string fname(argv[1]);
	if (fname.size() < 4 || fname.substr(fname.size() - 4, 4) != ".asm") {
		cout << "the assemble filename must have a appendix '.asm'";
		abort();
	}
	Parser parser(argv[1]);
	Code code;
	SymbolTable symbols;
	// First scan, record the constants
	if (!allow_collide) {
		while (parser.hasMoreCommands()) {
			if (parser.commandType() == AType) {
				string sym = parser.symbol();
				if (sym.front() >= '0' && sym.front() <= '9') {
					code_16 symCode = atoi(sym.c_str());
					symbols.addAddr(symCode);
				}
			}
			parser.advance();
		}
	}
	// Second scan, record the jump positions
	parser.reset();
	while (parser.hasMoreCommands()) {
		if (parser.commandType() == LType) {
			string sym = parser.symbol();
			if (symbols.containSym(sym)) throw AssembleError("this label is ambiguous or collide with default symbols", parser.lineNum, parser.line.c_str());
			symbols.addEntry(sym, parser.pc);
			if (!allow_collide) symbols.addAddr(parser.pc);
		}
		parser.advance();
	}
	// Third scan, make the binary file
	parser.reset();
	ofstream hackfile(fname.substr(0, fname.size() - 4) + ".hack", ios::out);
	code_16 available_address(16);
	while (parser.hasMoreCommands()) {
		switch (parser.commandType())
		{
		case AType: {
			string sym = parser.symbol();
			code_16 symCode;
			if (sym.front() >= '0' && sym.front() <= '9') {
				symCode = atoi(sym.c_str());
			}
			else {
				if (symbols.containSym(sym)) symCode = symbols.getAddr(sym);
				else {
					// get next available variable address
					while (symbols.containAddr(available_address)) {
						available_address++;
						if (available_address >= symbols.getAddr("SCREEN")) throw AssembleError("RAM overflow", parser.lineNum, parser.line.c_str());
					}
					// symbols.addEntry(sym, available_address);
					symCode = available_address;
				}
			}
			hackfile << code_16Tostring(symCode) << endl;
		}
			break;
		case CType: {
			code_16 cmdCode;
			try
			{
				code_16 dest = code.dest(parser.dest());
				code_16 comp = code.comp(parser.comp());
				code_16 jump = code.jump(parser.jump());
				cmdCode = dest | comp | jump;
			}
			catch (const runtime_error& e)
			{
				throw AssembleError(e.what(), parser.lineNum, parser.line.c_str());
			}
			hackfile << code_16Tostring(cmdCode) << endl;
		}
			break;
		case LType:
			break;
		}
		parser.advance();
	}
	parser.close();
	hackfile.close();
}