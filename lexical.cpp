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
			optrs.push_back(make_pair("{", DELIMITER));
			optrs.push_back(make_pair("}", DELIMITER));

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

			row = column = 0;
		}
		bool isKey(const string &str); // 是否为关键字
		int getKeyPointer(const string &str);
		bool isOptr(const string &str); // 是否为运算符
		bool isId(const string &str); // 是否为标识符
		int getIDPointer(const string &str);
		bool isNum(const string &str); // 是否数值
		bool getIn();
		void analysis();
};

const char *Lexical::typeStr[] = {
	"ERROR",
	"KEY",
	"DELIMITER",
	"ARITHMETICOPTR",
	"RELATIONOPTR",
	"NUMBER",
	"ID"
};

string Lexical::cut(int i, int j) {
	return string(in.begin() + i, in.begin() + j);
}

bool Lexical::isKey(const string &str) {
	if(keys.find(str) != -1) return true;
	else return false;
}

int Lexical::getIDPointer(const string &str) {
	vector<pair<string, Type> >::iterator it = find(indetifiers.begin(), indetifiers.end(), make_pair(str, ID));
	if(it != indetifiers.end()) // 找到了
		return it - indetifiers.begin() + 1;
	else {
		indetifiers.push_back(make_pair(str, ID));
		return indetifiers.size();
	}
}

int Lexical::getKeyPointer(const string &str) {
	return keys.find(str);
}

bool Lexical::isOptr(const string &str) {
	for(auto opt:optrs)
		if(opt.first == str) return true;
	return false;
}

bool Lexical::isId(const string &str) {
	if(!isalpha(str.c_str()[0]) && str.c_str()[0] != '_')
		return false;
	for(auto c: str)
		if(!isalnum(c) && c != '_') return false;
	return true;
}
bool Lexical::isNum(const string &str) {
	for(auto c:str)
		if(!isdigit(c)) return false;
	return true;
}


bool Lexical::getIn() {
	++row;
	return getline(cin, in);
}

void Lexical::analysis() {
	unsigned int j = 0;
	for(column = 0; column < in.length(); ++column) {
		char c = in.c_str()[column];
		if(isalpha(c)) {
			for(j = column+1; j < in.length() && (isalnum(in[j]) || in[j] == '_'); ++j); // 匹配关键字或者标识符自动机
			string s = cut(column, j);
			if(isKey(s))
				printf("%s\t(%d, %d)\t%s\tpos(%d, %d)\n", s.c_str(), KEY, getKeyPointer(s), typeStr[KEY], row, column+1);
			else if(isId(s))
				printf("%s\t(%d, %d)\t%s\tpos(%d, %d)\n", s.c_str(), ID, getIDPointer(s), typeStr[ID], row, column+1);
			else
				printf("%s\t%s\t%s\tpos(%d, %d)\n", s.c_str(), typeStr[ERROR], typeStr[ERROR], row, column+1);
			column = j-1;
		}

	}
}


int main() {
	Lexical lex;
	// string s;
	// while(cin >> s) {
		// printf("%s: Key(%d) Optr(%d) Id(%d)\n", s.c_str(), lex.isKey(s), lex.isOptr(s), lex.isId(s));
	// }

	while(lex.getIn()) {
		lex.analysis();
	}
	return 0;
}
