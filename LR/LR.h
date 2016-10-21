/*************************************************************************
	> File Name: LR.h
	  > Author: Netcan
	  > Blog: http://www.netcan666.com
	  > Mail: 1469709759@qq.com
	  > Created Time: 2016-10-19 三 08:36:05 CST

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

#ifndef _LR_H
#define _LR_H

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
		static string replaceAll(const string &in, const string from, const string to);
		string displayStr() const;
		Prod(const string &in);
		Prod(const char &noTerminal, const string& right, const set<char>& additionalVt):
			noTerminal(noTerminal), right(right), additionalVt(additionalVt) {}
};

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

class LR {
	private:
		Item G; // 文法G
		enum actionStat{
			ACCEPT=0,
			SHIFT,
			REDUCE,
		};
		static const char *actionStatStr[];

		vector<Item> C; // 项目集规范族
		map<pair<int, char>, int> GOTO; // goto数组，项目集<int, int>=char
		map<pair<int, char>, pair<actionStat, int> > ACTION; // Action数组，Action[(i, a)]=(s|r)j
		map<char, set<char> > FIRST; // first集
		map<char, set<char> > FOLLOW; // follow集
		set<char> first(const string &s); // 求first集
		void follow();
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
		void showGrammar(); // 显示输入的文法产生式
		void drawGraph(); // 画出DFA！
		void generateDot();
		void run();
};

#endif
