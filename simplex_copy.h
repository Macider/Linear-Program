#pragma once
#include <cassert>
#include <cfloat>
#include <iostream>
#include <string>
#include <vector>

#include "inputdata.h"
using namespace std;

// initialize   //找到一组基解
// simplex      //找到入基变量和出基变量
// pivot        //入基&出基


class Base {
   public:
    int* baseVarOfConstraint;  // m member in [0,n)
    int* constraintOfBaseVar;  // n member in [0,m) //互为逆运算
    int enterBaseVar;
    int leaveBaseVar;
    Base(int n, int m) {
        this->baseVarOfConstraint = new int[m];
        fill_n(this->baseVarOfConstraint, m, -1);
        this->constraintOfBaseVar = new int[n];
        fill_n(this->constraintOfBaseVar, n, -1);  //-1=uncheck,m=nonbasic
        enterBaseVar = leaveBaseVar = -1;
    }
};

Base* Initialize(StandardForm* stdFm) {
    cout << "Init!" << endl;
    Base* base = new Base(stdFm->n, stdFm->m);
    for (int i_m = 0; i_m < stdFm->m; i_m++)
        for (int i_n = 0; i_n < stdFm->n; i_n++) {
            if (base->constraintOfBaseVar[i_n] >= 0)
                continue;
            if (stdFm->P.at(i_n).at(i_m) == 0)
                continue;
            if (stdFm->P.at(i_n).at(i_m) != 1) {
                // base->varState[i_n] = NONBASIC_VAR;
                base->constraintOfBaseVar[i_n] = stdFm->m;
                continue;
            }
            for (int j_m = i_m + 1; j_m < stdFm->m; j_m++)
                if (stdFm->P.at(i_n).at(j_m) != 0) {
                    // base->varState[i_n] = NONBASIC_VAR;
                    base->constraintOfBaseVar[i_n] = stdFm->m;
                    break;
                }
            if (base->constraintOfBaseVar[i_n] == -1) {
                base->constraintOfBaseVar[i_n] = i_m;
                base->baseVarOfConstraint[i_m] = i_n;
            }
        }
    int baseCount = 0;
    for (int i_n = 0; i_n < stdFm->n; i_n++) {
        assert(base->constraintOfBaseVar[i_n] != -1);
        if (base->constraintOfBaseVar[i_n] < stdFm->m)
            baseCount++;
    }
    if (baseCount < stdFm->m)
        cout << "缺少" << stdFm->m - baseCount << "个基变量" << endl;
    return base;
}

int Simplex(StandardForm* stdFm, Base* base) {
    cout << "Simplex!" << endl;
    double lamda[stdFm->n] = {0};  // 检验系数
    double maxLamda = 0;
    int enterBaseVar = -1;  // 入基变量
    for (int i_n = 0; i_n < stdFm->n; i_n++) {
        if (base->constraintOfBaseVar[i_n] < stdFm->m)
            continue;
        lamda[i_n] = stdFm->C.at(i_n);
        for (int i_m = 0; i_m < stdFm->m; i_m++)  // baseVarOfConstraint[i_m]是第i_m号方程对应的基变量
            lamda[i_n] -= stdFm->C.at(base->baseVarOfConstraint[i_m]) * stdFm->P.at(i_n).at(i_m);
        cout << "Lamda[" << i_n << "] = " << lamda[i_n] << endl;
        if (lamda[i_n] >= maxLamda) {
            enterBaseVar = i_n;  // 总是选择序号大的变量入基
            maxLamda = lamda[i_n];
        }
        if (lamda[i_n] > 0)
            for (int i_m = 0; i_m < stdFm->m; i_m++) {
                if (stdFm->P.at(i_n).at(i_m) > 0)
                    break;
                if (i_m == stdFm->m - 1) {
                    cout << "无有界最优解" << endl;
                    return 3;
                }
            }
    }
    if (enterBaseVar == -1) {  // 所有检验数均为负
        cout << "所有检验数均为负，到达最优解" << endl;
        return 1;
    }
    if (maxLamda == 0) {  // 最大检验数为0，可能为无穷多最优解
        cout << "最大检验数为0" << endl;
        return 2;
    }
    double minTheta = DBL_MAX;
    int leaveBaseVar = -1;
    for (int i_m = 0; i_m < stdFm->m; i_m++)
        if (stdFm->P.at(enterBaseVar).at(i_m) > 0)
            if (stdFm->B.at(i_m).second / stdFm->P.at(enterBaseVar).at(i_m) < minTheta) {
                minTheta = stdFm->B.at(i_m).second / stdFm->P.at(enterBaseVar).at(i_m);
                leaveBaseVar = base->baseVarOfConstraint[i_m];  // i_m号方程对应的基变量出基
            }
    base->enterBaseVar = enterBaseVar;
    base->leaveBaseVar = leaveBaseVar;
    cout << "继续！" << endl;
    return 0;  // 继续进行
}

void Pivot(StandardForm* stdFm, Base* base) {
    cout << "Pivot!" << endl;
    int r = base->constraintOfBaseVar[base->leaveBaseVar];  // 出基变量所在方程 // in [0,m)
    int k = base->enterBaseVar;                             // 入基变量编号     // in [0,n)
    double Pkr = stdFm->P.at(k).at(r);
    for (int i_n = 0; i_n < stdFm->n; i_n++)
        stdFm->P.at(i_n).at(r) /= Pkr;
    stdFm->B.at(r).second /= Pkr;

    for (int i_m = 0; i_m < stdFm->m; i_m++) {
        if (i_m == r)
            continue;
        double Pkm = stdFm->P.at(k).at(i_m);
        for (int i_n = 0; i_n < stdFm->n; i_n++)
            stdFm->P.at(i_n).at(i_m) -= Pkm * stdFm->P.at(i_n).at(r);
        stdFm->B.at(i_m).second -= Pkm * stdFm->B.at(r).second;
    }

    for (int i_m = 0; i_m < stdFm->m; i_m++) {
        for (int i_n = 0; i_n < stdFm->n; i_n++) {
            cout << " ";
            if (stdFm->P.at(i_n).at(i_m) >= 0)
                cout << "+";
            cout << stdFm->P.at(i_n).at(i_m);
        }
        cout << " = " << stdFm->B.at(i_m).second << endl;
    }

    base->constraintOfBaseVar[k] = r;                          // k号变量是r号方程的基变量
    base->baseVarOfConstraint[r] = k;                          // r号方程以k号变量为基变量
    base->constraintOfBaseVar[base->leaveBaseVar] = stdFm->m;  // 出基变量不再作为任何方程的基变量
}

void SimplexMethod(StandardForm* stdFm0) {
    StandardForm* stdFm = new StandardForm(*stdFm0);  // 深拷贝
    Base* base = Initialize(stdFm);
    int result = -1;
    while (!(result = Simplex(stdFm, base)))
        Pivot(stdFm, base);
    if (result == 1) {
        double res = 0;
        for (int i_n = 0; i_n < stdFm->n; i_n++)
            if (stdFm->C.at(i_n))
                res += stdFm->C.at(i_n) * stdFm->B.at(base->constraintOfBaseVar[i_n]).second;
        cout << "result is " << res;
        cout << ", got at (";
        for (int i_n = 0; i_n < stdFm->n; i_n++) {
            if (base->constraintOfBaseVar[i_n] >= 0 && base->constraintOfBaseVar[i_n] < stdFm->m)
                cout << stdFm->B.at(base->constraintOfBaseVar[i_n]).second;
            else
                cout << "0";
            if (i_n == stdFm->n - 1)
                cout << ")" << endl;
            else
                cout << ", ";
        }
    }
}
