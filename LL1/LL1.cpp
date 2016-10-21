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

#include "LL1.h"

Prod::Prod(const string &in) {
	prod = in;
	isValid = false;
	split();
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

	return isValid = true;
}

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
			set<char> f = first(sel); // 求出候选式的first集合
			FIRST[prod.noTerminal].insert(f.begin(), f.end());
		}
		return FIRST[prod.noTerminal];
	}
	else if(s.length() == 0)  // 空串
			return set<char>({'@'});
	else if(s.length() == 1){ // 终结符或非终结符
		if(VT.find(s[0]) != VT.end())  // 终结符
			return set<char>({s[0]});
		else { // 非终结符
			if(FIRST[s[0]].size() != 0) return FIRST[s[0]];
			else {
				vector<Prod>::iterator it = find(G.begin(), G.end(), s[0]); // 求出非终结符的产生式
				if(it != G.end()) { // 找到产生式
					set<char> f = first(it->prod);
					FIRST[s[0]].insert(f.begin(), f.end());
				}
				return FIRST[s[0]];
			}
		}
	} else { // 候选式X1X2X3X4...
		set<char> ret;
		for(unsigned int i=0; i<s.length(); ++i) {
			set<char> f = first(string(1, s[i])); // 逐个符号求first(Xi)集
			if(f.find('@') != f.end() && s.length() - 1 != i) { // 发现@
				f.erase(f.find('@')); // 减去@
				ret.insert(f.begin(), f.end()); // 放入first集合
			} else { // 无@，则不需要求下去了
				ret.insert(f.begin(), f.end());
				break;
			}
		}
		return ret;
	}
}

// A->aXb，求follow(X)
void LL1::follow() { // 非递归求法，防止死递归
	FOLLOW[G[0].noTerminal].insert('#'); // 开始符号放'#'
	for(auto pp: G) { // 直到follow(X)不在增大
		unsigned int size = 0;
		while(size != FOLLOW[pp.noTerminal].size()) {
			size = FOLLOW[pp.noTerminal].size();
			for(auto prod: G) { // 求出所有非终结符的follow集合
				char X = prod.noTerminal;
				for(auto p: G) // 求出X的follow集合
					for(auto s: p.selection) { // 逐个候选式找X
						unsigned long loc = 0;
						if((loc = s.find(X)) != string::npos) { // 找到非终结符X
							set<char> f = first(string(s.begin() + loc + 1, s.end())); // 求first(b)
							FOLLOW[X].insert(f.begin(), f.end()); // 加入到follow(X)中
							if(f.find('@') != f.end()) {// 找到@
								FOLLOW[X].erase(FOLLOW[X].find('@')); // 删除@
								set<char> fw = FOLLOW[p.noTerminal]; // 把follow(A)放入follow(X)
								FOLLOW[X].insert(fw.begin(), fw.end());
							}
						}
					}
			}
		}
	}
}

void LL1::parseTable() { // X->a
	for(auto prod: G) { // 枚举产生式
		for(auto sel:prod.selection) { // 枚举候选式
			set<char> f = first(sel); // 求first集合
			for(auto terminal: f)
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

	// 求follow集
	follow();

	parseTable(); // 预测分析表
	info();
	// 求完表后，将@替换为#
	if(VT.find('@') != VT.end())
		VT.erase(VT.find('@'));
	VT.insert('#');
	tableInfo();

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

void LL1::error(int step) {
	printf("{ \"step\": %d, ", step++);

	printf("\"parseStack\": \"ERROR\""
			", \"indataStack\": \"ERROR\""
			", \"production\": \"ERROR\""
			"}\n");
}

void LL1::parser() {
	parse.push_back('#');
	parse.push_back(G[0].noTerminal); // 文法开始符号
	// printf("step\tparseStack\tindataStack\tproduction\n");
	printf("\"Parser\": [");
	int i = 0; // 处理打印的第一个逗号

	int ptop, itop, step = 0;
	string prod = ""; // 候选式
	while((ptop = parse.size() - 1) >= 0) {
		itop = indata.size() - 1;

		printf("%s{ \"step\": %d, ", i++==0?" ":", ", step++);

		printf("\"parseStack\": \"");
		showParseStack();
		printf("\", \"indataStack\": \"");
		showIndataStack();
		printf("\", \"production\": \"%s\"", prod.c_str());
		printf("}\n");

		// begin
		prod = "";
		char X = parse[ptop];
		char curc = indata[itop];
		parse.pop_back();
		if(VT.find(X) != VT.end()) { // 终结符
			if(X != curc) {
				printf("%s", i==0?" ":", ");
				error(step);
				break;
			}
			else {
				indata.pop_back();
				prod = "match " + string(1, X);
			}
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
				printf("%s", i==0?" ":", ");
				error(step);
				break;
			}
			prod = string(1, X) + "->" + prod;
		}
	}

	printf("]\n");
	// printf("Parse Success!\n");
}

void LL1::info() {
	printf("\"VT\": [");
	int i = 0; // 处理打印的第一个逗号

	for(auto c: VT) {
		printf("%s\"%c\"", i == 0?" ":", ", c);
		++i;
	}
	printf("], \n");

	i = 0;
	printf("\"VN\": [");
	for(auto c: VN) {
		printf("%s\"%c\"", i == 0?" ":", ", c);
		++i;
	}
	printf("], \n");

	// for(auto p: G)
		// printf("noTerminal: %s\n", p.prod.c_str());

	printf("\"FIRST\": [");
	i = 0;
	for(auto prod:G) {
		printf("\n%c{ \"noTerminal\": \"%c\", \"Terminal\": [", i==0?' ':',', prod.noTerminal);
		int j = 0;
		for(auto c:FIRST[prod.noTerminal]) {
			printf("%s\"%c\"", j == 0?" ":", ", c);
			++j;
		}
		printf("] }");
		++i;
	}
	printf("], \n");

	printf("\"FOLLOW\": [");
	i = 0;
	for(auto prod:G) {
		printf("\n%c{ \"noTerminal\": \"%c\", \"Terminal\": [", i==0?' ':',', prod.noTerminal);
		int j = 0;
		for(auto c:FOLLOW[prod.noTerminal]) {
			printf("%s\"%c\"", j == 0?" ":", ", c);
			++j;
		}
		printf("] }");
		++i;
	}
	printf("], \n");
}

void LL1::tableInfo() {
	printf("\"Table\": {\n");
	printf("\"Header\": [");
	for(auto c:VT)  // 终结符，表头， #号放在最后一列
		if(c != '#')
			printf("\"%c\", ", c);
	printf("\"#\"");
	printf("], \n");

	printf("\"Body\": [");
	int i = 0;
	for(auto prod:G) {
		printf("%s{\"noTerminal\": \"%c\", ", i == 0?" ":", ", prod.noTerminal);
		printf("\"production\": [");
		for(auto c:VT) // 终结符
			if(c != '#')
				printf("\"%s\", ", M[make_pair(prod.noTerminal, c)].c_str());
		printf("\"%s\"", M[make_pair(prod.noTerminal, '#')].c_str());

		printf("]}\n");
		++i;
	}
	printf("] }, \n");
}

void LL1::run() {
	puts("{"); // 输出json，与前端交互
	string in;
	while(cin >> in && in != "#") // 读取文法
		addProd(Prod(in));
	in = "";
	cin >> in; // 表达式
	if(in == "#" || in.size() == 0) return;
	loadIndata(in);

	build();
	parser();
	puts("}");
}

int main() {
	LL1 ll;
	ll.run();

	return 0;
}
