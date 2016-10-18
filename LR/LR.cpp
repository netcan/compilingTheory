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
	friend class Item;
	friend class LR;
	private:
		char noTerminal; // 产生式左部非终结符名字
		string selection; // 产生式右部
		set<char> additionalVt; // 附加终结符
		friend bool operator == (const Prod &a, const Prod &b) {
			return a.noTerminal == b.noTerminal && a.selection == b.selection;
		}
		friend bool operator == (const Prod &a, char c) {
			return a.noTerminal == c;
		}

	public:
		static string cut(const string &in, int i, int j) {
			return string(in.begin() + i, in.begin() + j);
		}
		void display();
		Prod(const string &in);
		Prod(char noTerminal, string selection, set<char> additionalVt):
			noTerminal(noTerminal), selection(selection), additionalVt(additionalVt) {}
};

void Prod::display() {
	printf("%c->%s", noTerminal, selection.c_str());
	int i = 0;
	for(auto c:additionalVt)
		printf("%c%c", i++==0?',':'/', c);
	puts("");
}

Prod::Prod(const string &in) {
	// printf("%s\n", in.c_str());
	noTerminal = in[0];
	selection = cut(in, 3, in.length());
}


class Item { // 项目集
	friend class LR;
	private:
		vector<Prod> prods; // 项目集
		static set<char> Vn; // 非终结符
		static set<char> Vt; // 终结符
	public:
		void add(const string &prod);
};

set<char>Item::Vn; // 全局静态变量
set<char>Item::Vt;


void Item::add(const string &prod) {
	if(prod.length() < 4) return;
	char noTerminal;
	if(Prod::cut(prod, 1, 3) == "->" && (isupper(prod[0]) || prod[0] == EXTENSION_NOTERMINAL)) // A->...则noTerminal = A
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
		Prod p = Prod(string(1, noTerminal)+"->"+Prod::cut(prod, i, j));
		if(find(prods.begin(), prods.end(), p) == prods.end()) // 去重
			prods.push_back(p);
		i = j;
	}
}


class LR {
	private:
		Item G; // 文法G
		vector<Item> C; // 项目集规范族
		map<pair<int, char>, int> GOTO; // goto数组，项目集<int, int>=char
		map<char, set<char> > FIRST; // first集
		set<char> first(const string &s); // 求first集
		Item closure(Item I); // 求该项目的闭包
		void items(); // 求项目集状态机DFA！!
	public:
		void add(const string &s); // 添加产生式
		void debug();
};

void LR::add(const string &s) {
	G.add(s);
}

Item LR::closure(Item I) {
	unsigned int size = 0;
	while(size != I.prods.size()) { // 当没有项目加入的时候
		size = I.prods.size();
		for(auto prod: I.prods) { // 枚举I的产生式
			unsigned long pointLoc = 0;
			if((pointLoc = prod.selection.find('.')) != string::npos && pointLoc != prod.selection.length() - 1) { // 找到.，A->a.Bc,d
				string f = Prod::cut(prod.selection, pointLoc+2, prod.selection.length());
				// prod.display();
				// printf("f: %s\n", f.c_str());
				set<char> fst;
				for(auto c: prod.additionalVt) {
					set<char> fs = first(f + c);
					fst.insert(fs.begin(), fs.end());
				}


				for(vector<Prod>::iterator it = G.prods.begin(); it != G.prods.end(); ++it) {
					if(*it == prod.selection[pointLoc + 1]) { // 找到产生式
						Prod p = *it;
						p.selection = '.' + p.selection;
						vector<Prod>::iterator Iit = find(I.prods.begin(), I.prods.end(), p); // 找I中是否存在产生式
						if(Iit != I.prods.end())  // 找到
							Iit->additionalVt.insert(fst.begin(), fst.end());
						else {
							p.additionalVt.insert(fst.begin(), fst.end());
							I.prods.push_back(p);
						}
					}
				}
			}
		}
	}
	return I;
}

void LR::items() {// 求项目集状态机DFA！!
	Item initial;
	initial.prods.push_back(Prod('^', '.' + string(1, G.prods[0].noTerminal), {'#'})); // 初值，^->.S,#
	C.push_back(closure(initial)); // 置C初值

}

set<char> LR::first(const string &s) { // s不为产生式！
	if(s.length() == 0)
		return set<char>({'@'});
	else if(s.length() == 1) {
		if(G.Vt.find(s[0]) != G.Vt.end() || s[0] == '#') // 终结符
			return set<char>({s[0]});
		else
			if(FIRST[s[0]].size() != 0) return FIRST[s[0]];
			else {
				for(vector<Prod>::iterator it = G.prods.begin(); it != G.prods.end(); ++it)
					if(it->noTerminal == s[0]) {
						set<char> f = first(it->selection);
						FIRST[s[0]].insert(f.begin(), f.end());
					}
				return FIRST[s[0]];
			}
	} else { // first(X1X2X3X4)...
		set<char> ret;
		for(unsigned int i=0; i<s.length(); ++i) {
			set<char> f = first(string(1, s[i])); // 逐个符号求first(Xi)集
			if(f.find('@') != f.end() && s.length() - 1 != i) { // 发现@
				f.erase(f.find('@')); // 减去@
				ret.insert(f.begin(), f.end()); // 放入first集合
			} else { // 无@或者最后一个Xi，则不需要求下去了
				ret.insert(f.begin(), f.end());
				break;
			}
		}
		return ret;
	}
}


void LR::debug() {
	puts("=====Proj:======");
	for(auto pro: G.prods)
		printf("%s\n", (string(1, pro.noTerminal) + "->" + pro.selection).c_str());
	puts("=====Vt:======");
	for(auto vt: G.Vt)
		printf("%c, ", vt);
	puts("\n=====Vn:======");
	for(auto vn: G.Vn)
		printf("%c, ", vn);
	// puts("\n=====FIRST:======");
	// set<char> f = first("S");
	// for(auto c: f)
		// printf("%c, ", c);
	puts("");
	// puts("\n=====Item:======");
	// Item I;
	// I.prods.push_back(Prod('B', "b.B", {'b', 'a'}));
	// I = closure(I);
	// for(auto prod: I.prods)
		// prod.display();
}



int main() {
	string in;
	LR lr;
	while(cin >> in && in != "#")
		lr.add(in);
	lr.debug();

	return 0;
}


