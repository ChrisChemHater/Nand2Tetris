#include "head.h"

SymbolTable::SymbolTable() {
	symTable["SP"] = 0;
	symTable["LCL"] = 1;
	symTable["ARG"] = 2;
	symTable["THIS"] = 3;
	symTable["THAT"] = 4;
	symTable["SCREEN"] = 0x4000;
	symTable["KBD"] = 0x6000;

	symTable["R0"] = 0;
	symTable["R1"] = 1;
	symTable["R2"] = 2;
	symTable["R3"] = 3;
	symTable["R4"] = 4;
	symTable["R5"] = 5;
	symTable["R6"] = 6;
	symTable["R7"] = 7;
	symTable["R8"] = 8;
	symTable["R9"] = 9;
	symTable["R10"] = 10;
	symTable["R11"] = 11;
	symTable["R12"] = 12;
	symTable["R13"] = 13;
	symTable["R14"] = 14;
	symTable["R15"] = 15;

	addrSet.insert({ 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,0x4000,0x6000 });
}
void SymbolTable::addEntry(const string& symbol, code_16 address) {
	symTable.insert(pair<string, code_16>(symbol, address));
	//addrSet.insert(address);
}
void SymbolTable::addAddr(code_16 address) {
	addrSet.insert(address);
}
bool SymbolTable::containSym(const string& symbol) {
	return symTable.find(symbol) != symTable.end();
}
bool SymbolTable::containAddr(const code_16 addr) {
	return addrSet.find(addr) != addrSet.end();
}
code_16 SymbolTable::getAddr(const string& symbol) {
	//map<string, code_16>::iterator p = symTable.find(symbol);
	//if (p == symTable.end()) {
	//	string errorMsg("symbol doesn't exist: ");
	//	throw runtime_error(errorMsg + symbol);
	//}
	//return p->second;
	return symTable[symbol];
}