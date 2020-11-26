#pragma once

#include <iostream>
#include <errno.h>
#include <stdio.h>
#include <string>
#include <stack>
#include <vector>
#include <stdlib.h>
#include <math.h>

using namespace std;

#define STKDPTH 320 /* Глубина стека */
#define SIMB_SIZE  1

/* Значения, возвращаемые функцией parse */
enum eCalcEnum {
    VAL =  0  /* В стек занесено новое значение */
    ,ADD =  1  /* Сложение */
    ,SUB =  2  /* Вычитание */
    ,MUL =  3  /* Умножение */
    ,DIV =  4  /* Деление */
    ,DEG =  5  /* Возведение в степень */
    ,SOF = -1  /* Переполнение стека */
    ,SUF = -2  /* В стеке недостаточно операндов */
    ,UNK = -3  /* Неопознанное значение */
    ,END = -4
};


class Calculator
{
public:
    Calculator();
    virtual ~Calculator();

public:
    double Calc(const std::string &str);

private:
    //правила формирования выражения
    bool isNenegativeNumber(char* s);
    void isExpressionCorrect(); //проверка на правильность записи мат. выражения
    bool isCharAllowed(char s);
    bool TwoNeighborChars(std::string ::iterator &it);
    bool isNewLine(char s);
    bool isMinus(char s);
    bool isOperator(char s); // *, /, -, +
    bool isSymbol(char s);   // ), (, ., ,
    bool isDot(char s);
    bool isDotCorrect(std::string ::iterator &it);
    bool isHook(char s);
    bool isDotNumber(char s);
    bool NumberOfHooksCorrect(); //проверка числа скобок
    //
    int RPNparse(char *); //
    bool RPNbuilder(char* s); //формирует обратную польскую запись ОПЗ
    void ConsiderNumber(char *s, int *pnSize);
    char* ExtractStr(char *s, int offset);
    double RPNcalc();  //расчет  стека ОПЗ
    void SimbStackExec(char *s);
    void AddOperator(char *s);
    //
    int nCurrentIndex;
    double dRPNValue[STKDPTH];
    std::string expression;
    int nNumberOfOperators;
    std::stack<char*> SimbolStack;  //не может содержать более 1 оператора
    std::vector<char*>  RPNStack;
    std::stack<int>  NumToLoad;
    vector<char*> cleaner;
};

