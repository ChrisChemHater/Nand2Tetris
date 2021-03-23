#include "myutils.h"
#include "head.h"

map<string, string> lattSegLib({
	pair<string, string>("argument", "ARG"),
	pair<string, string>("local", "LCL"),
	pair<string, string>("this", "THIS"),
	pair<string, string>("that", "THAT")
	});

CodeWriter::CodeWriter() : lineNum(0) {}
void CodeWriter::open(const char* file) {
	_file.open(file);
	if (!_file) {
		cout << "can't open file: " << file << endl;
		abort();
	}
}
CodeWriter::CodeWriter(const char* file) : lineNum(0) {
	open(file);
}
void CodeWriter::setClassName(const string& className) {
	_fname.assign(className);
}
void CodeWriter::writeArithmetic(const string& cmd) {
	if (cmd == "neg" || cmd == "not") {
		_file <<
			"@SP\n"
			"A=M-1\n";
		if (cmd == "neg") _file << "M=-M\n";
		else _file << "M=!M\n"; // not
		lineNum += 3;
	}
	else if (
		cmd == "add" ||
		cmd == "sub" ||
		cmd == "and" ||
		cmd == "or")
	{
		_file <<
			"@SP\n"
			"AM=M-1\n"
			"D=M\n"
			"A=A-1\n";
		if (cmd == "add") _file << "M=D+M\n";
		else if (cmd == "sub")_file << "M=M-D\n";
		else if (cmd == "and")_file << "M=D&M\n";
		else _file << "M=D|M\n";  // or
		lineNum += 5;
	}
	else if (cmd == "eq") {
		boost::format codeFmt(
			"@SP\n"
			"AM=M-1\n"
			"D=M\n"
			"A=A-1\n"
			"D=M-D\n"
			"M=-1\n"
			"@EQ_%d\n"
			"D;JEQ\n"
			"@SP\n"
			"A=M-1\n"
			"M=0\n"
			"(EQ_%d)\n"
		);
		codeFmt% _eqIdx % (++_eqIdx);
		_file << codeFmt.str();
		lineNum += 12;
	}
	else if (cmd == "gt") {
		boost::format codeFmt(
			"@SP\n"
			"AM=M-1\n"
			"D=M\n"
			"A=A-1\n"
			"D=M-D\n"
			"M=-1\n"
			"@GT_%d\n"
			"D;JGT\n"
			"@SP\n"
			"A=M-1\n"
			"M=0\n"
			"(GT_%d)\n"
		);
		codeFmt% _gtIdx % (++_gtIdx);
		_file << codeFmt.str();
		lineNum += 12;
	}
	else if (cmd == "lt") {
		boost::format codeFmt(
			"@SP\n"
			"AM=M-1\n"
			"D=M\n"
			"A=A-1\n"
			"D=M-D\n"
			"M=-1\n"
			"@LT_%d\n"
			"D;JLT\n"
			"@SP\n"
			"A=M-1\n"
			"M=0\n"
			"(LT_%d)\n"
		);
		codeFmt% _ltIdx % (++_ltIdx);
		_file << codeFmt.str();
		lineNum += 12;
	}
	else throw "unresolved command";
}
void CodeWriter::writePushPop(CommandType cmdtype, const string& segment, unsigned index) {
	if (segment == "argument" ||
		segment == "local" ||
		segment == "this" ||
		segment == "that") {
		if (cmdtype == C_PUSH) {
			boost::format codeFmt(
				"@%d\n"
				"D=A\n"
				"@%s\n"
				"A=D+M\n"
				"D=M\n"
				"@SP\n"
				"AM=M+1\n"
				"A=A-1\n"
				"M=D\n"
			);
			codeFmt% index% lattSegLib[segment];
			_file << codeFmt.str();
		}
		else {  // "C_POP"
			boost::format codeFmt(
				"@%d\n"
				"D=A\n"
				"@%s\n"
				"D=D+M\n"
				"@R13\n"
				"M=D\n"
				"@SP\n"
				"AM=M-1\n"
				"D=M\n"
				"@R13\n"
				"A=M\n"
				"M=D\n"
			);
			codeFmt% index% lattSegLib[segment];
			_file << codeFmt.str();
		}
	}
	else if (segment == "static") {
		if (cmdtype == C_PUSH) {
			boost::format codeFmt(
				"@%s.%d\n"
				"D=M\n"
				"@SP\n"
				"AM=M+1\n"
				"A=A-1\n"
				"M=D\n"
			);
			codeFmt% _fname% index;
			_file << codeFmt.str();
		}
		else {
			boost::format codeFmt(
				"@SP\n"
				"AM=M-1\n"
				"D=M\n"
				"@%s.%d\n"
				"M=D\n"
			);
			codeFmt% _fname% index;
			_file << codeFmt.str();
		}
	}
	else if (segment == "constant") {
		if (cmdtype == C_POP) throw "const segment can't be changed";
		boost::format codeFmt(
			"@%d\n"
			"D=A\n"
			"@SP\n"
			"AM=M+1\n"
			"A=A-1\n"
			"M=D\n"
		);
		codeFmt% index;
		_file << codeFmt.str();
	}
	else if (segment == "pointer") {
		if (cmdtype == C_PUSH) {
			boost::format codeFmt(
				"@%s\n"
				"D=M\n"
				"@SP\n"
				"AM=M+1\n"
				"A=A-1\n"
				"M=D\n"
			);
			switch (index)
			{
			case 0:
				codeFmt % "THIS";
				break;
			case 1:
				codeFmt % "THAT";
				break;
			default:
				throw "index error";
				break;
			}
			_file << codeFmt.str();
		}
		else {
			boost::format codeFmt(
				"@SP\n"
				"AM=M-1\n"
				"D=M\n"
				"@%s\n"
				"M=D\n"
			);
			switch (index)
			{
			case 0:
				codeFmt % "THIS";
				break;
			case 1:
				codeFmt % "THAT";
				break;
			default:
				throw "index error";
				break;
			}
			_file << codeFmt.str();
		}
	}
	else if (segment == "temp") {
		if (index + 5 > 12) throw "temp overflow";
		if (cmdtype == C_PUSH) {
			boost::format codeFmt(
				"@%d\n"
				"D=M\n"
				"@SP\n"
				"AM=M+1\n"
				"A=A-1\n"
				"M=D\n"
			);
			codeFmt % (index + 5);
			_file << codeFmt.str();
		}
		else {
			boost::format codeFmt(
				"@SP\n"
				"AM=M-1\n"
				"D=M\n"
				"@%d\n"
				"M=D\n"
			);
			codeFmt % (index + 5);
			_file << codeFmt.str();
		}
	}
	else throw "unsolved segment";
}
void CodeWriter::writeComment(const string& comment) {
	_file << "//" << comment << endl;
}
void CodeWriter::writeInit() {
	_file <<
		"@256\n"
		"D=A\n"
		"@SP\n"
		"M=D\n";
	writeCall("Sys.init", 0);
}
void CodeWriter::writeLabel(const string& funcName, const string& label) {
	boost::format codeFmt("(%s$%s)\n");
	codeFmt% funcName% label;
	_file << codeFmt.str();
}
void CodeWriter::writeGoto(const string& funcName, const string& label) {
	boost::format codeFmt(
		"@%s$%s\n"
		"0;JMP\n"
	);
	codeFmt% funcName% label;
	_file << codeFmt.str();
}
void CodeWriter::writeIf(const string& funcName, const string& label) {
	boost::format codeFmt(
		"@SP\n"
		"AM=M-1\n"
		"D=M\n"
		"@%s$%s\n"
		"D;JNE\n"
	);
	codeFmt% funcName% label;
	_file << codeFmt.str();
}
void CodeWriter::writeCall(const string& funcName, unsigned numArgs) {
	boost::format codeFmt(
		"@SP\n"  // newARG = SP - numArgs  --> R13
		"D=M\n"
		"@R13\n"
		"M=D\n"
		"@%d\n"  // %d = numArgs
		"D=A\n"
		"@R13\n"
		"M=M-D\n"
		"@RETURN_LABEL$%d\n"  // push return-address, %d = callIdx
		"D=A\n"
		"@SP\n"
		"M=M-1\n"  // ATTENTION! SP- = real SP - 1, to accelarate call process
		"AM=M+1\n"
		"M=D\n"
		"@LCL\n"  // push LCL
		"D=M\n"
		"@SP\n"
		"AM=M+1\n"
		"M=D\n"
		"@ARG\n"  // push ARG
		"D=M\n"
		"@SP\n"
		"AM=M+1\n"
		"M=D\n"
		"@THIS\n"  // push THIS
		"D=M\n"
		"@SP\n"
		"AM=M+1\n"
		"M=D\n"
		"@THAT\n"  // push THAT
		"D=M\n"
		"@SP\n"
		"AM=M+1\n"
		"M=D\n"
		"@R13\n"  // R13 = newARG --> ARG
		"D=M\n"
		"@ARG\n"
		"M=D\n"
		"@SP\n"  // LCL, SP = SP- + 1
		"MD=M+1\n"
		"@LCL\n"
		"M=D\n"
		"@%s\n"  // goto f, %s = funcName
		"0;JMP\n"
		"(RETURN_LABEL$%d)\n"  // %d = callIdx
	);
	codeFmt% numArgs% _returnIdx% funcName% (_returnIdx++);
	_file << codeFmt.str();
}
void CodeWriter::writeRet() {
	_file <<
		"@LCL\n"  // FRAME = LCL
		"D=M\n"
		//"@R13\n"
		//"M=D\n"
		"@5\n"  // RET = *(FRAME - 5)
		"A=D-A\n"
		"D=M\n"
		"@R14\n"
		"M=D\n"
		"@SP\n"  // *ARG = pop()
		"AM=M-1\n"
		"D=M\n"
		"@ARG\n"
		"A=M\n"
		"M=D\n"
		"@ARG\n"  // SP = ARG + 1
		"D=M+1\n"
		"@SP\n"
		"M=D\n"
		"@LCL\n"  // THAT = *(FRAME - 1)
		"AM=M-1\n"
		"D=M\n"
		"@THAT\n"
		"M=D\n"
		"@LCL\n"  // THIS = *(FRAME - 2)
		"AM=M-1\n"
		"D=M\n"
		"@THIS\n"
		"M=D\n"
		"@LCL\n"  // ARG = *(FRAME - 3)
		"AM=M-1\n"
		"D=M\n"
		"@ARG\n"
		"M=D\n"
		"@LCL\n"  // LCL = *(FRAME - 4)
		"AM=M-1\n"
		"D=M\n"
		"@LCL\n"
		"M=D\n"
		"@R14\n"  // goto RET
		"A=M\n"
		"0;JMP\n";
}
void CodeWriter::writeFunc(const string& funcName, unsigned numLocals) {
	_file << '(' << funcName << ')' << endl;
	_file <<
		"@SP\n"
		"A=M\n";
	for (unsigned i = 0; i < numLocals; i++)
		_file <<
			"M=0\n"
			"A=A+1\n";
	_file << '@' << numLocals << endl;
	_file <<
		"D=A\n"
		"@SP\n"
		"M=D+M\n";
}
void CodeWriter::close() {
	//_file <<
	//	"(END$)\n"
	//	"@END$\n"
	//	"0;JMP\n";
	_file.close();
}