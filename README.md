这是根据 craftingInterpreter 这个网站的教程，同时也大量参考了站上其他的同类项目而写的学习项目。
下面是一些相关的东西和学习与实现目标。

### 数据类型

* **布尔值**. bool

```kt
true;
false;
```

* **数字**. int / float

```kt
1234;  // int   (8 bytes)
12.34; // float (8 bytes)
```

* **字符串**. str

```kt
"I am a string";
"";     // 空字符串

/*
  * 字符串模板, "${...}"
  **  '...'的部分可以是多种表达式，但有以下例外：
  * 1. 嵌套： ${...${...}}
  * 2. '$'符号： ${...$...}
  * 3. 大括号： ${...{...}...}
  * 4. 字符串字面量: ${..."123"}
*/
let file = 1;
let copy = 2;

// load 1 file, save 2 copies.
let message = "load ${file} file, save ${copy} copies.";

// 如果想要保持‘${xxx}’原字符，可以在‘$’符号前使用‘\’反斜杠
let num = 100;
let s1 = "I have $${num + num}.";
let s2 = "I have $\${num}.";
print(s1);   // I have $200.
print(s2);   // I have $\${num}.
```

* **空值**. nil

```kt
nil;

/* 
  * 与以下类似：
  * None - python;
  * null - Java;
  * nullptr - C++;
*/
```

* **集合**. list / set / map

```kt
/*
  * 集合类型： 数组、集合、字典
  * 1. 集合中的元素可以随意 增、删、改、查
  * 2. 对元素的修改（增、改）需要保证其类型

  * - map是键值对集合，他的键固定为str类型，值的类型不限制
*/

// 声明时确定类型
var num_list: list[int];    // int类型的list
var float_set: set[float];  // float类型的set
var str_map: map[str];      // str类型的map

// 声明时初始化，此时不必要指定类型，会自动推断
var l = [1, 2, 2];           // int类型的list
var s = {4.2, 5.3, 6};       // float类型的set
var m = {"a": "chars", "b": "bytes"};    // str类型的map

// 声明时的类型推断取决于初始值的类型，如果是空集合就不行了
var empty = [];     // 会报错，虽然推断为list，但不知道元素类型
var ser_or_map = {}; // 会报错，此时甚至无法推断是set还是map
```

### 逻辑操作

```kt
// not
not true;   // false
not false;  // true
!true;      // false
!false;     // true

// and
true and false;     // false
true and true;      // true
true && false;      // false
true && true;       // true

// or
false or false;     // false
false or true;      // true
false || false;     // false
false || true;      // true
```

### 变量

* **var 可变变量**. 在声明后可以随意修改它的值。

```kt
var hello: str;     // 未初始化时，必须指定变量的类型
hello = "hello world!";

// 声明时初始化，此时不必要指定类型，会自动推断
var string = "here is my string";
string = "new string";
```

* **let 不可变量**. 在声明时就必须初始化，并且在之后就不能改变值。

```kt
let E;          // 会报错，必须立即初始化
E = 2.71828;

let PI = 3.14;  // 声明时初始化，此时不必要指定类型，会自动推断为float
PI = 3.33;      // 也会报错，不允许修改
```

---

### 控制语句

* **if**

```kt
if (cond1) {
    print("yes");

} elif (cond2) {
    print("or");

} else {
    print("no");
}
```

* **while**

```kt
var a = 1;
while (a < 10) {
    print(a);
    a = a + 1;
}
```

* **for**. 迭代循环，对集合类型迭代。

```kt
for (i in 5..1) {
    print(i);           // 从5输出到1
}

for (char in "abcde") { // 对字符串迭代，每次输出的是长度为1的字符串
    print(char);
}
```

* **when**. 类似switch语句，但更灵活。

```kt
// 每个分支不必 break;
when (x) {
    1 -> { /* aaa */  }     // 单个条件

    2, 3 -> { /* bbb */ }   // 多个条件

    in 4..8 -> { /* ccc */ }    // 范围检查(in, !in)

    is float -> { /* ddd */ }   // 类型检查(is, !is)
    
    else -> { /* eee */ }       // 其他情况(必要分支)
}

// 上面的语句与下面等效
if(x == 1) {
    // aaa

} elif(x == 2 || x == 3) {
    // bbb

} elif(x in 4..8) {
    // ccc

} elif(x is float) {
    // ddd

} else {
    // eee
}
```

### 函数

```kt
// 没有返回值时，默认返回nil
fun printMsg() {
    print("Hello, Idun!");
}
printMsg();

// 有参数和返回值时必须定义类型
fun sum(a: int, b: int): int {
    return a + b;
}
print("Sum: ${sum(5, 10)}");

/*
  * 参数默认值
  * 1.即便有默认值，参数类型也是必须的
  * 2.带默认值的参数必须在所有参数最后定义
*/
fun printMessage(hi: str, message: str = "Default!") {
    print(hi + ", " + message);
}
printMessage("Hello");           // 打印 ‘Hello, Default!’
printMessage("Hello", "Idun!");  // 打印 ‘Hello, Idun!’
```

### 类

```kt
/*
  * 类的声明
  * 1.括号中的参数是初始化构造器，所有的变量都被视为类的成员变量。
  * 2.初始化构造器可以没有,成员变量可以在类的内部常规定义。
  * 3.与函数参数写法类似，成员变量可以有默认参数。
*/
class Position(var x: float, var y: float) {
    fun move(dx: float, dy: float) {
        x += dx; y += dy;
    }

    fun posPrint() {
        print("(${x}, ${y})");
    }
}

// 类的继承
class Circle(var radius: float, var x: float, var y: float) : Position(x, y) {
    fun area(): float {
        return Math.PI * radius ** 2;  // πr²
    }

    fun distance(pos: Position): float {
        let dx = this.x - pos.x;   // this表示当前对象
        let dy = this.y - pos.y;
        return Math.sqrt(dx ** 2 + dy ** 2);
    }
}

let circle = Circle(5, 0, 0);   // r=5, p=(0, 0)

circle.move(3, 4);              // 改变的是当前对象的成员变量

circle.posPrint();              // (3, 4)

print("Circle area: ${circle.area()}"); // Circle area: 78.539816

print("Distance to parent: ${circle.distance(newPosition)}");
```

### Import. 导入模块或包

```kt
// 数学模块
import math;

print(math.PI);    // 3.1415926
print(math.sqrt(16));  // 4
```

### 关键字

|**break**|**class**|**continue**| **else** | **elif** |**enum**|
|:-------:|:-------:|:----------:|:--------:|:--------:|:------:|
|**false**| **fun** |  **for**   |  **if**  |**import**| **in** |
| **is**  | **let** |  **nil**   |**return**|**super** |**this**|
|**true** | **var** |  **when**  |**while** |||

### 操作符优先级

```kt
// & 和 | 运算符优先级高于 == （与C不同）
if (flags & FLAG_MASK == SOME_FLAG) { ... } // 先做按位与&操作再比较相等

```

### 标准库

```kt
...
```

### TODO

* [ ] 实现类型系统

* [ ] 实现各个集合类型

* [ ] 实现 `let` 不可变

* [ ] 实现 `enum` 语句

* [x] 实现 `break` 语句

* [ ] 实现 `import` 语句

* [x] 实现 `countine` 语句

* [ ] 处理 `字符串` 的 `转义字符`

* [ ] 实现 `throw` 和 `try-catch-finally` 语句

* [ ] 支持十六进制

* [x] 实现 `字符串模板`

* [ ] 忽略表达式语句末尾的 `;`

* [ ] 实现 `三元运算符` `x ? y : z;`

* [ ] 支持 `lambda` 形式的匿名函数

* [ ] 函数只能是顶级函数或是类的方法

* [ ] 禁止在运行时添加类的 `成员变量` 和 `方法`

* [ ] 实现上述所有功能和标准库，转换为静态类型编程语言
