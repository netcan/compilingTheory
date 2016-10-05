/*************************************************************************
	> File Name: lexical.cpp
	  > Author: Netcan
	  > Blog: http://www.netcan666.com
	  > Mail: 1469709759@qq.com
	  > Created Time: 2016-10-05 三 20:34:12 CST
 ************************************************************************/

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <stdio.h>
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

void Key::add(const string &str, int loc) {
	Trie *p = this->root, *q;
	for(unsigned int i=0; i<str.length(); ++i) {
		int id = str[i] - 'a';
		if(p->next[id] == NULL) {
			q = new Trie();
			p->next[id] = q;
		}
		p = p->next[id];
	}
	p->isEnd = true;
	p->loc = loc;
}

void Key::add(vector<string> strs) {
	int loc = 0;
	for(auto str : strs) {
		this->add(str, loc++);
	}
}

int Key::find(const string &str) {
	Trie *p = this->root;
	for(unsigned int i=0; i<str.length(); ++i) {
		int id = str[i] - 'a';
		if(id < 0 || id >= 26)
			return -1;
		p = p->next[id];
		if(p == NULL) return -1;
	}
	if(p->isEnd) return p->loc;
	else return -1;
}

void Key::free(Trie *p) {
	if(p == NULL) return;
	for(int i=0; i<26; ++i) this->free(p->next[i]);
	delete p;
}

template<class T, size_t n> // 数组末尾元素指针
	T* tail(T (&a)[n]) {
		return a+n;
	}

enum Type {
	ERROR = 0,
	KEY = 1,
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
		Key keys;
		vector<pair<string, Type> > optrs;
	public:
		Lexical() {
			const char * ks[] = { // 关键字表
				"auto", "double", "int", "struct", "break", "else", "long",
				"switch","case", "enum", "register", "typedef", "char",
				"extern", "return", "union","const", "float", "short",
				"unsigned", "continue", "for", "signed", "void","default",
				"goto", "sizeof", "volatile", "do", "if", "while", "static"
			};
			keys.add(vector<string>(ks, tail(ks)));

			// 分界符
			optrs.push_back(make_pair(",", DELIMITER));
			optrs.push_back(make_pair(";", DELIMITER));
			optrs.push_back(make_pair("(", DELIMITER));
			optrs.push_back(make_pair(")", DELIMITER));
			optrs.push_back(make_pair("[", DELIMITER));
			optrs.push_back(make_pair("]", DELIMITER));

			// 算术运算符
			optrs.push_back(make_pair("+", ARITHMETICOPTR));
			optrs.push_back(make_pair("-", ARITHMETICOPTR));
			optrs.push_back(make_pair("*", ARITHMETICOPTR));
			optrs.push_back(make_pair("/", ARITHMETICOPTR));
			optrs.push_back(make_pair("%", ARITHMETICOPTR));
			optrs.push_back(make_pair("++", ARITHMETICOPTR));
			optrs.push_back(make_pair("--", ARITHMETICOPTR));

			// 关系运算符
			optrs.push_back(make_pair(">", RELATIONOPTR));
			optrs.push_back(make_pair(">=", RELATIONOPTR));
			optrs.push_back(make_pair("<", RELATIONOPTR));
			optrs.push_back(make_pair("<=", RELATIONOPTR));
			optrs.push_back(make_pair("==", RELATIONOPTR));
			optrs.push_back(make_pair("!=", RELATIONOPTR));

		}
		bool isKey(const string &str); // 是否为关键字
		bool isOptr(const string &str); // 是否为运算符
};

bool Lexical::isKey(const string &str) {
	if(this->keys.find(str) != -1) return true;
	else return false;
}

bool Lexical::isOptr(const string &str) {
	for(auto opt:optrs)
		if(opt.first == str) return true;
	return false;
}

int main() {
	Lexical lex;
	string str;
	while(1) {
		cin >> str;
		printf("%s: key(%d) optr(%d)\n",
				str.c_str(), lex.isKey(str), lex.isOptr(str));
	}
	return 0;
}
