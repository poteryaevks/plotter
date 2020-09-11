#include <algorithm>
#include <assert.h>
#include <string>
#include <string>
#include <sstream>
#include "calc.h"

#include <stack>
#include <functional>

Calculator::Calculator()
    : nCurrentIndex(0),
      nNumberOfOperators(0),
      expression(std::string())
{
    setlocale(LC_ALL, "Russian");
}

Calculator::~Calculator()
{

}

double Calculator::Calc(const std::string &str)
{
    expression = str;
    std::remove_if(expression.begin(), expression.end(), [](char ch){
        return isspace(ch);
    });

    char *RPNstr = const_cast<char*>(expression.c_str());

    try	{
        isExpressionCorrect();
    }
    catch (const std::exception&) {
        throw;
    }

    RPNbuilder(RPNstr);
    double res = RPNcalc();

    for (size_t i = 0; i < cleaner.size(); i++) {
        char* temp = cleaner[i];
        delete[] temp;
    }

    cleaner.clear();
    RPNStack.clear();
    SimbolStack = stack<char*>();
    NumToLoad = stack<int>();

    return res;
}

bool Calculator::isNenegativeNumber(char* s)
{
    return (*s == '-'
            && *(s + 1) != '\0');
}

bool Calculator::isCharAllowed(char s)
{
    return (isSymbol(s)
            || isdigit((int)s)
            || isOperator(s));
}


bool Calculator::isOperator(char s)
{
    return (s == 0x2A
            || s == 0x2B
            || s == 0x2D
            || s == 0x2F
            || s == 0x5E); // ^ - �������
}

bool Calculator::isDotNumber(char s)
{
    return (isdigit((int)s)
            || isDot(s));

}


bool Calculator::isSymbol(char s)
{
    return (s == 0x28
            ||s == 0x29
            || s == 0x2C
            || s == 0x2E);
}

bool Calculator::TwoNeighborChars(std::string ::iterator &it)
{
    bool f1(false), f2(false), f3(false);

    if(it != expression.end()) {
        auto it_next = it;
        ++it_next;
        f1 =  (isOperator(*it) && isOperator(*it_next));
        //
        f2 = ((isOperator(*it) && *it != '-') && *it_next == ')');
        f3 = (*it == '(' && (isOperator(*it_next) && *it_next != '-'));

    }
    return (f1 || f2 || f3);
}

bool Calculator::isDot(char s)
{
    return (s == 0x2C
            || s == 0x2E);
}

bool Calculator::isDotCorrect(std::string ::iterator &it)
{
    bool f1(true), f2(true);
    if(it != expression.begin()) {
        f1 = isdigit((int)(*(--it)));
        it++;
    }
    if(it != expression.end()) {
        isdigit((int)(*(++it)));
        it--;
    }
    return (f1 && f2);
}

bool Calculator::NumberOfHooksCorrect()
{
    auto rHookNum = std::count(expression.begin(), expression.end(), '(');
    auto lHookNum = std::count(expression.begin(), expression.end(), ')');
    return (rHookNum == lHookNum);
}


bool Calculator::isNewLine(char s)
{
    return (s == 0x28);
}


bool Calculator::isHook(char s)
{
    return (s == 0x28 || s == 0x29);
}

bool Calculator::isMinus(char s)
{
    return (s == '-');
}

void Calculator::isExpressionCorrect()
{
    int nNumberOfDots { 0 }; //����������� ����� � �����

    auto s = expression.begin();
    //�������� ������ ���������� � �����, ���� ��� �� '-'
    if (isOperator(*s) && *s != '-')
        throw std::invalid_argument("uncorrect expression");

    if (isSymbol(*s) && *s != '(')
        throw std::invalid_argument("uncorrect expression");

    if (!NumberOfHooksCorrect())
        throw std::invalid_argument("uncorrect expression");


    for(; s != expression.end(); s++) {
        //� ����� ����� ���� ������ ���� ����� ��� �������
        if (nNumberOfDots > 1)
            return throw std::invalid_argument("uncorrect expression");

        //������ ��������?
        if (!isCharAllowed(*s))
            throw std::invalid_argument("uncorrect symbol");

        //�� ������ ���� 2 �������� ������, ����� ������
        else if (TwoNeighborChars(s))
            throw std::invalid_argument("uncorrect expression");

        //����� ��� ������� ����� ���������� ������ ����� �������
        else if (isDot(*s)) {
            nNumberOfDots++;
            if (!isDotCorrect(s))
                throw std::invalid_argument("uncorrect expression");
        }

        //���������� ������� �������, ���� �������� ����� ��������
        else if ((isOperator(*s) || isHook(*s))
                 && nNumberOfDots > 0)
            nNumberOfDots--;
    }

    s--;
    if (isOperator(*s))
        throw std::invalid_argument("uncorrect expression");

    if (isSymbol(*s) && *s != ')')
        throw std::invalid_argument("uncorrect expression");
}



int Calculator::RPNparse(char* s)
{
    /* ���������� ����� �������������� �������� */
    switch (*s) {
    case '-':
        /* ���� ����� �������� ������ � �� ��������� �������� ���������,
         * ������ ������������ ���� ������������� ����� � ���������� ���
         * ��� �������� ��������� �� ����� */
        if (isNenegativeNumber(s))
            break;
        if (nCurrentIndex >= 2) {
            nCurrentIndex -= 1;
            return SUB;
        }
        else return SUF;

    case ' ':
        return VAL;

    case '+':
        if (nCurrentIndex >= 2) {
            nCurrentIndex -= 1;
            return eCalcEnum::ADD;
        }
        else return eCalcEnum::SUF;

    case '*':
        if (nCurrentIndex >= 2) {
            nCurrentIndex -= 1;
            return MUL;
        }
        else
            return SUF;

    case '/':
        if (nCurrentIndex >= 2) {
            nCurrentIndex -= 1;
            return DIV;
        }
        else
            return SUF;
    case '^':
        if (nCurrentIndex >= 2) {
            nCurrentIndex -= 1;
            return DEG;
        }
        else
            return SUF;
    case 'r':
        return END;
    }

    double temp = 0; //�����
    char * endptr = (char*)"";  //��� ����������� ������
    /* �������� ��������������� ��������� �������� � ����� */
    std::stringstream ss(s);
    ss >> temp;

    /* ������� ������ `������������ ��������' � ������ ������� */
    if (*endptr != '\0')
        return UNK;  //

    /* ���������, ���� �� ��������� ����� � �����
     * � ��������� � ��� �������, ����� ���������� ������ ������������ */
    if (nCurrentIndex < STKDPTH)
        dRPNValue[nCurrentIndex++] = temp;
    else
        return SOF;

    return VAL;
}

double Calculator::RPNcalc()
{
    /* ���������� ���� ��� �������� ���������� ��������� ������ */
    for(auto str : RPNStack) {
        /* �������� ���������� ������� �������� ��� ����� ���
         * ������ �������������� �������� */
        switch (RPNparse(str)) {
        case VAL: continue;
            /* ��������� */
        case eCalcEnum::ADD:
            dRPNValue[nCurrentIndex - 1] += dRPNValue[nCurrentIndex];
            break;

        case SUB:
            dRPNValue[nCurrentIndex - 1] -= dRPNValue[nCurrentIndex];
            break;

        case eCalcEnum::MUL:
            dRPNValue[nCurrentIndex - 1] *= dRPNValue[nCurrentIndex];
            break;

        case eCalcEnum::DEG:
            dRPNValue[nCurrentIndex - 1] = pow(dRPNValue[nCurrentIndex - 1], dRPNValue[nCurrentIndex]);
            break;

        case eCalcEnum::DIV:
            if (dRPNValue[nCurrentIndex] != 0) {
                dRPNValue[nCurrentIndex - 1] /= dRPNValue[nCurrentIndex];
                break;
            }
            else {
                fprintf(stderr, "������� �� ����!\n");
                return 0;
            }

            /* ��������� ������ */
        case eCalcEnum::SUF:
            fprintf(stderr, "������������ ���������!\n");
            return 0;

        case eCalcEnum::SOF:
            fprintf(stderr, "������������ �����!\n");
            return 0;

        case eCalcEnum::UNK:
            fprintf(stderr, "������������ ��������!\n");
            return 0;

        case eCalcEnum::END:
            nCurrentIndex = 0;
            return dRPNValue[0];
        }
    }
}

void Calculator::AddOperator(char *s)
{
    if (SimbolStack.size() == 0) {
        SimbolStack.push(s);
        int temp = NumToLoad.top();
        NumToLoad.pop();
        NumToLoad.push(++temp);
    }
    //���� *s ����� ��������� ���� ��� ������,
    //�� ��������� ���� �� SimbolStack ������ � ������ ������,
    //� *s ����������� � SimbolStack. � ���� ������ ��� ����������� ��� ������.
    else {
        if (((*s == '+' || *s == '-') &&
             (*SimbolStack.top() == '-' || *SimbolStack.top() == '+')) ||
                ((*s == '+' || *s == '-') &&
                 (*SimbolStack.top() == '/' || *SimbolStack.top() == '*')) ||
                ((*s == '*' || *s == '/') &&
                 (*SimbolStack.top() == '/' || *SimbolStack.top() == '*'))) {//���������
            char *temp = SimbolStack.top();
            RPNStack.push_back(temp);
            SimbolStack.pop();
            SimbolStack.push(s);
        }
        else if (*s == '^' &&
                 (*SimbolStack.top() == '/' ||
                  *SimbolStack.top() == '*' ||
                  *SimbolStack.top() == '+' ||
                  *SimbolStack.top() == '-')) {//����������
            SimbolStack.push(s);
            int temp = NumToLoad.top();
            NumToLoad.pop();
            NumToLoad.push(++temp);
        }
        else if ((*s == '/' ||
                  *s == '*' ||
                  *s == '+' ||
                  *s == '-') &&
                 *SimbolStack.top() == '^') {
            char *temp = SimbolStack.top();
            RPNStack.push_back(temp);
            SimbolStack.pop();
            SimbolStack.push(s);
        }
        else if (*s == '^' && *SimbolStack.top() == '^') {
            SimbolStack.push(s);
            int temp = NumToLoad.top();
            NumToLoad.pop();
            NumToLoad.push(++temp);
        }
        else
        {//����������
            SimbolStack.push(s);
            int temp = NumToLoad.top();
            NumToLoad.pop();
            NumToLoad.push(++temp);
        }
    }

}


void Calculator::SimbStackExec(char *s)  //�������� ������ ��� ��������
{
    switch (*s) {
    case '+':
        AddOperator(s);
        break;

    case '^':
        AddOperator(s);
        break;

    case '-':
        AddOperator(s);
        break;

    case '*':
        AddOperator(s);
        break;

    case '/':
        AddOperator(s);
        break;

    case '(':
        SimbolStack.push(s); //����� � ���� ��������
        NumToLoad.push(1);
        break;

    case ')':
        //��������� ��������� ������ �� SimbolStack � RPNStack
        //� ������� ��������� 2 ������� �� SimbolStack
        for (size_t i = 0; i < NumToLoad.top() - 1; i++) {
            char *temp = SimbolStack.top();
            RPNStack.push_back(temp);
            SimbolStack.pop();
        }
        SimbolStack.pop();
        NumToLoad.pop();
        break;
    default:
        break;
    }
}


//��������� ������ - ���������, ������� ������ ��������
bool Calculator::RPNbuilder(char *s)
{
    int nPPSize;
    bool bNewSubExpression = true; //������� ������ ��������� (������ ��� ������)
    NumToLoad.push(0); //������� �������� ����� 0
    do {
        if (bNewSubExpression) {
            if (isHook(*s)) {
                char* p = ExtractStr(s, SIMB_SIZE);
                SimbStackExec(p); //
                s += SIMB_SIZE;
                continue;
            }
            ConsiderNumber(s, &nPPSize); //� ������ ��������� ������ �����
            char* p = ExtractStr(s, nPPSize);
            RPNStack.push_back(p);
            s += nPPSize;
            bNewSubExpression = false;
        }
        else if (isOperator(*s)) {
            char* p = ExtractStr(s, SIMB_SIZE);
            SimbStackExec(p); //
            s += SIMB_SIZE;
        }
        else if (isdigit((int)*s)) {
            ConsiderNumber(s, &nPPSize);
            char* p = ExtractStr(s, nPPSize);
            RPNStack.push_back(p);
            s += nPPSize;
        }
        else if (isHook(*s)) {
            if (isNewLine(*s)) {
                bNewSubExpression = true;
                continue;
            }
            char* p = ExtractStr(s, SIMB_SIZE);
            SimbStackExec(p); //
            s += SIMB_SIZE;

        }
    } while (*s);

    for (size_t i = 0; i < NumToLoad.top(); i++){
        char *temp = SimbolStack.top();
        RPNStack.push_back(temp);
        SimbolStack.pop();
    }

    RPNStack.push_back("ready");
    return 0;
}


char *Calculator::ExtractStr(char *s, int offset)
{
    int size;
    bool rec = false;
    int index = 0;
    size = offset;
    if (size < 0)
        throw "size ������ ����!";

    char *temp = new char[size + 1];  //                              <---------------------------------------------
    cleaner.push_back(temp);
    for (size_t i = 0; i < offset; i++) {
        temp[index] = s[i];
        index++;
    }
    temp[index] = '\0';

    return temp;
}

void Calculator::ConsiderNumber(char *s, int *pnSize) //����� � ������ �� ��������� ������ ���������� ����� pnSize
{
    *pnSize = 0;
    bool flag = true;
    do {
        if (*s == '-' && flag) {//���� ����� �������������
            flag = false;
            (*pnSize)++;
            continue;
        }
        (*pnSize)++;

    } while (isDotNumber(*++s));
}
