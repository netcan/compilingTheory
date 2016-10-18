/*************************************************************************
	> File Name: LR.cpp
	  > Author: Netcan
	  > Blog: http://www.netcan666.com
	  > Mail: 1469709759@qq.com
	  > Created Time: 2016-10-17 一 21:03:17 CST

                                                    __----~~~~~~~~~~~------___
                                   .  .   ~~//====......          __--~ ~~
                   -.            \_|//     |||\\  ~~~~~~::::... /~
                ___-==_       _-~o~  \/    |||  \\            _/~~-
        __---~~~.==~||\=_    -_--~/_-~|-   |\\   \\        _/~
    _-~~     .=~    |  \\-_    '-~7  /-   /  ||    \      /
  .~       .~       |   \\ -_    /  /-   /   ||      \   /
 /  ____  /         |     \\ ~-_/  /|- _/   .||       \ /
 |~~    ~~|--~~~~--_ \     ~==-/   | \~--===~~        .\
          '         ~-|      /|    |-~\~~       __--~~
                      |-~~-_/ |    |   ~\_   _-~            /\
                           /  \     \__   \/~                \__
                       _--~ _/ | .-~~____--~-/                  ~~==.
                      ((->/~   '.|||' -_|    ~~-/ ,              . _||
                                 -_     ~\      ~~---l__i__i__i--~~_/
                                 _-~-__   ~)  \--______________--~~
                               //.-~~~-~_--~- |-------~~~~~~~~
                                      //.-~~~--\
						神兽保佑，代码无BUG!
 ************************************************************************/

#include <iostream>
#include <ctype.h>
#include <vector>
#include <string>
#include <algorithm>
#include <map>
#include <set>
using namespace std;
#define EXTENSION_NOTERMINAL '^'

class Prod { // 这里是存放形如X->abc的形式，不存在多个候选式
	friend class Proj;
	private:
		string prod; // 产生式
		char noTerminal; // 产生式左部非终结符名字
		string selection; // 产生式右部
		friend bool operator == (const Prod &a, const Prod &b) {
			// return a.noTerminal == b.noTerminal && a.selection == b.selection;
		}
		string cut(int i, int j) {
			return string(prod.begin() + i, prod.begin() + j);
		}

	public:
		Prod(const string &in);
};

Prod::Prod(const string &in) {
	// printf("%s\n", in.c_str());
	prod = in;
	noTerminal = in[0];
	selection = cut(3, in.length());
}


class Proj {
	private:
		vector<Prod> projs; // 项目集
		set<char> Vn; // 非终结符
		set<char> Vt; // 终结符
		string cut(const string &in, int i, int j) {
			return string(in.begin() + i, in.begin() + j);
		}
	public:
		void add(const string &prod);
		void debug();
};


void Proj::add(const string &prod) {
	if(prod.length() < 4) return;
	char noTerminal;
	if(cut(prod, 1, 3) == "->" && (isupper(prod[0]) || prod[0] == EXTENSION_NOTERMINAL)) // A->...则noTerminal = A
		noTerminal = prod[0];
	else return;

	for(unsigned int i=0; i<prod.length(); ++i) { // 提取终结符、非终结符
		char c = prod[i];
		if(isupper(c) || c == EXTENSION_NOTERMINAL) Vn.insert(c);
		else if(c!='|' && !(c=='-' && prod[i+1] == '>' && ++i)) Vt.insert(c);
	}

	for(unsigned int i=3; i<prod.length(); ++i) { // 提取候选式
		unsigned int j;
		for(j=i+1; j<prod.length() && prod[j] != '|'; ++j);
		Prod p = Prod(string(1, noTerminal)+"->"+cut(prod, i, j));
		if(find(projs.begin(), projs.end(), p) == projs.end()) // 去重
			projs.push_back(p);
		i = j;
	}
}

void Proj::debug() {
	puts("=====Projs:======");
	for(auto pro: projs)
		printf("%s\n", pro.prod.c_str());
	puts("=====Vt:======");
	for(auto vt: Vt)
		printf("%c, ", vt);
	puts("\n=====Vn:======");
	for(auto vn: Vn)
		printf("%c, ", vn);
	printf("\n");
}



int main() {
	string in;
	Proj proj;
	while(cin >> in && in != "#")
		proj.add(in);
	proj.debug();

	return 0;
}


