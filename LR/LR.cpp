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
		string right; // 产生式右部
		set<char> additionalVt; // 附加终结符
		friend bool operator == (const Prod &a, const Prod &b) {
			return a.noTerminal == b.noTerminal && a.right == b.right;
		}
		friend bool operator == (const Prod &a, char c) {
			return a.noTerminal == c;
		}

	public:
		static string cut(const string &in, int i, int j) {
			return string(in.begin() + i, in.begin() + j);
		}
		void display() const;
		Prod(const string &in);
		Prod(const char &noTerminal, const string& right, const set<char>& additionalVt):
			noTerminal(noTerminal), right(right), additionalVt(additionalVt) {}
};

void Prod::display() const{
	printf("%c->%s", noTerminal, right.c_str());
	int i = 0;
	for(const auto& c:additionalVt)
		printf("%c%c", i++==0?',':'/', c);
	puts("");
}

Prod::Prod(const string &in) {
	// printf("%s\n", in.c_str());
	noTerminal = in[0];
	right = cut(in, 3, in.length()); // A->X1X2X3X4
}


class Item { // 项目集
	friend class LR;
	private:
		vector<Prod> prods; // 项目集
		static set<char> Vn; // 非终结符
		static set<char> Vt; // 终结符
		static set<char> Symbol; // 所有符号
		friend bool operator == (const Item &a, const Item &b) {
			if(a.prods.size() != b.prods.size()) return false;
			else {
				for(const auto& p: a.prods) {// 选择a的每个产生式
					auto it = find(b.prods.begin(), b.prods.end(), p);
					if(it == b.prods.end())  // 找不到
						return false;
					else  {// 找到的话判断附加终结符是否都相等
						if(p.additionalVt != it->additionalVt)
							return false;
					}
				}
				return true;
			}
		}
	public:
		void add(const string &prod);
		void display() const;
};

set<char>Item::Vn; // 全局静态变量
set<char>Item::Vt;
set<char>Item::Symbol;


void Item::add(const string &prod) {
	if(prod.length() < 4) return;
	char noTerminal;
	if(Prod::cut(prod, 1, 3) == "->" && (isupper(prod[0]))) // A->...则noTerminal = A
		noTerminal = prod[0];
	else return;

	for(unsigned int i=0; i<prod.length(); ++i) { // 提取终结符、非终结符
		char c = prod[i];
		if(isupper(c)) {
			Vn.insert(c);
			Symbol.insert(c);
		}
		else if(c!='|' && !(c=='-' && prod[i+1] == '>' && ++i)) {
			Vt.insert(c);
			Symbol.insert(c);
		}
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

void Item::display() const {
	for(const auto& prod: prods)
		prod.display();
}


class LR {
	private:
		Item G; // 文法G
		enum actionStat{
			ACCEPT=0,
			SHIFTIN,
			RECURSIVE,
		};
		static const char *actionStatStr[];

		vector<Item> C; // 项目集规范族
		map<pair<int, char>, int> GOTO; // goto数组，项目集<int, int>=char
		map<pair<int, char>, pair<actionStat, int> > ACTION; // Action数组，Action[(i, a)]=(s|r)j
		map<char, set<char> > FIRST; // first集
		set<char> first(const string &s); // 求first集
		vector<char> inStr; // 输入串/栈
		vector<int> status; // 状态栈
		vector<char> parse; // 分析栈
		Item closure(Item I); // 求该项目的闭包
		Item Goto(const Item& I, char X); // 求I经过X到达的项目集
		void items(); // 求项目集状态机DFA！!
		void showStrStack(); // 显示输入栈
		void showStatusStack();
		void showParseStack();
	public:
		void add(const string &s); // 添加产生式
		void build(); // 构造Action、GOTO表
		void showTable(); // 打印LR分析表！
		void debug();
		void loadStr(const string &in); // 读取输入串
		void parser(); // LR(1)分析
};

const char*LR::actionStatStr[] = {
	"acc",
	"s",
	"r"
};

void LR::add(const string &s) {
	G.add(s);
}

void LR::loadStr(const string &in) {
	inStr.push_back('#');
	status.push_back(0);
	for(int i = in.length() - 1; i>=0; --i)
		inStr.push_back(in[i]);
}

void LR::showStrStack() {
	for(vector<char>::reverse_iterator it = inStr.rbegin(); it != inStr.rend(); ++it)
		printf("%c", *it);
}


void LR::showStatusStack() {
	for(vector<int>::iterator it = status.begin(); it != status.end(); ++it)
		printf("%d", *it);
}

void LR::showParseStack() {
	for(vector<char>::iterator it = parse.begin(); it != parse.end(); ++it)
		printf("%c", *it);
}

void LR::parser() {
	puts("Step\tstatusStack\tparseStack\tinStrStack\taction");
	bool success = false;
	int step = 0;
	while(! success) {
		printf("%d\t", step);

		int sTop = status.size() - 1; // 栈顶
		int iTop = inStr.size() - 1;
		pair<int, char> p = make_pair(status[sTop], inStr[iTop]);
		if(ACTION.find(p) == ACTION.end())  // 出错！
			break;
	 	pair<actionStat, int> act = ACTION[p];
		if(act.first == SHIFTIN) { // 移进
			showStatusStack();
			printf("\t\t");
			showParseStack();
			printf("\t\t");
			showStrStack();
			printf("\t\t");
			printf("SHIFTIN");

			status.push_back(act.second);
			parse.push_back(inStr[iTop]);
			inStr.pop_back();
		} else if(act.first == RECURSIVE){
			Prod p = G.prods[act.second];

			showStatusStack();
			printf("\t\t");
			showParseStack();
			printf("\t\t");
			showStrStack();
			printf("\t\t");
			printf("Recursive %c->%s", p.noTerminal, p.right.c_str());

			for(unsigned i=0; i<p.right.size(); ++i) {
				status.pop_back();
				parse.pop_back();
			}
			parse.push_back(p.noTerminal);
			status.push_back(GOTO[make_pair(status[status.size() - 1], p.noTerminal)]);
		} else if(act.first == ACCEPT) {
			success = true;

			showStatusStack();
			printf("\t\t");
			showParseStack();
			printf("\t\t");
			showStrStack();
			printf("\t\t");
			printf("ACCEPT");
		}
		puts("");
		++step;
	}
	if(! success) {
		showStatusStack();
		printf("\t\t");
		showParseStack();
		printf("\t\t");
		showStrStack();
		printf("\t\t");
		printf("ERROR");
	}
}

Item LR::closure(Item I) {
	if(I.prods.size() == 0) return I;
	unsigned int size = 0;
	while(size != I.prods.size()) { // 当没有项目加入的时候
		size = I.prods.size();
		for(const auto &prod: I.prods) { // 枚举I的产生式
			unsigned long pointLoc = 0;
			if((pointLoc = prod.right.find('.')) != string::npos && pointLoc != prod.right.length() - 1) { // 找到.，A->a.Bc,d
				if(G.Vt.find(prod.right[pointLoc + 1]) != G.Vt.end()) continue;

				string f = Prod::cut(prod.right, pointLoc+2, prod.right.length());
				// prod.display();
				// printf("f: %s\n", f.c_str());
				set<char> fst;
				for(const auto& c: prod.additionalVt) {
					set<char> fs = first(f + c);
					fst.insert(fs.begin(), fs.end());
				}


				for(vector<Prod>::iterator it = G.prods.begin(); it != G.prods.end(); ++it) {
					if(*it == prod.right[pointLoc + 1]) { // 找到产生式
						Prod p = *it;
						p.right = '.' + p.right;
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

Item LR::Goto(const Item& I, char X) {
	Item J;
	if(I.prods.size() == 0) return J;

	for(const auto& p: I.prods)  {// I中的每个项目
		string right = p.right;
		unsigned long pointLoc = right.find('.');
		if(right[pointLoc + 1] == X) {
			swap(right[pointLoc], right[pointLoc + 1]);
			J.prods.push_back(Prod(p.noTerminal, right, p.additionalVt));
		}
	}
	return closure(J);
}

void LR::items() {// 求项目集状态机DFA！!
	Item initial;
	initial.prods.push_back(Prod(EXTENSION_NOTERMINAL, '.' + string(1, G.prods[0].noTerminal), {'#'})); // 初值，^->.S,#
	C.push_back(closure(initial)); // 置C初值
	size_t size = 0;
	while(size != C.size()) { // 当没有项目集加入C中
		size = C.size();
		// for(auto &I: C) { // C的每个项目集，这样写有坑！！
		for(unsigned int i=0; i<C.size(); ++i) { // C的每个项目集
			Item I = C[i];
			for(const auto &X: G.Symbol) { // 每个文法Vt符号X
				Item next = Goto(I, X);
				if(next.prods.size() != 0) { // 不为空
					auto it = find(C.begin(), C.end(), next);
					if(it != C.end()) { // 找到
						GOTO[make_pair(i, X)] = it - C.begin();
					} else {
						C.push_back(next);
						GOTO[make_pair(i, X)] = C.size()-1;
					}
				}
			}
		}
	}
	// printf("size: %ld\n", C.size());
	// for(auto Item: C) {
		// puts("=====");
		// Item.display();
	// }
}

void LR::build() { // 构造Action、GOTO表
	items();
	for(unsigned int i=0; i<C.size(); ++i) { // 逐个项目集
		const Item & item = C[i];
		for(const auto& prod: item.prods) { // 逐个项目
			unsigned long pointLoc = prod.right.find('.');
			if(pointLoc < prod.right.length() - 1) { // 不是最后一个
				char X = prod.right[pointLoc + 1];
				if(G.Vt.find(X) != G.Vt.end() && GOTO.find(make_pair(i, X)) != GOTO.end()) { // 终结符
					int j = GOTO[make_pair(i, X)];
					ACTION[make_pair(i, X)] = make_pair(SHIFTIN, j);
				}
			} else {
				if(prod == Prod(EXTENSION_NOTERMINAL, string(1, G.prods[0].noTerminal)+'.', {}) && prod.additionalVt == set<char>({'#'}))  // S'->S.,# acction[i, #] = acc
					ACTION[make_pair(i, '#')] = make_pair(ACCEPT, 0);
				else if(prod.noTerminal != EXTENSION_NOTERMINAL){
					string right = prod.right;
					right.erase(pointLoc, 1); // 删除.
					for(const auto& X: prod.additionalVt) { // A->a.,b，枚举b
						vector<Prod>::iterator it = find(G.prods.begin(), G.prods.end(), Prod(prod.noTerminal, right, set<char>{}));
						if(it != G.prods.end())  // 找到了
							ACTION[make_pair(i, X)] = make_pair(RECURSIVE, it - G.prods.begin());
					}
				}
			}
		}
	}
	if(G.Vt.find('@') != G.Vt.end()) { // 删除@，移进#
		G.Vt.erase(G.Vt.find('@'));
		G.Symbol.erase(G.Symbol.find('@'));
	}
	G.Vt.insert('#');
	G.Symbol.insert('#');
}

void LR::showTable() {
	printf("\t");
	for(const auto & X: G.Vt) {
		if(X != '#')
			printf("%c\t", X);
	}
	printf("%c\t", '#'); // #放到最后一列显示，美观

	for(const auto & X: G.Vn)
		printf("%c\t", X);
	puts("");
	for(unsigned int i=0; i<C.size(); ++i) {
		printf("%d\t", i);
		for(const auto & X: G.Vt) {
			if(X != '#') {
				pair<int, char> p = make_pair(i, X);
				if(ACTION.find(p) != ACTION.end()) {
					pair<actionStat, int> res = ACTION[p];
					printf("%s", actionStatStr[res.first]);
					if(res.first != ACCEPT)
						printf("%d", res.second);
				}
				printf("\t");
			}
		}

		pair<int, char> p = make_pair(i, '#');
		if(ACTION.find(p) != ACTION.end()) {
			pair<actionStat, int> res = ACTION[p];
			printf("%s", actionStatStr[res.first]);
			if(res.first != ACCEPT)
				printf("%d", res.second);
		}
		printf("\t");

		for(const auto & X: G.Vn) {
			pair<int, char> p = make_pair(i, X);
			if(GOTO.find(p) != GOTO.end())
				printf("%d", GOTO[make_pair(i, X)]);
			printf("\t");
		}
		puts("");
	}
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
						set<char> f = first(it->right);
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
		printf("%s\n", (string(1, pro.noTerminal) + "->" + pro.right).c_str());
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
	build();
	showTable();
	parser();
	puts("");
}



int main() {
	string in;
	LR lr;
	while(cin >> in && in != "#")
		lr.add(in);
	in = "";
	cin >> in;
	lr.loadStr(in);
	lr.debug();

	return 0;
}


