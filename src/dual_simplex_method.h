/*
 * 对偶单纯形法
 */
// 在无解时会怎么样
#pragma once

#include "./problem.h"
#include "./simplex_method.h"
using namespace std;

// 声明区
ResultType DualSimplex(Problem*, Base*);  // 找到入基变量和出基变量
Problem* DualSimplexMethod(Problem*);     // 依序调用initialize、simplex、pivot

// 函数区
ResultType DualSimplex(Problem* pblm, Base* base) {
    // 先定出基，再定入基
    cout << "Simplex!" << endl;
    int m = pblm->B.size(), n = pblm->X.size();

    double ofst = 0;
    for (int i_m = 0; i_m < m; i_m++) {
        cout << "C = " << pblm->C.at(base->baseVarOfConstraint[i_m]) << ",B = " << pblm->B.at(i_m).second << endl;
        ofst -= pblm->C.at(base->baseVarOfConstraint[i_m]) * pblm->B.at(i_m).second;
    }
    pblm->offset = ofst;

    int leaveBaseVar = -1;  // 出基变量
    double minB = 0;
    for (int i_m = 0; i_m < m; i_m++)
        if (pblm->B.at(i_m).second <= minB) {
            leaveBaseVar = base->baseVarOfConstraint[i_m];
            minB = pblm->B.at(i_m).second;
        }
    if (leaveBaseVar == -1) {
        cout << "对偶问题所有检验数均为负，到达最优解" << endl;
        return ONE_SOLUTION;
    }
    if (minB == 0) {
        cout << "对偶问题最大检验数为0" << endl;
        return MAYBE_MANY;
    }
    cout << "leaveBaseVar is " << leaveBaseVar << endl;
    double minLamda = DBL_MAX;  // 寻找最小的检验数对应变量入基
    int enterBaseVar = -1;
    for (int i_n = 0; i_n < n; i_n++) {
        cout << "i_n is " << i_n << endl;
        cout << "constraintOfBaseVar[leaveBaseVar] is " << base->constraintOfBaseVar[leaveBaseVar] << endl;
        if (pblm->P.at(i_n).at(base->constraintOfBaseVar[leaveBaseVar]) >= 0)
            continue;
        double lamda = pblm->C.at(i_n);
        for (int i_m = 0; i_m < m; i_m++)
            lamda -= pblm->C.at(base->baseVarOfConstraint[i_m]) * pblm->P.at(i_n).at(i_m);
        lamda /= pblm->P.at(i_n).at(base->constraintOfBaseVar[leaveBaseVar]);
        if (lamda < minLamda) {
            minLamda = lamda;
            enterBaseVar = i_n;
        }
    }

    assert(enterBaseVar != -1);
    base->enterBaseVar = enterBaseVar;
    base->leaveBaseVar = leaveBaseVar;

    cout << "出基变量为" << pblm->X.at(leaveBaseVar).name << "; 入基变量为" << pblm->X.at(enterBaseVar).name << endl;
    return UNKNOWN;  // 继续进行
}

Problem* DualSimplexMethod(Problem* pblm0) {
    // 对输入的对象进行对偶单纯形法求解，返回变形结束的单纯形法
    // 前置要求：max、C<=0、B<=0、X>=0、约束取等
    Problem* pblm = new Problem(*pblm0);  // 深拷贝
    if (!pblm->IsStandard())
        pblm = pblm->Standardlize();  // 标准化
    if (pblm->RangeB() != SMALL_EQUAL)
        pblm->ChangeB(SMALL_EQUAL);
    if (pblm->RangeC() != SMALL_EQUAL)
        cout << "RangeC error!" << endl;
    pblm->OutputPblm();
    // pblm = pblm->Dualize();
    int m = pblm->B.size(), n = pblm->X.size();

    Base* base = Initialize(pblm);
    if (pblm->result == UNKNOWN)
        while (DualSimplex(pblm, base) == UNKNOWN)
            Pivot(pblm, base);
    cout << "End!" << endl;
    return pblm;
}
