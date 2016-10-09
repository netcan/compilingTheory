/*************************************************************************
	> File Name: lexical.cpp
	  > Author: Netcan
	  > Blog: http://www.netcan666.com
	  > Mail: 1469709759@qq.com
	  > Created Time: 2016-10-05 三 20:34:12 CST
 ************************************************************************/

#include "lexical.h"

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

Lexical::Lexical() {
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
	optrs.push_back(make_pair("<>", RELATIONOPTR));
	optrs.push_back(make_pair("=", RELATIONOPTR));

	row = column = 0;
	isFirst = true;
}


string Lexical::cut(int i, int j) {
	return string(in.begin() + i, in.begin() + j);
}

bool Lexical::isKey(const string &str) {
	if(keys.find(str) != -1) return true;
	else return false;
}


int Lexical::getKeyPointer(const string &str) {
	return keys.find(str);
}

bool Lexical::isOptr(const string &str) {
	for(auto opt:optrs)
		if(opt.first == str) return true;
	return false;
}

int Lexical::getOptrPointer(const string &str) {
	vector<pair<string, Type> >::iterator it;
	for(it = optrs.begin(); it != optrs.end() && it->first != str; ++it);
	return it - optrs.begin();
}

Type Lexical::getOptrType(const string &str) {
	vector<pair<string, Type> >::iterator it;
	for(it = optrs.begin(); it != optrs.end() && it->first != str; ++it);
	if(it != optrs.end())
		return it->second;
	else return ERROR;
}

bool Lexical::isId(const string &str) {
	if(!isalpha(str.c_str()[0]) && str.c_str()[0] != '_')
		return false;
	for(auto c: str)
		if(!isalnum(c) && c != '_') return false;
	return true;
}

int Lexical::getIDPointer(const string &str) {
	vector<pair<string, Type> >::iterator it = find(indetifiers.begin(), indetifiers.end(), make_pair(str, ID));
	if(it != indetifiers.end()) // 找到了
		return it - indetifiers.begin();
	else {
		indetifiers.push_back(make_pair(str, ID));
		return indetifiers.size() - 1;
	}
}

bool Lexical::isNum(const string &str) {
	for(auto c:str)
		if(!isdigit(c)) return false;
	return true;
}

int Lexical::getNumPointer(const string &str) {
	vector<pair<string, Type> >::iterator it = find(constants.begin(), constants.end(), make_pair(str, NUMBER));
	if(it != constants.end()) // 找到了
		return it - constants.begin();
	else {
		constants.push_back(make_pair(str, NUMBER));
		return constants.size() - 1;
	}
}

bool Lexical::isString(const string &str) { // 是否字符串
	return true;
}

int Lexical::getStringPointer(const string &str) {
	vector<pair<string, Type> >::iterator it = find(strings.begin(), strings.end(), make_pair(str, STRING));
	if(it != strings.end()) // 找到了
		return it - strings.begin();
	else {
		strings.push_back(make_pair(str, STRING));
		return strings.size() - 1;
	}
}

bool Lexical::isChar(const string &chr) {
	if(chr.length() == 1 || (chr[0] == '\\' && chr.length() == 2))
		return true;
	else
		return false;
}

int Lexical::getCharPointer(const string &chr) {
	vector<pair<char, Type> >::iterator it = find(chars.begin(), chars.end(), make_pair(chr.c_str()[0], CHAR));
	if(it != chars.end()) // 找到了
		return it - chars.begin();
	else {
		chars.push_back(make_pair(chr.c_str()[0], CHAR));
		return chars.size() - 1;
	}
};

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

			if(!isFirst) printf(", ");
			else isFirst = false;

			if(isKey(s))
				printf("{\"word\": \"%s\", \"tuple\": [%d, %d], \"type\": \"%s\", \"pos\": [%d, %d]}\n", s.c_str(), KEY, getKeyPointer(s), typeStr[KEY], row, column+1);
			else if(isId(s))
				printf("{\"word\": \"%s\", \"tuple\": [%d, %d], \"type\": \"%s\", \"pos\": [%d, %d]}\n", s.c_str(), ID, getIDPointer(s), typeStr[ID], row, column+1);
			else
				printf("{\"word\": \"%s\", \"tuple\": [%d, %d], \"type\": \"%s\", \"pos\": [%d, %d]}\n", s.c_str(), ERROR, ERROR, typeStr[ERROR], row, column+1);

			column = j-1;
		}
		else if(isdigit(c)) {
			for(j = column+1; j < in.length() && (isalnum(in[j]) || in[j] == '_'); ++j); // 匹配数字字符串自动机
			string s = cut(column, j);

			if(!isFirst) printf(", ");
			else isFirst = false;
			if(isNum(s))
				printf("{\"word\": \"%s\", \"tuple\": [%d, %d], \"type\": \"%s\", \"pos\": [%d, %d]}\n", s.c_str(), NUMBER, getNumPointer(s), typeStr[NUMBER], row, column+1);
			else
				printf("{\"word\": \"%s\", \"tuple\": [%d, %d], \"type\": \"%s\", \"pos\": [%d, %d]}\n", s.c_str(), ERROR, ERROR, typeStr[ERROR], row, column+1);

			column = j - 1;
		}
		else if(c == '/' && in[column+1] == '/') { // 注释
			for(column += 2; column < in.length() && isspace(column); ++column);
			string s = cut(column, in.length());

			if(!isFirst) printf(", ");
			else isFirst = false;
			printf("{\"word\": \"%s\", \"tuple\": [%d, %d], \"type\": \"%s\", \"pos\": [%d, %d]}\n", s.c_str(), COMMENT, 0, typeStr[COMMENT], row, column+1);
			column = in.length();
		}
		else if(isOptr(string(1, c))){
			for(j = column+1; j < in.length() && isOptr(string(1, in[j])) && getOptrType(string(1, in[j])) != DELIMITER && getOptrType(string(1, in[j])) == getOptrType(string(1,c)); ++j); // 运算符自动机
			string s = cut(column, j);

			if(!isFirst) printf(", ");
			else isFirst = false;
			if(isOptr(s))
				printf("{\"word\": \"%s\", \"tuple\": [%d, %d], \"type\": \"%s\", \"pos\": [%d, %d]}\n", s.c_str(), getOptrType(s), getOptrPointer(s), typeStr[getOptrType(s)], row, column+1);
			else
				printf("{\"word\": \"%s\", \"tuple\": [%d, %d], \"type\": \"%s\", \"pos\": [%d, %d]}\n", s.c_str(), ERROR, ERROR, typeStr[ERROR], row, column+1);

			column = j - 1;
		}
		else if(c == '"' || c == '\'') { // 字符串
			for(j = column+1; j < in.length() && ( (in[j]=='\\' && ++j) || in[j] != c); ++j); // 字符(串)自动机
			string s = cut(column+1, j);
			// printf("%s(%ld)\n", s.c_str(), s.length());

			if(!isFirst) printf(", ");
			else isFirst = false;
			if(c == '"' && j < in.length())
				printf("{\"word\": \"%s\", \"tuple\": [%d, %d], \"type\": \"%s\", \"pos\": [%d, %d]}\n", s.c_str(), STRING, getStringPointer(s), typeStr[STRING], row, column+1);
			else if(c == '\'' && isChar(s))
				printf("{\"word\": \"%s\", \"tuple\": [%d, %d], \"type\": \"%s\", \"pos\": [%d, %d]}\n", s.c_str(), CHAR, getCharPointer(s), typeStr[CHAR], row, column+1);
			else
				printf("{\"word\": \"%s\", \"tuple\": [%d, %d], \"type\": \"%s\", \"pos\": [%d, %d]}\n", s.c_str(), ERROR, ERROR, typeStr[ERROR], row, column+1);
			column = j;
		}
		else if(!isspace(c)) {
			if(!isFirst) printf(", ");
			else isFirst = false;
			if(c=='"')
				printf("{\"word\": \"\\\"\", \"tuple\": [%d, %d], \"type\": \"%s\", \"pos\": [%d, %d]}\n", ERROR, ERROR, typeStr[ERROR], row, column+1);
			else
				printf("{\"word\": \"%s\", \"tuple\": [%d, %d], \"type\": \"%s\", \"pos\": [%d, %d]}\n", string(1,c).c_str(), ERROR, ERROR, typeStr[ERROR], row, column+1);
		}
	}
}

void Lexical::run() {
	while(getIn()) {
		analysis();
	}
}

int main() {
	Lexical lex;
	lex.run();

	return 0;
}
