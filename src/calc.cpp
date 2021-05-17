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
            || s == 0x5E); // ^ - степень
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
    int nNumberOfDots { 0 }; //колличество точек у числа

    auto s = expression.begin();
    //выражени должно начинаться с числа, если это не '-'
    if (isOperator(*s) && *s != '-')
        throw std::invalid_argument("uncorrect expression");

    if (isSymbol(*s) && *s != '(')
        throw std::invalid_argument("uncorrect expression");

    if (!NumberOfHooksCorrect())
        throw std::invalid_argument("uncorrect expression");


    for(; s != expression.end(); s++) {
        //у числа может быть только одна точка или запятая
        if (nNumberOfDots > 1)
            return throw std::invalid_argument("uncorrect expression");

        //символ разрешен?
        if (!isCharAllowed(*s))
            throw std::invalid_argument("uncorrect symbol");

        //не должно быть 2 операнда подряд, кроме скобок
        else if (TwoNeighborChars(s))
            throw std::invalid_argument("uncorrect expression");

        //точка или запятая могут находиться только между числами
        else if (isDot(*s)) {
            nNumberOfDots++;
            if (!isDotCorrect(s))
                throw std::invalid_argument("uncorrect expression");
        }

        //сбрасываем счетчик запятых, если проходим через оператор
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
    /* Распознаем знаки арифметических операций */
    switch (*s) {
    case '-':
        /* Если минус является первым и не последним символом аргумента,
         * значит пользователь ввел отрицательное число и опознавать его
         * как операцию вычитания не нужно */
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

    double temp = 0; //число
    char * endptr = (char*)"";  //для определения ошибки
    /* Пытаемся сконвертировать строковый аргумент в число */
    std::stringstream ss(s);
    ss >> temp;

    /* Вернуть ошибку `неопознанный аргумент' в случае неудачи */
    if (*endptr != '\0')
        return UNK;  //

    /* Проверяем, есть ли свободное место в стеке
     * и сохраняем в нем операнд, иначе возвращаем ошибку переполнения */
    if (nCurrentIndex < STKDPTH)
        dRPNValue[nCurrentIndex++] = temp;
    else
        return SOF;

    return VAL;
}

double Calculator::RPNcalc()
{
    /* Организуем цикл для перебора аргументов командной строки */
    for(auto str : RPNStack) {
        /* Пытаемся распознать текущий аргумент как число или
         * символ арифметической операции */
        switch (RPNparse(str)) {
        case VAL: continue;
            /* Вычисляем */
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
                fprintf(stderr, "Деление на ноль!\n");
                return 0;
            }

            /* Обработка ошибок */
        case eCalcEnum::SUF:
            fprintf(stderr, "Недостаточно операндов!\n");
            return 0;

        case eCalcEnum::SOF:
            fprintf(stderr, "Переполнение стека!\n");
            return 0;

        case eCalcEnum::UNK:
            fprintf(stderr, "Неопознанный аргумент!\n");
            return 0;

        case eCalcEnum::END:
            nCurrentIndex = 0;
            return dRPNValue[0];
        }
    }
    return 0;
}

void Calculator::AddOperator(char *s)
{
    if (SimbolStack.size() == 0) {
        SimbolStack.push(s);
        int temp = NumToLoad.top();
        NumToLoad.pop();
        NumToLoad.push(++temp);
    }
    //если *s имеет приоритет ниже или равный,
    //то последний знак из SimbolStack уходит в массив выхода,
    //а *s добавляется в SimbolStack. В ином случае все добавляется как обычно.
    else {
        if (((*s == '+' || *s == '-') &&
             (*SimbolStack.top() == '-' || *SimbolStack.top() == '+')) ||
                ((*s == '+' || *s == '-') &&
                 (*SimbolStack.top() == '/' || *SimbolStack.top() == '*')) ||
                ((*s == '*' || *s == '/') &&
                 (*SimbolStack.top() == '/' || *SimbolStack.top() == '*'))) {//замещение
            char *temp = SimbolStack.top();
            RPNStack.push_back(temp);
            SimbolStack.pop();
            SimbolStack.push(s);
        }
        else if (*s == '^' &&
                 (*SimbolStack.top() == '/' ||
                  *SimbolStack.top() == '*' ||
                  *SimbolStack.top() == '+' ||
                  *SimbolStack.top() == '-')) {//добавление
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
        {//добавление
            SimbolStack.push(s);
            int temp = NumToLoad.top();
            NumToLoad.pop();
            NumToLoad.push(++temp);
        }
    }

}


void Calculator::SimbStackExec(char *s)  //добавить символ или выгрузки
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
        SimbolStack.push(s); //пишем в стек символов
        NumToLoad.push(1);
        break;

    case ')':
        //выгрузить последний символ из SimbolStack в RPNStack
        //и удалить последние 2 символа из SimbolStack
        for (int i = 0; i < NumToLoad.top() - 1; i++) {
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


//принимает строку - выражение, которое прошло проверку
bool Calculator::RPNbuilder(char *s)
{
    int nPPSize;
    bool bNewSubExpression = true; //признак нового выражения (начало или скобки)
    NumToLoad.push(0); //счетчик операций равен 0
    do {
        if (bNewSubExpression) {
            if (isHook(*s)) {
                char* p = ExtractStr(s, SIMB_SIZE);
                SimbStackExec(p); //
                s += SIMB_SIZE;
                continue;
            }
            ConsiderNumber(s, &nPPSize); //в начале выражения всегда ЧИСЛО
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
    int index = 0;
    size = offset;
    if (size < 0)
        throw "size меньше нуля!";

    char *temp = new char[size + 1];  //                              <---------------------------------------------
    cleaner.push_back(temp);
    for (int i = 0; i < offset; i++) {
        temp[index] = s[i];
        index++;
    }
    temp[index] = '\0';

    return temp;
}

void Calculator::ConsiderNumber(char *s, int *pnSize) //пишем в память по указателю размер найденного числа pnSize
{
    *pnSize = 0;
    bool flag = true;
    do {
        if (*s == '-' && flag) {//если число отрицательное
            flag = false;
            (*pnSize)++;
            continue;
        }
        (*pnSize)++;

    } while (isDotNumber(*++s));
}
