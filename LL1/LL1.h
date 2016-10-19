/*************************************************************************
	> File Name: LL1.h
	  > Author: Netcan
	  > Blog: http://www.netcan666.com
	  > Mail: 1469709759@qq.com
	  > Created Time: 2016-10-11 二 23:18:13 CST

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

#ifndef LL1_H
#define LL1_H

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

class LL1 {
	private: vector<Prod> G; // 文法G
		set<char> VN; // 非终结符
		set<char> VT; // 终结符
		map<char, set<char> > FIRST; // first集
		map<char, set<char> > FOLLOW; // follow集
		map<pair<char, char>, string> M; // 分析表
		set<char> first(const string &s);
		void follow();
		vector<char> parse; // 分析栈
		vector<char> indata; // 输入表达式栈
		void parseTable();
	public:
		bool addProd(const Prod & prod); // 添加产生式
		void info(); // 输出相关结果
		void tableInfo(); // 输出表
		void build(); // 建立first、follow集、分析表
		void showIndataStack(); // 输出输入串内容
		void showParseStack(); // 输出分析栈内容
		void loadIndata(const string &s); // 输入串入栈
		void parser(); // LL1预测分析
		void error(int step); // 错误处理
		void run(); // 运行LL1
};

#endif
