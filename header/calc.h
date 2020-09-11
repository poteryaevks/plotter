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

#define STKDPTH 320 /* ������� ����� */
#define SIMB_SIZE  1

/* ��������, ������������ �������� parse */
enum eCalcEnum {
    VAL =  0  /* � ���� �������� ����� �������� */
    ,ADD =  1  /* �������� */
    ,SUB =  2  /* ��������� */
    ,MUL =  3  /* ��������� */
    ,DIV =  4  /* ������� */
    ,DEG =  5  /* ���������� � ������� */
    ,SOF = -1  /* ������������ ����� */
    ,SUF = -2  /* � ����� ������������ ��������� */
    ,UNK = -3  /* ������������ �������� */
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
    //������� ������������ ���������
    bool isNenegativeNumber(char* s);
    void isExpressionCorrect(); //�������� �� ������������ ������ ���. ���������
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
    bool NumberOfHooksCorrect(); //�������� ����� ������
    //
    int RPNparse(char *); //
    bool RPNbuilder(char* s); //��������� �������� �������� ������ ���
    void ConsiderNumber(char *s, int *pnSize);
    char* ExtractStr(char *s, int offset);
    double RPNcalc();  //������  ����� ���
    void SimbStackExec(char *s);
    void AddOperator(char *s);
    //
    int nCurrentIndex;
    double dRPNValue[STKDPTH];
    std::string expression;
    int nNumberOfOperators;
    std::stack<char*> SimbolStack;  //�� ����� ��������� ����� 1 ���������
    std::vector<char*>  RPNStack;
    std::stack<int>  NumToLoad;
    vector<char*> cleaner;
};

