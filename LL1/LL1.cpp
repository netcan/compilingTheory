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
#include <algorithm>
#include <map>
#include <set>
using namespace std;

class Prod { // 产生式
	friend class LL1;
	private:
		string prod; // 产生式
		char noTerminal; // 产生式左部非终结符名字
		set<string> selection; // 候选式集合
		set<char> Vn; // 非终结符
		set<char> Vt; // 终结符
		string cut(int i, int j) {
			return string(prod.begin() + i, prod.begin() + j);
		}
		friend bool operator == (const Prod &a, const char &c) {
			return a.noTerminal == c;
		}

		bool isValid; // 产生式是否合法

	public:
		Prod(const string &in);
		bool split(); // 分割终结符、非终结符、产生式集合、左部非终结符名字，返回分割是否成功
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
		else if(c!='|' && !(c=='-' && prod[i+1] == '>' && ++i)) Vt.insert(c);
	}

	for(unsigned int i=3; i<prod.length(); ++i) { // 提取候选式
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
	private: vector<Prod> G; // 文法G
		set<char> VN; // 非终结符
		set<char> VT; // 终结符
		map<char, set<char> > FIRST;
		set<char> first(const string &s);
	public:
		bool addProd(const Prod & prod);
		void show();
		void buildFirst();
};

bool LL1::addProd(const Prod &prod) {
	if(prod.isValid) {
		vector<Prod>::iterator it = find(G.begin(), G.end(), prod.noTerminal);
		if(it != G.end()) {   // 找到产生式
			it->selection.insert(prod.selection.begin(), prod.selection.end()); // 追加候选式
			for(auto s: prod.selection)
				it->prod += ("|" + s); // 追加候选式
		}
		else  // 没找到
			G.push_back(prod);
		VN.insert(prod.Vn.begin(), prod.Vn.end());
		VT.insert(prod.Vt.begin(), prod.Vt.end());
		return true;
	}
	else return false;
}

set<char> LL1::first(const string &s) {
	Prod prod = Prod(s);
	if(prod.isValid) { // 产生式
		for(auto sel:prod.selection) {
			set<char> f = first(sel);
			FIRST[prod.noTerminal].insert(f.begin(), f.end());
		}
		return FIRST[prod.noTerminal];
	}
	else { // 非产生式
		if(VT.find(s[0]) != VT.end()) { // 终结符
			set<char> res;
			res.insert(s[0]);
			return res;
		}
		else { // 非终结符、候选式
			if(FIRST[s[0]].size() != 0) return FIRST[s[0]];
			else {
				vector<Prod>::iterator it = find(G.begin(), G.end(), s[0]);
				if(it != G.end()) {
					set<char> f = first(it->prod);
					FIRST[s[0]].insert(f.begin(), f.end());
				}
				return FIRST[s[0]];
			}
		}
	}
}

void LL1::buildFirst() {
	for(auto prod: G) first(prod.prod);
	return;
}

void LL1::show() {
	printf("VT: \n");
	for(auto c: VT) {
		printf("%c, ", c);
	}
	printf("\nVN: \n");
	for(auto c: VN)
		printf("%c, ", c);
	puts("");
	for(auto p: G) {
		printf("noTerminal: %c\n", p.noTerminal);
		printf("selections: \n");
		for(auto s: p.selection)
			cout << s << endl;
	}
	for(auto prod:G) {
		printf("FIRST(%c)= {", prod.noTerminal);
		for(auto c:FIRST[prod.noTerminal]) {
			printf("%c,", c);
		}
		printf("}\n");
	}
}

int main() {
	LL1 ll;
	string in;
	while(cin >> in)
		ll.addProd(Prod(in));
	ll.buildFirst();
	ll.show();

	return 0;
}
