#include "widget.h"
#include "ui_widget.h"
#include <QTextStream>
#include <QDebug>
#include <QString>
#include <map>
#include <vector>
#include <stack>
#include <unordered_map>
#include <queue>
#include <set>
#include <unordered_set>
#include <algorithm>
#include <string>
#include <sstream>
#include <fstream>
#include <QFile>
#include <QFileDialog>
#include <QTextCodec>
#include <QMessageBox>
#include <iostream>
#pragma execution_character_set("utf-8")
using namespace std;


Widget::Widget(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
}

Widget::~Widget()
{
    delete ui;
}


// nfa图的结点数量，用于为nfa结点附上id值
int nodeCount = 0;

// 空字符串用“#”表示
const char EPSILON = '#';

// 全局字符统计
set<char> nfaCharSet;
set<char> dfaCharSet;

// 赋值映射 将命名语句中的变量名映射成正则表达式
unordered_map<string, string> name;

// 将多行正则表达式合并成一行
/*
    line1
    line2    =>    (line1)|(line2)|(line3)
    line3
*/
QString merge(QString regex)
{
    string stdRegex = regex.toStdString();
    vector<string> lines;
    vector<string> resVec;
    istringstream iss(stdRegex);
    string line;
    string output;

    while (std::getline(iss, line)) {
        if (!line.empty()) {
            lines.push_back(line);
        }
    }

    for (int i = 0; i < lines.size(); ++i) {
        size_t assignPos = lines[i].find('=');
        // 命名
        if(assignPos != string::npos && lines[i][0] != '_'){
            string variable = lines[i].substr(0, assignPos);
            string value = lines[i].substr(assignPos + 1);
            name[variable] = value;
        }
        // 将等号右边的正则表达式分离出来
        else if(assignPos != string::npos && lines[i][0] == '_'){
            string exp = lines[i].substr(assignPos + 1);
            cout << exp << endl;
            string result = exp;

            // 遍历映射表的键值对，替换字符串中的对应子串
            for (auto& mapping : name) {
                size_t pos = 0;
                string replacement = "(" + mapping.second + ")";       // 为每个映射值添加括号
                // 查找并替换所有匹配的键
                while ((pos = result.find(mapping.first, pos)) != std::string::npos) {
                    result.replace(pos, mapping.first.length(), replacement);
                    pos += mapping.second.length(); // 跳过替换后的部分，防止重复替换
                }
            }
            cout << result << endl;
            resVec.push_back(result);
        }
    }

    for(size_t i = 0; i < resVec.size(); i++){
        output += resVec[i];
        cout << "test" << endl;
        if(i < resVec.size() - 1){
            output += "|";
        }
    }

//    for(auto &it : assign){
//        cout << it.first << it.second << endl;
//    }
    return QString::fromStdString(output);
}



// 将set转为string，将set中结果在表格中展示
string setTostring(set<int> s)
{
    string result;

    for (int i : s) {
        result.append(to_string(i));
        result.append(",");
    }

    if (result.size() != 0)
        result.pop_back();

    return result;
}


bool basicChar(char c)
{
    if ((c >= 'a' && c <= 'z') ||
        (c >= 'A' && c <= 'Z') ||
        (c >= '0' && c <= '9') ||
        c == '!' || c == '@' || c == '+' ||
        c == '$' || c == '%' || c == '^' ||
        c == '&' || c == ',' || c == '=' ||
        c == '>' || c == '<' || c == '/')
        return true;
    return false;
}


// 预处理正则表达式
QString preprocess(QString regex)
{
    string stdRegex = regex.toStdString();

    // 处理中括号  [ABC] -> (A|B|C) [a-z] -> (a|b|c...|y|z)
    string result;
    bool isContent = false;     // 中括号内容范围标志
    string currentString;

    for (int i = 0; i < stdRegex[i]; i++) {
        if (stdRegex[i] == '[') {
            isContent = true;
            currentString.push_back('(');
        }
        else if (stdRegex[i] == ']') {
            isContent = false;
            currentString.push_back(')');
            result += currentString;
            currentString.clear();
        }
        else if (isContent) {
            // if条件以防出现 [ab] -> (|a|b)
            if (currentString.length() > 1) {
                currentString.push_back('|');
            }
            currentString.push_back(stdRegex[i]);
            if (stdRegex[i] == '-') {
                // 处理[a-z]
                if (islower(stdRegex[i - 1])) {
                    currentString.pop_back();       // 弹出'-'
                    for (int j = 1; j < stdRegex[i + 1] - 'a'; j++) {
                        currentString.push_back(stdRegex[i - 1] + j);
                        if (j != stdRegex[i + 1] - 'a' - 1) {
                            currentString.push_back('|');
                        }
                    }
                }
                // 处理[A-Z]
                else if (isupper(stdRegex[i - 1])) {
                    currentString.pop_back();       // 弹出'-'
                    for (int j = 1; j < stdRegex[i + 1] - 'A'; j++) {
                        currentString.push_back(stdRegex[i - 1] + j);
                        if (j != stdRegex[i + 1] - 'A' - 1) {
                            currentString.push_back('|');
                        }
                    }
                }
                // 处理[0-9]
                else if (isdigit(stdRegex[i - 1])) {
                    currentString.pop_back();       // 弹出'-'
                    for (int j = 1; j < stdRegex[i + 1] - '0'; j++) {
                        currentString.push_back(stdRegex[i - 1] + j);
                        if (j != stdRegex[i + 1] - '0' - 1) {
                            currentString.push_back('|');
                        }
                    }
                }
            }
        }
        // 正则表达式没有中括号的情况
        else {
            result.push_back(stdRegex[i]);
        }
    }

    stdRegex = result;

    // 处理正闭包   (ab)+cd+ -> (ab)(ab)*cdd*
    for (int i = 0; i < stdRegex.size(); i++)
    {
        if (stdRegex[i] == '+' && stdRegex[i - 1] != '\\')
        {
            int bracketPairs = 0;     // 通过自减和自增匹配“(”和“)”
            int j = i;
            do
            {
                j--;
                if (stdRegex[j] == ')')
                {
                    bracketPairs++;
                }
                else if (stdRegex[j] == '(')
                {
                    bracketPairs--;
                }
            } while (bracketPairs != 0);

            string str1 = stdRegex.substr(0, j);            // 受正闭包约束的字符串的前一部分字符串
            string str2 = stdRegex.substr(j, i - j);        // 受正闭包“+”约束的字符串
            string str3 = stdRegex.substr(i + 1, (stdRegex.size() - i));        // 受正闭包约束的字符串的后一部分字符串
            stdRegex = str1 + str2 + str2 + "*" + str3;
        }
        // 处理\+
        else if(stdRegex[i] == '+' && stdRegex[i - 1] == '\\'){
            stdRegex.replace(i - 1, 1, "");
        }
    }


//    int atPos = 0; // 记录“.”被替换成“@”在字符串中的位置
    // 处理转义字符
    for(size_t i = 1; i < stdRegex.size(); i++){
        // 处理\*
        if(stdRegex[i] == '*' && stdRegex[i - 1] == '\\'){
            stdRegex.replace(i - 1, 1, ".");
        }
        // 处理\?
        if(stdRegex[i] == '?' && stdRegex[i - 1] == '\\'){
            stdRegex.replace(i - 1, 1, ".");
        }
        // 处理\|
//        if(regexStd[i] == '|' && regexStd[i - 1] == '\\'){
//            atPos = i;
//            regexStd.replace(i, 1, "@");
//            regexStd.replace(i - 1, 1, ".");
//        }
    }



    for (int i = 0; i < stdRegex.size() - 1; i++)
    {
        if (basicChar(stdRegex[i]) && basicChar(stdRegex[i + 1])              // 连接两个操作数 ab -> a.b
            || basicChar(stdRegex[i]) && stdRegex[i + 1] == '('            // 连接操作数与括号 a(xxx) -> a.(xxx)
            || stdRegex[i] == ')' && basicChar(stdRegex[i + 1])            // 连接操作数与括号 (xxx)a -> (xxx).a
            || stdRegex[i] == ')' && stdRegex[i + 1] == '('             // 连接两个括号 (xxx)(xxx) -> (xxx).(xxx)
            || stdRegex[i] == '*' && stdRegex[i + 1] != ')' && stdRegex[i + 1] != '|' && stdRegex[i + 1] != '?' && stdRegex[i + 1] != '*'     // (a.b)*c -> (a.b)*.c
            || stdRegex[i] == '?' && stdRegex[i + 1] != ')' && stdRegex[i + 1] != '|' && stdRegex[i + 1] != '?' && stdRegex[i + 1] != '*'
            )
        {
            string str1 = stdRegex.substr(0, i + 1);
            string str2 = stdRegex.substr(i + 1, (stdRegex.size() - i));
            str1 += ".";
            stdRegex = str1 + str2;
        }
    }
    // 将被替换成“@”的“|”还原
//    for(int i = 0; i < regexStd.length(); i++){
//        if(regexStd[i] == '@' && i == atPos){
//            regexStd.replace(i, 1, "|");
//        }
//    }
    cout << "regexStd: " << stdRegex << endl;
    return QString::fromStdString(stdRegex);
}




// 正则转NFA


struct nfaNode;

// NFA图的边
struct nfaEdge
{
    char c;
    nfaNode* next;
};

// NFA图的结点
struct nfaNode
{
    vector<nfaEdge> edges;  // 一个结点可从vector中选择某条边转移状态
    int id; // 结点唯一编号
    bool isStart;   // 初态标识
    bool isEnd; // 终态标识
    nfaNode() {
        id = nodeCount++;
        isStart = false;
        isEnd = false;
    }
};

// NFA图
struct NFA
{
    // 只需起始结点和终止结点就能标识一个NFA图
    nfaNode* start;
    nfaNode* end;
    NFA() {}
    NFA(nfaNode* s, nfaNode* e)
    {
        start = s;
        end = e;
    }
};

// 基本字符NFA
NFA basicCharNFA(char character) {
    nfaNode* start = new nfaNode();
    nfaNode* end = new nfaNode();

    start->isStart = true;
    end->isEnd = true;

    nfaEdge edge;
    edge.c = character;
    edge.next = end;
    start->edges.push_back(edge);

    NFA nfa(start, end);

    nfaCharSet.insert(character);
    dfaCharSet.insert(character);

    return nfa;
}

// 连接运算符的NFA图
NFA concatNFA(NFA nfa1, NFA nfa2) {
    // 把nfa1的终止状态与nfa2的起始状态连接起来
    nfa1.end->isEnd = false;
    nfa2.start->isStart = false;

    nfaEdge edge;
    edge.c = EPSILON;
    edge.next = nfa2.start;
    nfa1.end->edges.push_back(edge);

    NFA nfa;
    nfa.start = nfa1.start;
    nfa.end = nfa2.end;

    return nfa;
}

// 选择运算符的NFA图
NFA orNFA(NFA nfa1, NFA nfa2) {
    nfaNode* start = new nfaNode();
    nfaNode* end = new nfaNode();

    start->isStart = true;
    end->isEnd = true;

    // 把新的初态与nfa1和nfa2的初态连接起来
    nfaEdge edge1;
    edge1.c = EPSILON;
    edge1.next = nfa1.start;
    start->edges.push_back(edge1);
    nfa1.start->isStart = false;    // 将nfa1的原初态修改为普通状态

    nfaEdge edge2;
    edge2.c = EPSILON;
    edge2.next = nfa2.start;
    start->edges.push_back(edge2);
    nfa2.start->isStart = false;    // 将nfa2的原初态修改为普通状态

    // 把nfa1和nfa2的终止状态与新的终止状态连接起来
    nfa1.end->isEnd = false;
    nfa2.end->isEnd = false;

    nfaEdge edge3;
    edge3.c = EPSILON;
    edge3.next = end;
    nfa1.end->edges.push_back(edge3);

    nfaEdge edge4;
    edge4.c = EPSILON;
    edge4.next = end;
    nfa2.end->edges.push_back(edge4);

    NFA nfa(start, end);

    return nfa;
}

// 闭包运算符的NFA图
NFA closureNFA(NFA nfa1) {
    nfaNode* start = new nfaNode();
    nfaNode* end = new nfaNode();

    start->isStart = true;
    end->isEnd = true;

    // 令nfa1的新初态指向nfa1的原初态
    nfaEdge edge1;
    edge1.c = EPSILON;
    edge1.next = nfa1.start;
    start->edges.push_back(edge1);
    nfa1.start->isStart = false;    // 将原初态修改为普通状态

    // 令nfa1的新初态指向nfa1的新终态
    nfaEdge edge2;
    edge2.c = EPSILON;
    edge2.next = end;
    start->edges.push_back(edge2);

    // 令nfa1的原终止状态指向nfa1的原初始状态
    nfa1.end->isEnd = false;

    nfaEdge edge3;
    edge3.c = EPSILON;
    edge3.next = nfa1.start;
    nfa1.end->edges.push_back(edge3);

    // 令nfa1的原终态指向nfa1的新终态
    nfaEdge edge4;
    edge4.c = EPSILON;
    edge4.next = end;
    nfa1.end->edges.push_back(edge4);

    NFA nfa(start,end);

    return nfa;
}

// 可选运算符的NFA图
// ?的NFA图其实就是无需自环的闭包NFA图
NFA optionalNFA(NFA nfa1) {
    nfaNode* start = new nfaNode();
    nfaNode* end = new nfaNode();

    start->isStart = true;
    end->isEnd = true;

    // 令nfa1的新初态指向nfa1原初态
    nfaEdge edge1;
    edge1.c = EPSILON;
    edge1.next = nfa1.start;
    start->edges.push_back(edge1);
    nfa1.start->isStart = false;    // 初态结束

    // 令nfa1的新初态指向nfa1新终态
    nfaEdge edge2;
    edge2.c = EPSILON;
    edge2.next = end;
    start->edges.push_back(edge2);

    // 令nfa1的原终态指向nfa1原终态
    nfa1.end->isEnd = false;

    nfaEdge edge3;
    edge3.c = EPSILON;
    edge3.next = end;
    nfa1.end->edges.push_back(edge3);

    NFA nfa(start, end);

    return nfa;
}

// 优先级判断
int priority(char op) {
    if (op == '|') {
        return 1;  // 选择运算符 "|" 的优先级
    }
    else if (op == '.') {
        return 2;  // 连接运算符 "." 的优先级
    }
    else if (op == '*' || op == '?') {
        return 3;  // 闭包运算符 "*"和 "?" 的优先级
    }
    else {
        return 0;  // 默认情况下，将其它字符的优先级设为0
    }
}

struct nfaStatusNode
{
    string flag;  // 标记初态还是终态
    int id; // 唯一id值  nfa状态结点的id对应的是nfa结点的id
    map<char, set<int>> transition;  // 对应字符能到达的结点集合
    nfaStatusNode()
    {
        flag = "";
    }
};

// 状态转换表
// nfa结点id映射至nfa状态结点
unordered_map<int, nfaStatusNode> statusTable;    // nfaStatusNode类型的数组
// insertionOrder记录插入statusTable的顺序
vector<int> insertionOrder;
set<int> startNFAstatus;
set<int> endNFAstatus;

// 对NFA图进行DFS的过程中，形成状态转换表

void formNFAStatus(NFA& nfa)
{
    stack<nfaNode*> nfaStack;
    set<nfaNode*> visitedNodes;

    // 初态
    // 从初态出发
    nfaNode* startNode = nfa.start;
    nfaStatusNode startStatusNode;
    startStatusNode.flag = '-'; // -表示初态
    startStatusNode.id = startNode->id;
    statusTable[startNode->id] = startStatusNode;
    insertionOrder.push_back(startNode->id);
    startNFAstatus.insert(startNode->id);

    nfaStack.push(startNode);

    while (!nfaStack.empty()) {
        nfaNode* currentNode = nfaStack.top();
        nfaStack.pop();
        visitedNodes.insert(currentNode);

        for (nfaEdge edge : currentNode->edges) {
            char transitionChar = edge.c;
            nfaNode* nextNode = edge.next;

            // 记录状态转换信息 当前状态结点经过transitionChar能到达它对应的结点指向的下一个结点，将这个“下一个结点”的id记录在当前状态结点的状态转换集合（set<int>）中
            statusTable[currentNode->id].transition[transitionChar].insert(nextNode->id);

            // 如果下一个状态未被访问，将其加入堆栈
            if (visitedNodes.find(nextNode) == visitedNodes.end()) {
                nfaStack.push(nextNode);

                // 记录状态信息
                // 为记录状态转换信息（statusTable[currentNode->id]）做铺垫
                nfaStatusNode nextStatus;
                nextStatus.id = nextNode->id;
                if (nextNode->isStart) {
                    nextStatus.flag += '-'; // -表示初态
                    startNFAstatus.insert(nextStatus.id);
                }
                else if (nextNode->isEnd) {
                    nextStatus.flag += '+'; // +表示终态
                    endNFAstatus.insert(nextStatus.id);
                }
                statusTable[nextNode->id] = nextStatus;
                // 确保终态在插入顺序表的最后一位
                if (!nextNode->isEnd)
                {
                    insertionOrder.push_back(nextNode->id);
                }
            }
        }
    }

    // 确保终态在插入顺序表的最后一位
    nfaNode* endNode = nfa.end;
    insertionOrder.push_back(endNode->id);
}


// 正则表达式转NFA
NFA regexToNFA(string regex)
{
    // 双栈法，创建两个栈opStack（运算符栈）,nfaStack（nfa图栈）
    stack<char> opStack;
    stack<NFA> nfaStack;

    int i = -1;
    // 对表达式进行后缀表达式处理
    // 运算符：| .（） *
    // 由于闭包*、?是单目运算符，闭包运算符与其前一个“表达式”为一个整体，与连接和或运算符不同，因此闭包运算符无需入栈
    for (char c : regex)
    {
        switch (c)
        {
        case ' ':
            i++;
            cout << "i1 = " << i << endl;
            break;
        case '(':
            i++;
            cout << "i2 = " << i << endl;
            opStack.push(c);
            break;
        case ')':
            // 扫描正则表达式，遇到")"则依次弹出栈内运算符，直到弹出一个“(”为止。
            i++;
            cout << "i3 = " << i << endl;
            while (!opStack.empty() && opStack.top() != '(')
            {
                // 处理栈顶运算符，构建NFA图，并将结果入栈
                char op = opStack.top();
                opStack.pop();

                if (op == '|') {
                    // 处理并构建"|"运算符
                    NFA nfa2 = nfaStack.top();      // “右”操作数
                    nfaStack.pop();
                    NFA nfa1 = nfaStack.top();      // “左”操作数
                    nfaStack.pop();

                    // 创建新的NFA，表示nfa1 | nfa2
                    NFA resultNFA = orNFA(nfa1, nfa2);
                    nfaStack.push(resultNFA);
                }
                else if (op == '.') {
                    // 处理并构建"."运算符
                    NFA nfa2 = nfaStack.top();      // “右”操作数
                    nfaStack.pop();
                    NFA nfa1 = nfaStack.top();      // “左”操作数
                    nfaStack.pop();

                    // 创建新的NFA，表示nfa1 . nfa2
                    NFA resultNFA = concatNFA(nfa1, nfa2);
                    nfaStack.push(resultNFA);
                }
            }
            if (opStack.empty())
            {
                qDebug() << "括号未闭合！";
                exit(-1);
            }
            else
            {
                opStack.pop(); // 弹出(
            }
            break;
        case '|':
        case '.':
            // 处理运算符 | 和 .
            i++;
            cout << "i4 = " << i << endl;
            // _test=\*|\?|\||\+|a|>=
//            if(i - 1 == 0 && regex[i - 1] == '.' || (regex[i - 2] == '|' && regex[i - 1] == '.')){
//                opStack.pop();      // 弹出“.”
//                NFA nfa = basicCharNFA(c); // 创建基本字符 \| NFA
//                nfaStack.push(nfa);
//            }
//            if(regex[i] == '|' && regex[i - 1] == '.'){
//                cout << "! _test=\+|\.|\* !" << endl;
//                NFA nfa = basicCharNFA(c); // 创建基本的字符 \| 的NFA
//                nfaStack.push(nfa);
//            }
//            else{
//            }
            while (!opStack.empty() && (opStack.top() == '|' || opStack.top() == '.') &&
                priority(opStack.top())>= priority(c))
            // 遇到运算符，依次弹出栈中优先级高于或等于当前扫描到的运算符的所有运算符，与“操作数”（即nfa图）运算
            // 然后将当前运算符压入栈中
            {
                char op = opStack.top();
                opStack.pop();

                // 处理栈顶运算符，构建NFA图，并将结果入栈
                if (op == '|') {
                    // 处理并构建"|"运算符
                    NFA nfa2 = nfaStack.top();
                    nfaStack.pop();
                    NFA nfa1 = nfaStack.top();
                    nfaStack.pop();

                    // 创建新的NFA，表示nfa1 | nfa2
                    NFA resultNFA = orNFA(nfa1, nfa2);
                    nfaStack.push(resultNFA);
                }
                else if (op == '.') {
                    // 处理并构建"."运算符
                    NFA nfa2 = nfaStack.top();
                    nfaStack.pop();
                    NFA nfa1 = nfaStack.top();
                    nfaStack.pop();

                    // 创建新的 NFA，表示 nfa1 . nfa2
                    NFA resultNFA = concatNFA(nfa1, nfa2);
                    nfaStack.push(resultNFA);
                }
            }
            opStack.push(c);
            break;
        case '?':
        case '*':
            i++;
            cout << "i5 = " << i << endl;
            // 处理闭包运算符 ? *
            // 从nfaStack弹出NFA，应用相应的闭包操作，并将结果入栈
            if (!nfaStack.empty() || regex[i - 1] == '.') {
                if (c == '?') {
                    // 处理前面没有基本字符的“\?”
                    if(i - 1 == 0 && regex[i - 1] == '.' || regex[i - 2] == '|'){
                        cout << "opStack.top()=" << opStack.top() << endl;
                        opStack.pop();      // 弹出“.”
                        NFA nfa = basicCharNFA(c); // 创建基本字符 \? NFA
                        nfaStack.push(nfa);
                        // _test=\?
                    }
                    // 处理 a\?
                    else if(regex[i - 1] == '.'){
                        cout << "here?" << endl;
                        // _test=a\*
                        NFA nfa = basicCharNFA(c); // 创建基本字符 \? NFA
                        nfaStack.push(nfa);
                        cout << "nfaStack.size()" << nfaStack.size() << endl;
                    }
                    // 处理 ?
                    else{
                        NFA nfa = nfaStack.top();
                        nfaStack.pop();
                        NFA resultNFA = optionalNFA(nfa);
                        nfaStack.push(resultNFA);
                    }
                }
                else if (c == '*') {
                    // 处理前面没有基本字符的“\*”
                    if(i - 1 == 0 && regex[i - 1] == '.' || regex[i - 2] == '|'){
                        cout << "opStack.top()=" << opStack.top() << endl;
                        opStack.pop();      // 弹出“.”
                        NFA nfa = basicCharNFA(c); // 创建基本的字符NFA
                        nfaStack.push(nfa);
                        // _test=\*
                    }
                    // 处理 a\*
                    else if(regex[i - 1] == '.'){
                        cout << "here*" << endl;
                        // _test=a\*
                        NFA nfa = basicCharNFA(c); // 创建基本的字符NFA
                        nfaStack.push(nfa);
                        cout << "nfaStack.size()" << nfaStack.size() << endl;
                    }
                    // 处理*
                    else{
                        NFA nfa = nfaStack.top();
                        nfaStack.pop();
                        NFA resultNFA = closureNFA(nfa);
                        nfaStack.push(resultNFA);
                    }
                }
            }
            else {
                qDebug() << "错误1";
                exit(-1);
            }
            break;
        default:
            i++;
            cout << "i6 = " << i << endl;
            // 处理其他字符
            NFA nfa = basicCharNFA(c); // 创建基本的字符NFA
            nfaStack.push(nfa);
            cout << "nfaStack.size()" << nfaStack.size() << endl;
            break;
        }

    }

    // 处理栈中剩余的运算符
    while (!opStack.empty())
    {
        char op = opStack.top();
        opStack.pop();

        if (op == '|' || op == '.')
        {
            // 处理并构建运算符 | 和 .
            if (nfaStack.size() < 2)
            {
                qDebug() << "错误2";
                exit(-1);
            }

            NFA nfa2 = nfaStack.top();
            nfaStack.pop();
            NFA nfa1 = nfaStack.top();
            nfaStack.pop();

            if (op == '|')
            {
                // 创建新的 NFA，表示 nfa1 | nfa2
                NFA resultNFA = orNFA(nfa1, nfa2);
                nfaStack.push(resultNFA);
            }
            else if (op == '.')
            {
                // 创建新的 NFA，表示 nfa1 . nfa2
                NFA resultNFA = concatNFA(nfa1, nfa2);
                nfaStack.push(resultNFA);
            }
        }
        else
        {
            qDebug() << "错误3";
            exit(-1);
        }
    }

    // 最终的NFA图在nfaStack的顶部
    NFA result = nfaStack.top();

    formNFAStatus(result);

    return result;
}

// NFA转DFA

// dfa节点
struct dfaNode
{
    string flag; // 是否包含终态（+）或初态（-）
    set<int> nfaStates; // 该DFA状态包含的NFA状态的集合
    map<char, set<int>> transitions; // 字符到下一状态的映射
    dfaNode() {
        flag = "";
    }
};
// dfa状态去重集
set<set<int>> dfaStatusSet;

// dfa最终结果
vector<dfaNode> dfaTable;

//下面用于DFA最小化
// dfa有且仅有一个初态，因此不需要初态集合
// dfa终态集合
set<int> dfaEndStatusSet;
// dfa非终态集合
set<int> dfaNotEndStatusSet;
// set对应序号MAP
map<set<int>, int> DFAToID;
int startStaus;

// 判断是否含有初态终态，含有则返回对应字符串
// 查看DFA的状态集合是否包含NFA中的初态或终态
// 若包含初态，则当前DFA状态为初态（-）；若包含终态，则当前DFA状态为终态（+）；
// 若都包含，则当前DFA状态同时为初态和终态（-+）；若都不包含，则当前DFA状态为普通状态（空字符串）
string start_end(set<int>& statusSet)
{
    string result = "";
    for (const int& element : startNFAstatus) {
        if (statusSet.count(element) > 0) {
            result += "-";
        }
    }

    for (const int& element : endNFAstatus) {
        if (statusSet.count(element) > 0) {
            result += "+";
        }
    }

    return result;
}

set<int> epsilonClosure(int id)
{
    set<int> eResult{ id };     // 结点本身也属于epsilon闭包
    stack<int> stack;
    stack.push(id);

    while (!stack.empty())
    {
        int current = stack.top();
        stack.pop();

        set<int> eClosure = statusTable[current].transition[EPSILON];    // nfa中current结点的id仅通过一次epsilon能够到达的结点的id集合
        for (auto t : eClosure)
        {
            // 防止遇到值全是epsilon的边组成的圈而导致陷入死循环
            if (eResult.find(t) == eResult.end())
            {
                eResult.insert(t);
                stack.push(t);          // 通过epsilon到达当前结点后，还需查看当前结点是否能通过epsilon到达其它结点
            }
        }
    }

    return eResult;     // epsilon闭包
}

// 转换闭包
set<int> transitionClosure(int source, char ch)
{
    set<int> result{};

    set<int> chClosure = statusTable[source].transition[ch];
    for (auto o : chClosure)
    {
        result.insert(o);
        auto tmp = epsilonClosure(o);
        result.insert(tmp.begin(), tmp.end());     // 当前结点的字符闭包就是当前结点通过字符指向的下一个结点加上“下一个结点”的esilon闭包
    }

    return result;
}


void NFAToDFA(NFA& nfa)
{
    // 获取DFA的起始结点（nfa初态的epsilon闭包） 将DFA的起始节点（set类型）映射成整型编号（如{1, 2, 6}用1代替） 判断起始节点的状态
    int dfaStatusCount = 1;
    auto start = nfa.start; // 获得NFA图的起始位置
    auto startId = start->id;   // 获得起始编号
    dfaNode startDFANode;
    startDFANode.nfaStates = epsilonClosure(startId); // 初始闭包
    startDFANode.flag = start_end(startDFANode.nfaStates); // 判断初态终态
    deque<set<int>> newStatus{};
    DFAToID[startDFANode.nfaStates] = dfaStatusCount;
    startStaus = dfaStatusCount;
    // 查看startDFANode.flag是否包含"+" (查看startDFANode是否是终态)
    if (start_end(startDFANode.nfaStates).find("+") != string::npos) {
        // 若包含（若是）
        dfaEndStatusSet.insert(dfaStatusCount++);
    }
    else
    {
        // 若不包含（若不是）
        dfaNotEndStatusSet.insert(dfaStatusCount++);
    }
    // 对dfa初始状态中的每个nfa状态进行对应字符的状态转换
    // {1, 2, 6}_a -> {3, 4, 7, 8}   {1, 2, 6}_b -> error
    for (auto ch : dfaCharSet)
    {
        set<int> currentChClosure{};
        for (auto c : startDFANode.nfaStates)
        {
            set<int> tmp = transitionClosure(c, ch);
            currentChClosure.insert(tmp.begin(), tmp.end());   // 状态集合中各个字符的转移闭包
        }
        if (currentChClosure.empty())  // 如果这个闭包是空集没必要继续下去了 因此可能会有些字符不会记录在transitions中
        {
            continue;
        }
        // 假设{1, 2, 6}_a -> {3, 4, 7, 8}   {1, 2, 6}_b -> {3, 4, 7, 8} 即{1， 2， 6}对a和b都转移到统一集合
        // 遍历到b字符时无需再对{3, 4, 7, 8}进行状态标识，也无需再将它映射成整型编号，也无需再将其压入队列中进行后序遍历，因为这些都在遍历到a字符时完成
        int presize = dfaStatusSet.size();
        dfaStatusSet.insert(currentChClosure);
        int postsize = dfaStatusSet.size();
        // 无论是否一样都是该节点这个字符的状态
        startDFANode.transitions[ch] = currentChClosure;
        // 如果大小不一样，说明集合并未去重，新添的集合元素与原有的元素不重复，证明是新状态
        if (postsize > presize)
        {
            DFAToID[currentChClosure] = dfaStatusCount;
            newStatus.push_back(currentChClosure);
            if (start_end(currentChClosure).find("+") != string::npos) {
                dfaEndStatusSet.insert(dfaStatusCount++);
            }
            else
            {
                dfaNotEndStatusSet.insert(dfaStatusCount++);
            }

        }

    }
    dfaTable.push_back(startDFANode);

    // 对后面的dfa新状态进行不停遍历
    while (!newStatus.empty())
    {
        // 拿出一个新状态
        set<int> newStat = newStatus.front();
        newStatus.pop_front();
        dfaNode DFANode;
        DFANode.nfaStates = newStat;  // 该节点状态集合
        DFANode.flag = start_end(newStat);

        for (auto ch : dfaCharSet)
        {

            set<int> currentChClosure{};
            for (auto c : newStat)
            {
                set<int> tmp = transitionClosure(c, ch);
                currentChClosure.insert(tmp.begin(), tmp.end());
            }
            if (currentChClosure.empty())  // 如果这个闭包是空集没必要继续下去了
            {
                continue;
            }
            int presize = dfaStatusSet.size();
            dfaStatusSet.insert(currentChClosure);
            int lastsize = dfaStatusSet.size();
            // 无论是否一样都是该节点这个字符的状态
            DFANode.transitions[ch] = currentChClosure;
            // 如果大小不一样，说明集合并未去重，新添的集合元素与原有的元素不重复，证明是新状态
            if (lastsize > presize)
            {
                DFAToID[currentChClosure] = dfaStatusCount;
                newStatus.push_back(currentChClosure);
                if (start_end(currentChClosure).find("+") != string::npos) {
                    dfaEndStatusSet.insert(dfaStatusCount++);
                }
                else
                {
                    dfaNotEndStatusSet.insert(dfaStatusCount++);
                }

            }

        }
        dfaTable.push_back(DFANode);

    }

}

// DFA最小化

// 判断是否含有初态终态，含有则返回对应字符串
string min_set_end(set<int>& statusSet)
{
    string result = "";
    if (statusSet.count(startStaus) > 0) {
        result += "-";
    }

    for (const int& element : dfaEndStatusSet) {
        if (statusSet.count(element) > 0) {
            result += "+";
            break;  // 若包含多个终态也只需要一个加号
        }
    }

    return result;
}

// dfa最小化节点
struct dfaMinNode
{
    string flag; // 是否包含终态（+）或初态（-）
    int id;
    map<char, int> transitions; // 字符到下一状态的映射
    dfaMinNode() {
        flag = "";
    }
};

vector<dfaMinNode> dfaMinVector;

// 用于分割集合
vector<set<int>> dividedSetVec;

map<int, int> dfaSameMinSet;    // DFA的dfaStatusCount只要被划分到同一个集合就会被映射成同样的值，值依赖于dividedSetVec大小

// 根据字符 ch 将状态集合 node 分成两个子集合
void divideSet(int i, char ch)
{
    set<int> result;
    auto& node = dividedSetVec[i];   // node是dfaStatusCount，而dfaStatusCount从1开始
    int s = -2;

    for (auto state : node)
    {
        int sameSetMark;
        // 当前DFA结点无法通过ch转移状态
        if (dfaTable[state - 1].transitions.find(ch) == dfaTable[state - 1].transitions.end())
        {
            sameSetMark = -1;
        }
        else
        {
            // 根据字符 ch 找到下一个状态
            int next_state = DFAToID[dfaTable[state - 1].transitions[ch]];//dfaStatusCount
            sameSetMark = dfaSameMinSet[next_state];    //
        }

        if (s == -2)    // 初始下标
        {
            s = sameSetMark;
        }
        else if (sameSetMark != s)   // 如果下标不同，就是有问题，需要分出来
        {
            result.insert(state);
        }
    }

    // 删除要删除的元素
    for (int state : result) {
        node.erase(state);
    }

    // 都遍历完了，如果result不是空，证明有新的，加入vector中
    if (!result.empty())
    {
        dividedSetVec.push_back(result);
        // 同时更新映射值
        for (auto a : result)
        {
            dfaSameMinSet[a] = dividedSetVec.size() - 1;
        }
    }

}

void DFAminimize()
{
    dividedSetVec.clear();
    dfaSameMinSet.clear();

    // 存入非终态集合
    if (dfaNotEndStatusSet.size() != 0)
    {
        dividedSetVec.push_back(dfaNotEndStatusSet);
    }
    // 初始化map
    for (auto t : dfaNotEndStatusSet)
    {
        dfaSameMinSet[t] = dividedSetVec.size() - 1;     // 0 或 -1
    }

    // 存入终态集合
    dividedSetVec.push_back(dfaEndStatusSet);

    // 初始化map
    for (auto t : dfaEndStatusSet)
    {
        dfaSameMinSet[t] = dividedSetVec.size() - 1;     // 1 或 0
    }

    // 当flag为1时，一直循环
    int continueFlag = 1;

    while (continueFlag)
    {
        continueFlag = 0;
        int size1 = dividedSetVec.size();

        for (int i = 0; i < size1; i++)
        {

            // 逐个字符尝试分割状态集合
            for (char ch : dfaCharSet)
            {
                divideSet(i, ch);
            }
        }
        int size2 = dividedSetVec.size();
        if (size2 > size1)      // 如果size2 > size1 不成立，说明已经无法再划分出新的集合（size2 = size1）；否则还有划分的可能
        {
            continueFlag = 1;
        }
    }

    for (int dfaMinCount = 0; dfaMinCount < dividedSetVec.size(); dfaMinCount++)
    {
        auto& v = dividedSetVec[dfaMinCount];
        dfaMinNode d;
        d.flag = min_set_end(v);
        // 为最小化后得到的结点附上id
        d.id = dfaMinCount;
        // 逐个字符
        for (char ch : dfaCharSet)
        {
            if (v.size() == 0)
            {
                d.transitions[ch] = -1;   // 空集特殊判断
                continue;
            }
            int i = *(v.begin());
            if (dfaTable[i - 1].transitions.find(ch) == dfaTable[i - 1].transitions.end())
            {
                d.transitions[ch] = -1;   // 空集特殊判断
                continue;
            }
            int next_state = DFAToID[dfaTable[i - 1].transitions[ch]];
            int sameSetMark = dfaSameMinSet[next_state];
            d.transitions[ch] = sameSetMark;
        }
        dfaMinVector.push_back(d);
    }
}


void clearAll(){
    nodeCount = 0;
    nfaCharSet.clear();
    dfaCharSet.clear();
    statusTable.clear();
    insertionOrder.clear();
    startNFAstatus.clear();
    endNFAstatus.clear();
    dfaStatusSet.clear();
    dfaEndStatusSet.clear();
    dfaNotEndStatusSet.clear();
    dfaMinVector.clear();
    dividedSetVec.clear();
    dfaSameMinSet.clear();
    dfaTable.clear();
}

// 运行按钮
void Widget::on_runButton_clicked()
{
    clearAll();

    nfaCharSet.insert(EPSILON); // 放入epsilon
    QString regex = ui->plainTextEdit_2->toPlainText();   // 拿到正则表达式

    // 多行表达式合并成一行
    regex = merge(regex);

    // 预处理
    regex = preprocess(regex);
    qDebug() << regex;


    string regexStd = regex.toStdString();

    NFA nfa = regexToNFA(regexStd);

    // NFA转DFA
    NFAToDFA(nfa);

    DFAminimize();

    QMessageBox::about(this, "Toast", "运行成功！请点击其余按钮查看结果！");
}

// NFA展示按钮
void Widget::on_displayNFAButton_clicked()
{
    ui->tableWidget->clearContents(); // 清除表格中的数据
    ui->tableWidget->setRowCount(0); // 清除所有行
    ui->tableWidget->setColumnCount(0); // 清除所有列
    // 设置列数
    int n = 2 + nfaCharSet.size(); // 默认两列：Flag 和 ID
    ui->tableWidget->setColumnCount(n);

    // 字符和第X列存起来对应
    map<char, int> headerCharNum;

    // 设置表头
    QStringList headerLabels;
    headerLabels << "-:初态 +:终态" << "结点标号";
    int headerCount = 3;
    for (const auto& ch : nfaCharSet) {
        headerLabels << QString(ch);
        headerCharNum[ch] = headerCount++;
    }
    ui->tableWidget->setHorizontalHeaderLabels(headerLabels);

    // 设置行数
    int rowCount = statusTable.size();
    ui->tableWidget->setRowCount(rowCount);

    // 填充数据
    int row = 0;
    for (auto id : insertionOrder) {
        const nfaStatusNode& node = statusTable[id];
        cout << "id: " << id << endl;

        // Flag 列
        ui->tableWidget->setItem(row, 0, new QTableWidgetItem(QString::fromStdString(node.flag)));

        // ID 列
        ui->tableWidget->setItem(row, 1, new QTableWidgetItem(QString::number(node.id)));

        // TransitionChar 列
        int col = 2;
        for (const auto& transitionEntry : node.transition) {
            string resutlt = setTostring(transitionEntry.second);

            // 放到指定列数据
            ui->tableWidget->setItem(row, headerCharNum[transitionEntry.first] - 1, new QTableWidgetItem(QString::fromStdString(resutlt)));
            col++;
        }

        row++;
    }

    // 调整列宽
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    // 显示表格
    ui->tableWidget->show();
}

// DFA展示按钮
void Widget::on_displayDFAButton_clicked(){
    ui->tableWidget->clearContents(); // 清除表格中的数据
    ui->tableWidget->setRowCount(0); // 清除所有行
    ui->tableWidget->setColumnCount(0); // 清除所有列

    // 设置列数
    int n = 2 + dfaCharSet.size(); // 默认两列：Flag 和 状态集合
    ui->tableWidget->setColumnCount(n);

    // 字符和第X列存起来对应
    map<char, int> headerCharNum;

    // 设置表头
    QStringList headerLabels;
    headerLabels << "-:初态 +:终态" << "状态集合";
    int headerCount = 3;
    for (const auto& ch : dfaCharSet) {
        headerLabels << QString(ch);
        headerCharNum[ch] = headerCount++;
    }
    ui->tableWidget->setHorizontalHeaderLabels(headerLabels);

    // 设置行数
    int rowCount = dfaTable.size();
    ui->tableWidget->setRowCount(rowCount);

    // 填充数据
    int row = 0;
    for (auto& dfaNode : dfaTable) {

        // Flag 列
        ui->tableWidget->setItem(row, 0, new QTableWidgetItem(QString::fromStdString(dfaNode.flag)));

        // 状态集合 列
        ui->tableWidget->setItem(row, 1, new QTableWidgetItem(QString::fromStdString("{" + setTostring(dfaNode.nfaStates) + "}")));

        // 状态转换 列
        int col = 2;
        for (const auto& transitionEntry : dfaNode.transitions) {
            string re = setTostring(transitionEntry.second);

            // 放到指定列数据
            ui->tableWidget->setItem(row, headerCharNum[transitionEntry.first] - 1, new QTableWidgetItem(QString::fromStdString("{" + re + "}")));
            col++;
        }

        row++;
    }

    // 调整列宽
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    // 显示表格
    ui->tableWidget->show();

}

// 最小化DFA展示按钮
void Widget::on_displayMinDFAButton_clicked()
{
    ui->tableWidget->clearContents(); // 清除表格中的数据
    ui->tableWidget->setRowCount(0); // 清除所有行
    ui->tableWidget->setColumnCount(0); // 清除所有列

    // 设置列数
    int n = 2 + dfaCharSet.size(); // 默认两列：Flag 和 状态集合
    ui->tableWidget->setColumnCount(n);

    // 字符和第X列存起来对应
    map<char, int> headerCharNum;

    // 设置表头
    QStringList headerLabels;
    headerLabels << "-:初态 +:终态" << "结点标号";
    int headerCount = 3;
    for (const auto& ch : dfaCharSet) {
        headerLabels << QString(ch);
        headerCharNum[ch] = headerCount++;
    }
    ui->tableWidget->setHorizontalHeaderLabels(headerLabels);

    // 设置行数
    int rowCount = dfaMinVector.size();
    ui->tableWidget->setRowCount(rowCount);

    // 填充数据
    int row = 0;
    for (auto& dfaNode : dfaMinVector) {

        // Flag 列
        ui->tableWidget->setItem(row, 0, new QTableWidgetItem(QString::fromStdString(dfaNode.flag)));

        // 状态集合 列
        ui->tableWidget->setItem(row, 1, new QTableWidgetItem(QString::number(dfaNode.id)));

        // 状态转换 列
        int col = 2;
        for (const auto& transitionEntry : dfaNode.transitions) {
            // 放到指定列数据
            ui->tableWidget->setItem(row, headerCharNum[transitionEntry.first] - 1, new QTableWidgetItem(transitionEntry.second == -1 ? QString::fromStdString("") : QString::number(transitionEntry.second)));
            col++;
        }

        row++;
    }

    // 调整列宽
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    // 显示表格
    ui->tableWidget->show();

}

// 打开正则表达式txt文件
void Widget::on_uploadButton_clicked()
{
    QString filePath = QFileDialog::getOpenFileName(this, tr("选择文件"), QDir::homePath(), tr("文本文件 (*.txt);;所有文件 (*.*)"));

    if (!filePath.isEmpty())
    {
        ifstream inputFile;
        QTextCodec* code = QTextCodec::codecForName("GB2312");

        string selectedFile = code->fromUnicode(filePath.toStdString().c_str()).data();
        inputFile.open(selectedFile.c_str(), ios::in);


        //        cout<<filePath.toStdString();
        //        ifstream inputFile(filePath.toStdString());
        if (!inputFile) {
            QMessageBox::critical(this, "错误", "无法打开文件！");
            cerr << "Error opening file." << endl;
        }
        // 读取文件内容并显示在 plainTextEdit_2
        stringstream buffer;
        buffer << inputFile.rdbuf();
        QString fileContents = QString::fromStdString(buffer.str());
        ui->plainTextEdit_2->setPlainText(fileContents);
    }

}

// 下载正则表达式txt文件
void Widget::on_downloadButton_clicked()
{
    // 保存结果到文本文件
    QString saveFilePath = QFileDialog::getSaveFileName(this, tr("保存结果文件"), QDir::homePath(), tr("文本文件 (*.txt)"));
    if (!saveFilePath.isEmpty() && !ui->plainTextEdit_2->toPlainText().isEmpty()) {
        QFile outputFile(saveFilePath);
        if (outputFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream stream(&outputFile);
            stream << ui->plainTextEdit_2->toPlainText();
            outputFile.close();
            QMessageBox::about(this, "Toast", "已保存！");
        }
    }
    else if (ui->plainTextEdit_2->toPlainText().isEmpty())
    {
        QMessageBox::warning(this, tr("Toast"), tr("输入框为空，请重试！"));
    }
}

