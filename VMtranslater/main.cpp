#include "myutils.h"
#include "head.h"

const char* help =
"filename_or_dirname\n"
"VMtranslator for Hack\n"
"v 1.0\n"
"Author: Chris\n\n";

void processSingleFile(const char* vmFile, CodeWriter& code, bool comment_mode) {
	Parser parser(vmFile);
	code.setClassName(parser.getClassName());
	while (parser.hasMoreCommands()) {
		if (comment_mode) code.writeComment(parser.line);
		try {
			switch (parser.getCommandType())
			{
			case C_ARITH:
				code.writeArithmetic(parser.arg1());
				break;
			case C_PUSH:
			case C_POP:
				code.writePushPop(parser.getCommandType(), parser.arg1(), parser.arg2());
				break;
			case C_LABEL:
				code.writeLabel(parser.getFuncName(), parser.arg1());
				break;
			case C_GOTO:
				code.writeGoto(parser.getFuncName(), parser.arg1());
				break;
			case C_IF:
				code.writeIf(parser.getFuncName(), parser.arg1());
				break;
			case C_CALL:
				code.writeCall(parser.arg1(), parser.arg2());
				break;
			case C_FUNCTION:
				code.writeFunc(parser.arg1(), parser.arg2());
				break;
			case C_RETURN:
				code.writeRet();
				break;
			default:
				break;
			}
		}
		catch (const char* errorMsg) {
			throw VMTranslateError(errorMsg, parser.lineNum, parser.line.c_str());
		}
		parser.advance();
	}
	parser.close();
}

int main(unsigned argn, char* argv[]) {
	cmdline::parser argParser;
	argParser.add("comment-mode", 'c', "In comment mode, asm code will be commented by correspond vm code.");
	argParser.add("no-init", '\0', "translating without setting SP nor calling Sys.init first.");
	argParser.footer(help);
	argParser.parse_check(argn, argv);
	bool comment_mode(argParser.exist("comment-mode"));
	bool no_init(argParser.exist("no-init"));

	// valify the filename
	if (argParser.rest().size() != 1) {
		cout << "Please Enter the .vm file name or the directory name!" << endl;
		cout << argParser.usage();
		abort();
	}
	string fname(argParser.rest().front());
	CodeWriter code;
	if (fname.find(".vm") == fname.size() - 3) {
		code.open((fname.substr(0, fname.size() - 3) + ".asm").c_str());
		if (!no_init) code.writeInit();
		processSingleFile(fname.c_str(), code, comment_mode);
		code.close();
	}
	else {
		if (fname.back() == '/' || fname.back() == '\\') fname.pop_back();
		path vmpath(fname);
		if (!exists(vmpath) || (!is_directory(vmpath))) {
			cout << fname << ": directory doesn't exist" << endl;
			cout << argParser.usage();
			abort();
		}
		if (vmpath.is_absolute()) code.open((fname + '/' + vmpath.filename().string() + ".asm").c_str());
		else code.open((fname + '\\' + vmpath.filename().string() + ".asm").c_str());
		if (!no_init) code.writeInit();
		directory_iterator vmdir(vmpath);
		for (const directory_entry& file : vmdir) {
			string vmfile(file.path().string());
			if (vmfile.find(".vm") != vmfile.size() - 3) continue;
			processSingleFile(vmfile.c_str(), code, comment_mode);
		}
		code.close();
	}
}