#include "part1.h"
#include "myutils.h"

const char* help =
"Jack compiler: Part1\n"
"v 0.5\n"
"Author: Chris\n\n"
"convert Jack codes to parse tree\n\n";

void processSingleFile(const char* file) {
	string strFile(file);
	string ofile = strFile.substr(0, strFile.size() - 5) + ".xml";
	CompilationEngine engine(file, ofile.c_str());
	if (!engine._tkparser.hasMoreCommand()) {
		cout << "empty file: " << file << endl;
	}
	else if (engine._tkparser.currentToken != "class") {
		cout << "In file " << file << endl;
		throw SyntaxError("file must begin with keyword: 'class'", engine._tkparser.lineNum);
	}
	else {
		engine.compileClass();
		if (engine._tkparser.hasMoreCommand())
			throw SyntaxError("Extra unresolved codes", engine._tkparser.lineNum);
	}
	engine.close();
}

int main(unsigned argc, const char* argv[]) {
	//CompilationEngine engine("test.jack", "test.xml");
	//engine.compileClass();

	if (argc != 2) {
		cout << "Unvalid param number" << endl;
		abort();
	}
	string fname(argv[1]);
	if (fname.size() >= 5 && fname.substr(fname.size()-5, fname.size()) == ".jack") {
		processSingleFile(fname.c_str());
	}
	else {
		
		path vmpath(fname);
		if (!exists(vmpath) || (!is_directory(vmpath))) {
			cout << fname << ": directory doesn't exist" << endl;
			abort();
		}
		if (vmpath.is_absolute()) {
			if (fname.back() != '\\') fname.push_back('\\');
		}
		else {
			if (fname.back() != '/') fname.push_back('/');
		}
		directory_iterator vmdir(vmpath);
		for (const directory_entry& file : vmdir) {
			if (file.path().extension().string() != ".jack") continue;
			processSingleFile((file.path().string()).c_str());
		}
	}
}