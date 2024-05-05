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
    // cout << "DualSimplex!" << endl;
    int m = pblm->B.size(), n = pblm->X.size();

    // double ofst = 0;
    // for (int i_m = 0; i_m < m; i_m++) {
    //     // cout << "C = " << pblm->C.at(base->baseVarOfConstraint[i_m]) << ",B = " << pblm->B.at(i_m).second << endl;
    //     ofst -= pblm->C.at(base->baseVarOfConstraint[i_m]) * pblm->B.at(i_m).second;
    // }
    // pblm->offset = ofst;

    int leaveBaseVar = -1;  // 出基变量
    double minB = 0;
    for (int i_m = 0; i_m < m; i_m++)
        if (smallThan(pblm->B.at(i_m).second, minB)) {
            leaveBaseVar = base->baseVarOfConstraint[i_m];
            minB = pblm->B.at(i_m).second;
        }
    if (leaveBaseVar == -1) {
        cout << "对偶问题所有检验数均为负，到达最优解" << endl;
        return ONE_SOLUTION;
    }
    if (equals(minB, 0)) {
        cout << "对偶问题最大检验数为0" << endl;
        return MAYBE_MANY;
    }
    // cout << "leaveBaseVar is " << leaveBaseVar << endl;
    // cout << "m is " << m << ", n is " << n << endl;
    double minLamda = DBL_MAX;  // 寻找最小的检验数对应变量入基 //事实上因为除了一个a，并不是检验数
    int enterBaseVar = -1;
    // pblm->OutputConstraint();
    for (int i_n = 0; i_n < n; i_n++) {  // 遍历所有变量，找到lamda最小的一个
        // cout << "i_n is " << i_n << endl;
        // cout << "constraintOfBaseVar[leaveBaseVar] is " << base->constraintOfBaseVar[leaveBaseVar] << endl;
        if (largeEquals(pblm->P.at(i_n).at(base->constraintOfBaseVar[leaveBaseVar]), 0))
            continue;
        double lamda = pblm->C.at(i_n);
        for (int i_m = 0; i_m < m; i_m++)
            lamda -= pblm->C.at(base->baseVarOfConstraint[i_m]) * pblm->P.at(i_n).at(i_m);
        lamda /= pblm->P.at(i_n).at(base->constraintOfBaseVar[leaveBaseVar]);
        // cout << "lamda is " << lamda << endl;
        if (smallThan(lamda, minLamda)) {       // 这里把等号加上会怎么样
            minLamda = lamda;
            enterBaseVar = i_n;
        }
    }

    assert(enterBaseVar != -1);
    if (enterBaseVar == -1) {
        cout << "无解" << endl;
        return NO_SOLUTION;
    }
    base->enterBaseVar = enterBaseVar;
    base->leaveBaseVar = leaveBaseVar;

    // cout << "出基变量为" << pblm->X.at(leaveBaseVar).name << "; 入基变量为" << pblm->X.at(enterBaseVar).name << endl;
    return UNKNOWN;  // 继续进行
}

Base* DualInitialize(Problem*& pblm) {
    pblm = pblm->Standardlize(DUAL_SIMPLEX);  // 需要进行特殊标准化

    int m = pblm->B.size();
    int n = pblm->X.size();

    bool CIsOK = true;  // 要求任意C<=0
    for (int i_n = 0; i_n < n; i_n++) {
        if (smallEquals(pblm->C.at(i_n), 0))
            continue;
        CIsOK = false;
    }
    assert(CIsOK);

    // 不需要保证存在b<0，如果找到一组基且这组基下任意c<=0，说明算法直接得到最优解
    // 由特殊标准化来保证一定能找到一组基

    return GetBase(pblm, DUAL_SIMPLEX);
}

Problem* DualSimplexMethod(Problem* pblm0) {
    // 对输入的对象进行对偶单纯形法求解，返回变形结束的单纯形法
    // 前置要求：max、任意C<=0、存在B<0、任意X>=0、任意约束取等
    // 其他都可以通过标准化实现，但任意C<=0比较复杂难以实现
    Problem* pblm = new Problem(*pblm0);  // 深拷贝

    Base* base = DualInitialize(pblm);
    // cout << "DualInitialize 后问题如下" << endl;
    // pblm->OutputPblm();
    if (pblm->result == UNKNOWN)
        while (DualSimplex(pblm, base) == UNKNOWN)
            Pivot(pblm, base);
    cout << "End!" << endl;

    cout << "测试原始约束" << endl;
    Problem* testPblm = new Problem(*pblm0);
    int n = testPblm->X.size();
    for (int i_n = 0; i_n < n; i_n++) {
        testPblm->X.at(i_n).value = pblm->X.at(i_n).value;
    }
    testPblm->TestConstraint();
    delete testPblm;

    return pblm;
}
