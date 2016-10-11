/*************************************************************************
	> File Name: LL1.cpp
	  > Author: Netcan
	  > Blog: http://www.netcan666.com
	  > Mail: 1469709759@qq.com
	  > Created Time: 2016-10-09 日 19:23:18 CST

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
*************************************************************************/

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
		map<char, set<char> > FIRST; // first集
		map<char, set<char> > FOLLOW; // follow集
		map<pair<char, char>, string> M; // 分析表
		set<char> first(const string &s);
		set<char> follow(const Prod &prod);
		vector<char> parse; // 分析栈
		vector<char> indata; // 输入表达式栈
		void parseTable();
	public:
		bool addProd(const Prod & prod); // 添加产生式
		void debug(); // 输出相关结果
		void build(); // 建立first、follow集、分析表
		void showIndataStack(); // 输出输入串内容
		void showParseStack(); // 输出分析栈内容
		void loadIndata(const string &s); // 输入串入栈
		void parser(); // LL1预测分析
		void error(); // 错误处理
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
		if(FIRST[prod.noTerminal].size() != 0) return FIRST[prod.noTerminal];
		for(auto sel:prod.selection) { // 候选式
			for(unsigned int i=0; i<sel.length(); ++i) {
				char c = sel[i];
				set<char> f = first(string(1, c));
				if(f.find('@') != f.end() && sel.length() - 1 != i) { // 发现@
					f.erase(f.find('@')); // 减去@
					FIRST[prod.noTerminal].insert(f.begin(), f.end()); // 放入first集合
				} else {
					FIRST[prod.noTerminal].insert(f.begin(), f.end());
					break;
				}
			}
		}
		return FIRST[prod.noTerminal];
	}
	else if(s.length() == 1){ // 非产生式
		if(VT.find(s[0]) != VT.end()) { // 终结符
			set<char> res;
			res.insert(s[0]);
			return res;
		}
		else { // 非终结符、候选式
			if(FIRST[s[0]].size() != 0) return FIRST[s[0]];
			else {
				vector<Prod>::iterator it = find(G.begin(), G.end(), s[0]);
				if(*it == s[0]) {
					set<char> f = first(it->prod);
					FIRST[s[0]].insert(f.begin(), f.end());
				}
				return FIRST[s[0]];
			}
		}
	} else {
		set<char> ret;
		for(unsigned int i=0; i<s.length(); ++i) {
			char c = s[i];
			set<char> f = first(string(1, c));
			if(f.find('@') != f.end() && s.length() - 1 != i) { // 发现@
				f.erase(f.find('@')); // 减去@
				ret.insert(f.begin(), f.end()); // 放入first集合
			} else {
				ret.insert(f.begin(), f.end());
				break;
			}
		}
		return ret;
	}
}

set<char> LL1::follow(const Prod &prod) {
	set<char> folw = FOLLOW[prod.noTerminal];
	if(folw.size() != 0 && !(folw.size() == 1 && folw.find('#') != folw.end()))
		return folw;

	for(auto p:G) { // 寻找候选式
		for(auto s: p.selection) {
			for(unsigned int i=0; i<s.length(); ++i) {
				if(s[i] == prod.noTerminal) { // 候选式中找到非终结符
					if(i+1 < s.length()) { // 不是最后一个
						char sym = s[i+1];
						if(VT.find(sym) != VT.end() && sym != '@') { // 终结符，加入follow集中
							FOLLOW[prod.noTerminal].insert(sym);
						}
						else { // 非终结符
							set<char> f = FIRST[sym];
							FOLLOW[prod.noTerminal].insert(f.begin(), f.end());
							if(f.find('@') != f.end()) { // 非终结符推出@，移除@符号
								FOLLOW[prod.noTerminal].erase(FOLLOW[prod.noTerminal].find('@'));
								if(i+1 == s.length() - 1 && sym != s[i]) { // 最后一个非终结符能推出@，将Follow(A)加入Follow(B)中
									set<char> ff = follow(p.prod);
									FOLLOW[prod.noTerminal].insert(ff.begin(), ff.end());
								}
							}
						}
					}
					else { // 最后一个符号
						if(p.noTerminal != s[i]) {
							set<char> ff = follow(p.prod); // 将Follow(A)加入Follow(B)中
							FOLLOW[prod.noTerminal].insert(ff.begin(), ff.end());
						}
					}
				}
			}
		}
	}
	return FOLLOW[prod.noTerminal];
}

void LL1::parseTable() { // X->a
	for(auto prod: G) { // 枚举产生式
		for(auto sel:prod.selection) { // 枚举候选式
			set<char> folw = first(sel);
			for(auto terminal: folw)
				if(terminal == '@')  // 存在@，将follow(x)的每个终结符放入表中
					for(auto term: FOLLOW[prod.noTerminal])
						M[make_pair(prod.noTerminal, term)] = sel;
				else
					M[make_pair(prod.noTerminal, terminal)] = sel;
		}
	}
}

void LL1::build() {
	for(auto prod: G) first(prod.prod); // 求first集

	FOLLOW[G[0].noTerminal].insert('#'); // 将结束符放入开始符号中
	for(auto prod: G) follow(prod); // 求follow集

	parseTable(); // 预测分析表
	// 求完表后，将@替换为#
	VT.erase(VT.find('@'));
	VT.insert('#');

	return;
}

void LL1::showIndataStack() {
	for(vector<char>::reverse_iterator it = indata.rbegin(); it != indata.rend(); ++it)
		printf("%c", *it);
}

void LL1::showParseStack() {
	for(vector<char>::iterator it = parse.begin(); it != parse.end(); ++it)
		printf("%c", *it);
}

void LL1::loadIndata(const string &s) {
	indata.push_back('#');
	for(int i=s.length()-1; i>=0; --i)
		indata.push_back(s[i]);
}

void LL1::error() {
	printf("Parse Error!\n");
}

void LL1::parser() {
	parse.push_back('#');
	parse.push_back(G[0].noTerminal); // 文法开始符号
	printf("step\tparseStack\tindataStack\tproduction\n");
	unsigned int ptop, itop, step = 0;
	string prod; // 候选式
	while((ptop = parse.size() - 1) > 0) {
		itop = indata.size() - 1;

		printf("%d\t", step++);
		showParseStack(); printf("\t\t");
		showIndataStack(); printf("\t\t%s\n", prod.c_str());

		// begin
		prod = "";
		char X = parse[ptop];
		char curc = indata[itop];
		parse.pop_back();
		if(VT.find(X) != VT.end()) { // 终结符
			if(X != curc) {
				error();
				break;
			}
			else
				indata.pop_back();
		} else if(X == '@')
			continue;
		else { // 终结符
			prod = M[make_pair(X, curc)];
			if(prod.size()) { // 找得到
				if(prod != "@")
					for(int i=prod.size() - 1; i>=0; --i)
						parse.push_back(prod[i]);
			}
			else {
				error();
				break;
			}
			prod = string(1, X) + "->" + prod;
		}
	}
	printf("%d\t", step++);
	showParseStack(); printf("\t\t");
	showIndataStack(); printf("\t\t%s\n", prod.c_str());
	printf("Parse Success!\n");
}

void LL1::debug() {
	printf("VT: \n");
	for(auto c: VT) {
		printf("%c, ", c);
	}
	printf("\nVN: \n");
	for(auto c: VN)
		printf("%c, ", c);
	puts("");
	for(auto p: G)
		printf("noTerminal: %s\n", p.prod.c_str());
	for(auto prod:G) {
		printf("FIRST(%c)= {", prod.noTerminal);
		for(auto c:FIRST[prod.noTerminal]) {
			printf("%c,", c);
		}
		printf("}\n");
	}

	for(auto prod:G) {
		printf("FOLLOW(%c)= {", prod.noTerminal);
		for(auto c:FOLLOW[prod.noTerminal]) {
			printf("%c,", c);
		}
		printf("}\n");
	}
	printf("Parse Table:\n");
	for(auto c:VT) // 终结符，表头
		printf("\t%c", c);
	puts("");
	for(auto prod:G) {
		printf("%2c|", prod.noTerminal);
		for(auto c:VT) // 终结符
			printf("\t%s", M[make_pair(prod.noTerminal, c)].c_str());
		puts("");
	}

}

int main() {
	LL1 ll;
	string in;
	while(cin >> in && in != "#") // 读取文法
		ll.addProd(Prod(in));
	ll.build();
	ll.debug();

	cin >> in; // 表达式
	ll.loadIndata(in);
	ll.showIndataStack();
	puts("");
	ll.parser();

	return 0;
}
