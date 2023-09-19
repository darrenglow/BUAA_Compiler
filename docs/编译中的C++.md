# 编译中的C++

## 一些细节

- vector中最好放指针而不是对象

## 关于cmake

- `aux_source_directory`：将路径下的所有文件添加到变量中，在`add_executable`中可以使用

```cmake
# 添加源文件
aux_source_directory(./src DIR_SRC)
aux_source_directory(./src DIR_Lexer)

add_executable(BUAA_Compiler ${DIR_SRC} ${DIR_Lexer})
```

## 关于.h

如果想在多个文件中声明变量，如`a`。`b.cpp`想要使用`a.cpp`中的变量，就需要`extern int a`。如果变量多了的话，很麻烦。

头文件就是来集中声明一些变量 函数。

**头文件中的普通的变量只能是声明，有一些特殊的可以是定义**

- `const`

包含头文件的各个cpp文件中的`const`变量都是不同的，非`extern`的，作用域只在当前文件。因此可以在头文件中定义。

- 内联函数

还没接触到

- 类

一般类定义在头文件，类的函数声明在头文件，类函数的定义在cpp中

## 文件读写

如果只执行输入，使用ifstream类；

如果只执行输出，使用 ofstream类；

如果都要，使用fstream，可以指定std::ios_base::in或out

```c++
#include <fstream>
int main()
{
	std::fstream my_file;
    my_file.open("<name>", std::ios_base::in | std::ios_base::out);
}
```

## 重载运算符<<

重载运算符必须是`friend`的形式，因为如果作为成员函数的话，第一个参数是`this`，而`<<`的第一个参数必须是`ostream`

```c++
//类中
friend std::ostream &operator<<(std::ostream &ostream, const Token &token);
//外部
std::ostream &operator<<(std::ostream &ostream, const Token &token) {
    ostream << "<" << Token::list[token.token_type] << " " << token.content << ">";
    return ostream;
}
```

## static

- 可以隐藏。在文件a.cpp中声明的，在b.cpp中无法使用。
- 不依赖对象而存在。
- 默认初始化

**必须在类的外部定义**

```
double Accout::interestRate=0.09;
```

## 引用

```c++
class Lexer{
private:
    std::vector<Token> tokens;
    std::ifstream& input;
public:
    Lexer(std::ifstream& input_) : input(input_) {}
};
```

这里的input如果不是&就会报错，某种奇怪的原因，貌似是啥拷贝构造之类的。使用引用的方式就行。

## const

```c++
std::vector<Token> getTokens() const { return this -> tokens; }
```

表示不改变成员变量的函数

## str.find()

```c++
string str = "I am a";
size_t index = str.find('a');	//找得到的话返回index，找不到的话返回string::npos
```

![image-20230914214649058](C:\Users\66418\AppData\Roaming\Typora\typora-user-images\image-20230914214649058.png)

很奇怪的错误，de了我很久：当find找不到时，我直接用`iter->second`，他始终会得到一个值，我百思不得其解，估计是随机初始化了？反正一定不要直接用，使用if判断，或者使用三元运算符计算一下。

## string 2 int

```cpp
atoi(str.c_str())
```

