#pragma once

#include <iostream>
#include <string>
#include <stack>
#include <vector>

#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <math.h>


//!
//! \brief The PvLineCalculator class
//!
class PvLineCalculator
{

public:

    //!
    //! \brief PvLineCalculator
    //!
    PvLineCalculator();

    virtual ~PvLineCalculator();

public:

    //!
    //! \brief calculate
    //! \param str
    //! \return
    //!
    double calculate(const std::string &str);

private:

    static constexpr int STACK_DEPTH { 320 }; // Глубина стека
    static constexpr int SIMB_SIZE {1};

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
    double dRPNValue[STACK_DEPTH];
    std::string expression;
    std::stack<char*> SimbolStack;  //не может содержать более 1 оператора
    std::vector<char*>  RPNStack;
    std::stack<int>  NumToLoad;
    std::vector<char*> cleaner;
};

