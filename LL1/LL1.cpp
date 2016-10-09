/*************************************************************************
	> File Name: LL1.cpp
	  > Author: Netcan
	  > Blog: http://www.netcan666.com
	  > Mail: 1469709759@qq.com
	  > Created Time: 2016-10-09 日 19:23:18 CST
 ************************************************************************/

#include <iostream>
#include <ctype.h>
#include <vector>
#include <string>
#include <set>
using namespace std;

class Prod { // 产生式
	friend class LL1;
	private:
		string prod; // 产生式
		char noTerminal; // 产生式左部非终结符名字
		set<string> selection; // 产生式集合
		set<char> Vn; // 非终结符
		set<char> Vt; // 终结符
		string cut(int i, int j) {
			return string(prod.begin() + i, prod.begin() + j);
		}
		bool isValid; // 产生式是否合法

	public:
		Prod(const string &in);
		bool split(); // 分割终结符、非终结符、产生式集合、左部非终结符名字，返回分割是否成功
		set<char> & getVn();
		set<char> & getVt();
};

Prod::Prod(const string &in) {
	prod = in;
	isValid = false;
	split();
	// printf("split(%d)\n", split());
}

bool Prod::split() {
	if(prod.length() < 4) return false;
	if(cut(1, 3) == "->" && isupper(prod[0])) // A->...则noTerminal = A
		noTerminal = prod[0];
	else return false;
	for(unsigned int i=0; i<prod.length(); ++i) { // 提取终结符、非终结符
		char c = prod[i];
		if(isupper(c)) Vn.insert(c);
		else if(c!='|' && c!='@' && !(c=='-' && prod[i+1] == '>' && ++i)) Vt.insert(c);
	}

	for(unsigned int i=3; i<prod.length(); ++i) { // 提取选择式
		unsigned int j;
		for(j=i+1; j<prod.length() && prod[j] != '|'; ++j);
		selection.insert(cut(i, j));
		i = j;
	}

	// printf("noTerminal:%c \n", noTerminal);

	// printf("selections: \n");
	// for(auto s: selection)
		// cout << s << endl;
	// printf("Vt: \n");
	// for(auto c: Vt)
		// cout << c << endl;
	// printf("Vn: \n");
	// for(auto c: Vn)
		// cout << c << endl;

	return isValid = true;
}


class LL1 {
	private:
		vector<Prod> G; // 文法G
		set<char> VN; // 非终结符
		set<char> VT; // 终结符
	public:
		bool addProd(const Prod & prod);
		void show();
};

bool LL1::addProd(const Prod &prod) {
	if(prod.isValid) {
		G.push_back(prod);
		VN.insert(prod.Vn.begin(), prod.Vn.end());
		VT.insert(prod.Vt.begin(), prod.Vt.end());
		return true;
	}
	else return false;
}

void LL1::show() {
	printf("VT: \n");
	for(auto c: VT)
		cout << c << endl;
	printf("VN: \n");
	for(auto c: VN)
		cout << c << endl;
	for(auto p: G) {
		printf("noTerminal:%c \n", p.noTerminal);
		printf("selections: \n");
		for(auto s: p.selection)
			cout << s << endl;
	}

}

int main() {
	LL1 ll;
	ll.addProd(Prod("E->TG"));
	ll.addProd(Prod("G->+TG|-TG"));
	ll.addProd(Prod("G->@"));
	ll.addProd(Prod("T->FS"));
	ll.addProd(Prod("S->*FS|/FS"));
	ll.addProd(Prod("S->@"));
	ll.addProd(Prod("F->(E)"));
	ll.addProd(Prod("F->i"));
	ll.show();

	return 0;
}
