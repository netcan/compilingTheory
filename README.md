# Introduction
合肥工业大学编译原理课设。
# Screencast
## Lexical
![lexical.gif](lexical.gif)

## LL1 Parser
![ll1.gif](ll1.gif)

## LR1 Parser
![LR.gif](LR.gif)

# 词法分析器
[http://www.netcan666.com/2016/10/07/%E7%BC%96%E8%AF%91%E5%8E%9F%E7%90%86%E4%B9%8B%E8%AF%8D%E6%B3%95%E5%88%86%E6%9E%90%E5%99%A8%E8%AE%BE%E8%AE%A1/](http://www.netcan666.com/2016/10/07/%E7%BC%96%E8%AF%91%E5%8E%9F%E7%90%86%E4%B9%8B%E8%AF%8D%E6%B3%95%E5%88%86%E6%9E%90%E5%99%A8%E8%AE%BE%E8%AE%A1/)

这是编译原理的第一个实验，算是热身实验吧，确实很简单，花了一晚上就把词法分析器底层部分写完了，老师比较喜欢图形界面，后来又加了前端，也就是现在看到的效果。实验要求能够匹配出**关键字**、**标记符**、**运算符**、**分界符**、**无符号数**，后来我又添加了一部分，现在能匹配出**字符/字符串**、**行间注释**。

## 词法分析器底层部分
底层部分是用C++写的，大体思路就是，每次从`stdin`读取出一行，然后从这行的第一个字符开始匹配。匹配完了，读取下一行，行号+1。

### 匹配**关键字**或**标记符**自动机
若当前匹配到的字符i是*字母*，就继续匹配下一个字符，直到下个字符j不是*字母*或者*数字*或者'_'为止，则截取字符串(i, j)，判断这个字符串是不是**关键字**或者**标记符**，否则**错误**处理。如果是**标记符**，将其存入**标记符**表中，其在**标记符**表的位置即为其`Pointer`。最后输出相关信息。

### 匹配**无符号数**自动机
若当前匹配到的字符i是*数字*，就继续匹配下一个字符，直到下个字符j不是*字母*或者*数字*或者'_'为止，则截取字符串(i, j)，判断这个字符串是不是**无符号数**。如果是**无符号数**，将其存入**常数**表中，其在**常数**表的位置即为其`Pointer`，若不是**无符号数**则当**错误**处理。最后输出相关信息。

### 匹配**行间注释**自动机
若当前匹配到的字符i是'/'并且下一个字符也是'/'，就继续匹配下一个字符，直到下个字符j不是*空白*（空格或tab）为止，则截取字符串(j, lineEnd)，作为注释处理。

### 匹配**运算符**、**分界符**自动机
我将**运算符**和**分界符**放到一个`optrs`表中，若当前匹配到的字符i是`optrs`的元素，就继续匹配下一个字符，直到下个字符j不是`optrs`的元素或者运算符类型与字符i不一样或者就是分界符为止，则截取字符串(i, j)，判断这个字符串是不是`optrs`的元素，并确定其类型，其Pointer为该**运算符**或**分界符**在`optrs`的位置，输出相关信息，否则当**错误**处理。最后输出相关信息。

### 匹配**字符**、**字符串**自动机
若当前匹配到的字符i是`"`或者`'`，就继续匹配下一个字符，直到下个字符j是字符i为止，则截取字符串(i, j)，判断这个字符串是**字符**还是**字符串**。如果是的话，将其存入**字符**或**字符串**表中，其在相应表的位置即为其`Pointer`，若不符合则**错误**处理。最后输出相关信息。

## 词法分析器前端部分
前端部分我比较认真，我用`html`+`js`+`php`来实现图形界面，之所以写成网页，是因为我不想写`native app`，我也没`GUI`开发环境。在互联网时代，`webapp`是趋势，谁还写本地客户端啊，况且带个几十M的`GUI`库实在是麻烦。

于是我的分析器底层部分设计成输出`json`格式，然后利用管道将`C++`程序与`php`程序进行数据传送。前端只要用`js`输数据取数据渲染页面即可。

在这之中发现一个问题，如果*输入文本*一长，渲染效率大大降低，因为我用`append`方法一个个加元素的。解决方案是最后转换为字符串一次性输出渲染，效率提高了不少。具体可看这个优化片段：[优化js运行效率](https://github.com/netcan/compilingTheory/commit/2a09a9054e467d35d416603c5d73247acf2af764)。

# LL1语法分析器
[http://www.netcan666.com/2016/10/09/%E7%BC%96%E8%AF%91%E5%8E%9F%E7%90%86%E4%B9%8BLL-1-%E8%AF%AD%E6%B3%95%E5%88%86%E6%9E%90/](http://www.netcan666.com/2016/10/09/%E7%BC%96%E8%AF%91%E5%8E%9F%E7%90%86%E4%B9%8BLL-1-%E8%AF%AD%E6%B3%95%E5%88%86%E6%9E%90/)

我写这篇博文写了好几天了，描述比较凌乱，建议大家还是看书吧，或者直接看我[程序设计部分](#程序设计)。一定要搞懂first集和follow集的求法，不然写程序也会遇到困难的，这里有篇不错的关于求first和follow集的论文，推荐看看：[https://www.cs.virginia.edu/~weimer/2008-415/reading/FirstFollowLL.pdf](https://www.cs.virginia.edu/~weimer/2008-415/reading/FirstFollowLL.pdf)

LL(1)文法主要是求first集和follow集，个人觉得follow集比较麻烦点，然后就是用这两个集合求预测分析表。

## first集
first集就是求文法符号串\alpha的开始符号集合first(\alpha)，例如有以下文法G(E)：
 
E\to TE' \\
E'\to +TE'|\varepsilon\\
T\to FT'\\
T'\to *FT'|\varepsilon\\
F\to (E)|\bf{id}


用例子还是比较好说明的，很容易求出各非终结符的first集。

first(E) = first(T) = first(F) = \{(, \bf{id}\}\\
first(E') = \{+, \varepsilon\}\\
first(T') = \{*, \varepsilon\}


这里给出first集的一般描述。


FIRST(\alpha) =\{t|(t\ is\ a\ terminal\ and\ \alpha\Rightarrow^∗ t\beta)or(t=\varepsilon\ and\ \alpha \Rightarrow^* \varepsilon)\}


也就是说，如果非终结符E有产生式T\beta，那么它们first(E)=first(T)，这个不难理解，因为我（E）能推出你（T），你又能推出它（开始符号，终结符），那我们都能推出它。

first集的作用是，当用来匹配开头为a的文本时，有产生式A\to X\alpha|Y\beta，若a\in X，则选择候选式A\to X\alpha，若a\in Y，选择A\to Y\beta，说了那么多，我只想说，不是用first(A)这个来匹配a，而是用它的候选式first(X)或者first(Y)来匹配。

用上面的例子来说，匹配(233，因为(\in first(TE')，应该选择E'\to +TE'这个候选式。

## follow集
follow集比较抽象，它用来解决这个问题的，如果非终结符E'的first(E')含有\varepsilon，那么选择会不确定，比如说G(E)，

E\to Tb|F\\
T\to a|\varepsilon\\
F\to c


很容易求得

first(Tb) = \{a, \varepsilon\} \\
first(F) = {c}


匹配开头为a，因为a\in first(Tb)，选择E\to Tb产生式，匹配开头为c，因为c\in first(F)，选择E\to F产生式。然而当我匹配b时，因为b\not \in first(Tb)\land \not \in first(F)，这时候就不知道选择哪个产生式了，但是因为\varepsilon\in first(Tb)，且E\to Tb|F\Rightarrow (a|\varepsilon)b|F\Rightarrow ab|b|F，应该选择E\to Tb的，这说明了first集的不足，从而引进follow集。

给出定义，follow(A)即为非终结符A后面可能接的符号集。

至于怎么求follow(A)，我就直接摘抄PPT的定义吧，然后在说明。

连续使用下面的规则，直至每个follow不再增大为止。
* 对于文法的开始符号S，置#于follow(S)中。
* 若A\to \alpha B\beta是一个产生式，则把first(\beta)\backslash \{\varepsilon \}加至follow(B)中；
* 若A\to \alpha B是一个产生式，或A\to \alpha B\beta 是一个产生式而\varepsilon\in FIRST(\beta)，则把follow(A)加至follow(B)中。 

第三条规则可以这么理解，因为B后面啥都没，A又能推出B，所以应该把A后面的符号（follow(A)）加入follow(B)中。需要注意的是，follow集不含\varepsilon。

所以要求某个非终结符的follow集，只要在**产生式右边**中找，然后根据它后面一个**符号**按照上述规则计算就行了。

## 预测分析表
求得first集和follow集后，求分析表就比较容易了。这里简单说下构建方法。

对文法的每个产生式，执行下面步骤。

* 对first(\alpha)的每个**终结符**a，将候选式A\to \alpha加入M[A, a]
* 如果\varepsilon\in first(\alpha)，把follow(A)的每个**终结符**b（包括#），把A\to \alpha加入M[A, b]。

## 程序设计
代码的核心部分就是求first集和follow集了，这也是程序的精髓所在。

首先我约定字符@代表\varepsilon，因为键盘上没那个符号，所以随便找了个合适的符号代替。约定单个大写字母代表非终结符，小写字母或某些符号代表终结符。

然后设计一个叫做产生式的类`Prod`，它的构造函数接受一个字符串，字符串描述了一个产生式。然后分割字符串，把它的非终结符存入`noTerminal`成员中，候选式存入`selection`集合中。

接着设计一个叫`LL1`的类，也就是核心部分了，它提供了求`first`、`follow`、分析表等方法。因为`first`集和`follow`集可能会求未知表达式的`first`集和`follow`集，比如说`A->B，B->C`，欲求`first(A)`，需求`first(B)`，欲求`first(B)`，需求`first(C)`，从而确定了这两种集合求法只能用递归来求，这也是我所能想到的最简单求法，可以参考我代码。

然而现在（2016/10/21）补充下，经过反复调教，`first`集都重写了5次，而`follow`集是**不能用递归**来求的，因为有可能出现这种情况：

A\to bAS\\
S\to aA\\
S\to d\\
A\to \varepsilon


求`follow(A)`需要`first(S)`和`follow(S)`，递归求`follow(S)`需要`follow(A)`，然而这样就进入了死递归。所以最后我改写成5个循环搞定。


求出了`first`和`follow`，剩下的就好办了。至于图形界面，和[上次](/2016/10/07/编译原理之词法分析器设计/)一样，套了个`php`，通过`php`传json数据到前端，前端输入数据取数据，渲染页面。那颗语法树的画法，通过前序遍历画得。

# LR1语法分析器
[http://www.netcan666.com/2016/10/21/%E7%BC%96%E8%AF%91%E5%8E%9F%E7%90%86%E5%AE%9E%E9%AA%8C%E4%B9%8BLR-1-%E5%88%86%E6%9E%90%E5%99%A8%E8%AE%BE%E8%AE%A1/](http://www.netcan666.com/2016/10/21/%E7%BC%96%E8%AF%91%E5%8E%9F%E7%90%86%E5%AE%9E%E9%AA%8C%E4%B9%8BLR-1-%E5%88%86%E6%9E%90%E5%99%A8%E8%AE%BE%E8%AE%A1/)

先来吐槽下，据说这个实验是最难的一个实验，当然也是最后的一个实验了。在我们实验室上一届学长只有一个人写出来，可见其难度。

![http://7xqytu.com1.z0.glb.clouddn.com//2016/10/QQ%E6%88%AA%E5%9B%BE20161021212712.png](http://7xqytu.com1.z0.glb.clouddn.com//2016/10/QQ%E6%88%AA%E5%9B%BE20161021212712.png)

然而我并不觉得有什么难的，当我上课听懂老师讲LR语法分析的时候，我就疑惑了，难点在哪？学长说难在自动机的构建上，自动机比较难拍，不好用数据结构来描述。这当然给了我巨大信心，回去不到一天把LR分析器核心拍出来了，在没有参考任何代码、龙书，仅看着教科书上的算法来写的。

## 产生式`Prod`类
我来说下具体是怎么实现的吧，用面向对象来写比较好写，绝对比面向过程好写。先来看看我设计的最小的类`Prod`（为减小篇幅已删除无关紧要的函数）。

```cpp
class Prod { // 这里是存放形如X->abc的形式，不存在多个候选式
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
		Prod(const char &noTerminal, const string& right, const set<char>& additionalVt):
			noTerminal(noTerminal), right(right), additionalVt(additionalVt) {}
};
```

这个类是存放产生式的，存放形如`A->Bc`，这里的`noTerminal`就是左边的`A`，`right`就是右边的`Bc`，而`additionalVt`是`LR(1)`的项目集的**搜索符**，长度为1所以叫`LR(1)`。我重载了`==`符号，后面用来搜索项目集/文法中是否有这个产生式简直不能再方便，构造函数也能快速构造产生式，无疑为后面项目集中插入产生式提供了方便。

## 项目集`Item`类
```cpp
class Item { // 项目集
	private:
		vector<Prod> prods; // 项目集产生式
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
};
```

项目集`Item`类中，`prods`向量存放这个项目集的所有**项目**（即产生式+**搜索符**），而`Vn/Vt`集合存放了**所有**产生式的非终结符/终结符，`Symbol`仅仅是`Vn/Vt`的并集，为后面`GOTO`函数枚举符号提供了方便，`add`方法为项目集插入项目。这里最重要的就是重载`==`符号了，它是判断两个项目集是否相等的关键，判断也很简单，首先判断两个项目集的项目数量是否相等，若相等进一步判断是否所有的**项目**都相等，这里就展现了前面重载`==`的优点（当然还需要判断**搜索符**是否也都相等）。能判断两个项目集是否相等就好办了，后面求**项目集规范族**插入项目就简单了。

还需要注意的一点，`Prod`类已经重载`==`了，为啥项目集`prods`用向量来存，而不是直接用集合来存？这样就不用判等了，还能二分查找提高了查询效率。但是我考虑到用集合来存的话，会按照字典序来排，但是这样还要重载`<`，会打乱产生式的顺序，所以我后面的`LR`类来存放文法产生式、**项目集规范族**也是用向量来存（插入的时候判等就是了），而不是集合，以免打乱了顺序。

## `LR`类
```cpp
class LR {
	private:
		Item G; // 文法G
		enum actionStat{
			ACCEPT=0,
			SHIFT,
			REDUCE,
		};

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
	public:
		void add(const string &s); // 添加产生式
		void parser(); // LR(1)分析
};
```

这是最后一个类了，重点说说。`G`用来存放输入的文法，把它看成一个项目集吧。`C`是**项目集规范族**，也就是项目集的集合，用向量来存。`actionStat`为枚举类型，用于表示`ACTION`的动作类型。`GOTO`用于存放自动机DFA上**边**，边`w(i, j)=X`，`ACTION`用于存放动作，`Action[(i, X)] = ((s|r)j)|acc`，即当状态`i`遇到**终结符X**的时候采取的动作，移进/规约/接受。`FIRST`集合存放各个**非终结符**的`FIRST`集合，`first`方法求集合会记忆化存储到`FIRST`集合里。接下来就是那三个栈了，还有`Closure`、`Goto`、`items`、`parser`这几个方法了，书上有我就不细讲了，设计好这几个数据结构，写起来会轻松很多。

## 空串处理
那时候我的分析器还不能处理形如A\to \varepsilon的产生式，书上、指导书给的文法，都没这类产生式。我就想，是不是`LR`不能处理空串啊？因为`LR`的`DFA`构建过程中，如果引出\varepsilon这条边，那就是`NFA`了，这样还要把它化为`DFA`，那就很麻烦了。请教了一下李老师，老师说处理A\to \varepsilon项目的时候，项目直接写成A\to .，也就是说求`GOTO`函数的时候不要把\varepsilon当终结符/非终结符处理，不要引出\varepsilon边。

恍然大悟，回去修改了下程序，果然能处理这类产生式了。

## `PL0`文法测试
后来有人给了一组数据，即`PL0`文法，测试失败。这里给下数据，做了点小修改，因为有些符号和我程序内部符号冲突了，所以只是做了简单的替换。
```cpp
A->B,
B->CEFH
B->H
B->CH
B->EH
B->FH
B->CFH
B->CEH
B->EFH
C->cY;
D->b=a
E->dX;
F->GB;
G->eb;
H->I
H->R
H->T
H->S
H->U
H->V
H->J
I->btL
J->fWg
K->LQL
K->hL
L->LOM
L->M
L->-M
L->+M
M->MPN
M->N
N->b
N->a
N->(L)
O->+
O->-
P->*
P->/
Q->=
Q->%
Q-><
Q->r
Q->>
Q->s
R->pKqH
S->mb
T->nKoH
U->i(X)
V->j(Z)
W->W;H
W->H
X->X,b
X->b
Y->Y,D
Y->D
Z->Z,L
Z->L
```

我调试了一下程序，发现2个比较严重的bug，有一个和程序无关紧要的bug，后面再说，这里先说下其中的一个bug，就是在求`first`集的bug。

如果文法产生式有直接左递归的话，那么就会死递归爆栈，所以我对`first`集做了下简单的修改，就是遇到直接左递归，忽略掉。

另一个`bug`也处理好了，`PL0`测试通过，近300个状态，这里贴一下局部预览图感受下。
![http://7xibui.com1.z0.glb.clouddn.com//2016/10/screenshot-window-2016-10-21-114458.png](http://7xibui.com1.z0.glb.clouddn.com//2016/10/screenshot-window-2016-10-21-114458.png)
![http://7xibui.com1.z0.glb.clouddn.com//2016/10/screenshot-window-2016-10-21-114315.png](http://7xibui.com1.z0.glb.clouddn.com//2016/10/screenshot-window-2016-10-21-114315.png)

## `for auto &` Bug
这个`bug`隐藏较深，就是我用`auto`引用类型引用容器中的元素，然后又在容器中插入元素，那么这个引用就会失效，换成迭代器也没达到预期效果，反而更糟，具体如下。

```cpp
#include <iostream>
#include <vector>
using namespace std;

int main() {
	vector<int> v={1, 2, 3};
	for(const auto &i: v) {
		printf("i=%d\n", i);
		v.push_back(5);
		printf("i=%d\n", i);
	}
	return 0;
}
```

只是简单地插入一个元素而已，输出结果却是这样的。
```cpp
i=1
i=0
i=0
i=0
i=3
i=3
```

`i=0`是哪来的。。。这个没找出原因，最后简单的用`for(int i=0; ...)`来替代处理了。

现在找到原因了，看了下[cppreference.com](http://en.cppreference.com/w/)关于`push_back`的定义，有这么一句话。

> If the new size() is greater than capacity() then all iterators and references (including the past-the-end iterator) are invalidated. Otherwise only the past-the-end iterator is invalidated.

就是说如果`vector`插入元素的`size()`大于`capacity()`的话，所有迭代器、引用都会无效，否则只是最后一个元素的迭代器/引用无效。这个可能和`vector`内存分配管理有关。

然后就是打印了下`vector`的大小，发现刚开始`size()==capacity()`的，所以一插入元素就会出问题，用`reserve()`简单的把`capacity()`调大就没问题了。

## 前端处理
核心程序写完了，最后就是把它展现出来，数据格式化下就好了。

之前在验收`LL1`实验的时候，老师看到我把语法树都画出来了，就说这个实验（`LL1`）没必要画语法树，`LR1`实验能把自动机画出来就好了，然后我就爽快答应了，因为只要核心程序写出来了，那么前端随便你怎么搞都行，这部分重点说下我是怎么画自动机的。

一开始我就是用`mermaid`这个`JS`库，只要给它图的描述，它就能画出来，试了下效果不是很满意。

继续`Google`，发现这么一个工具[graphviz](http://www.graphviz.org/)，它用了一种很简单的`DOT`语言，只要用`DOT`语言来描述这个图，它就能画出来。进一步发现这个工具有`js`移植版本，就试了试，效果不错，就是现在的样子。缺点就是这个移植版本太大了，`3.5MB`的库，所以第一次加载的时间全都花在下载这个库了，其二就是画那个`PL0`自动机的时候，会因为内存不足崩溃掉，可能这是`JS`的机制问题了吧。

考虑上面2个问题，我写了一个小程序`LR_DFA`，用它在后台直接输出`DOT`文件，然后交给`graphviz`处理导出`pdf`，能把完整的`PL0`自动机（画`PL0`比较耗时）画出来，就是前面贴出来的那个图了。

