/*************************************************************************
	> File Name: lexical.h
	  > Author: Netcan
	  > Blog: http://www.netcan666.com
	  > Mail: 1469709759@qq.com
	  > Created Time: 2016-10-06 四 10:05:08 CST
 ************************************************************************/

#ifndef LEXICAL_H
#define LEXICAL_H
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <ctype.h>
#include <cstdio>
using namespace std;

struct Trie {
	Trie *next[26];
	bool isEnd; // 字符串是否是结尾
	int loc; // 字符串的位置
	Trie() {
		isEnd = false;
		for(int i=0; i<26; ++i) next[i] = NULL;
	}
};

class Key { // 存放关键字数据结构，采用字典树结构
	private:
		Trie *root;
	public:
		Key() {
			root = new Trie();
		}
		void add(const string &str, int loc); // 添加关键字
		void add(vector<string> strs);
		int find(const string &str); // 查找关键字
		void free(Trie *p); // 释放关键字
		~Key() { // 释放关键字
			free(root);
		}
};

enum Type {
	ERROR = 0,
	KEY,
	DELIMITER,
	ARITHMETICOPTR,
	RELATIONOPTR,
	NUMBER,
	ID
};

struct Symbol {
	Type type;
	string optr;
	Symbol(Type type,string optr) : type(type), optr(optr) {};
	friend bool operator==(const Symbol &a,const Symbol &b) {
		return a.optr == b.optr;
	}
};

class Lexical { // 词法分析
	private:
		Key keys; // 关键字
		vector<pair<string, Type> > optrs; // 运算符
		vector<pair<string, Type> > indetifiers; // 标识符
		vector<pair<string, Type> > constants; // 常量
		unsigned int row, column;
		string in; // 输入程序
		static const char* typeStr[];

		string cut(int i, int j); // 截取in的字符串[i, j)
	public:
		Lexical();
		bool isKey(const string &str); // 是否为关键字
		int getKeyPointer(const string &str);
		bool isOptr(const string &str); // 是否为运算符
		int getOptrPointer(const string &str); // 查找运算符位置
		Type getOptrType(const string &str); // 查找运算符类型
		bool isId(const string &str); // 是否为标识符
		int getIDPointer(const string &str);
		bool isNum(const string &str); // 是否数值
		int getNumPointer(const string &str);
		bool getIn();
		void analysis();
		void run();
};


#endif
