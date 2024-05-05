/*
 * 单纯形法
 */
#pragma once

#include "./base.h"
#include "./problem.h"
using namespace std;

// 数据结构区

// 声明区

ResultType Simplex(Problem*, Base*);  // 找到入基变量和出基变量
void Pivot(Problem*, Base*);          // 入基&出基
Problem* SimplexMethod(Problem*);     // 依序调用initialize、simplex、pivot

// 函数区

ResultType Simplex(Problem* pblm, Base* base) {
    // 先定入基，再定出基
    // cout << "Simplex!";
    int m = pblm->B.size(), n = pblm->X.size();
    // cout << "m is " << m << ", n is " << n << endl;
    double maxLamda = 0;    // 寻找最大的检验数对应变量入基
    int enterBaseVar = -1;  // 入基变量
    for (int i_n = 0; i_n < n; i_n++)
        if (base->constraintOfBaseVar[i_n] != NOTBASIC)
            pblm->offset += pblm->C.at(i_n) * pblm->B.at(base->constraintOfBaseVar[i_n]).second;
    for (int i_n = 0; i_n < n; i_n++) {
        if (base->constraintOfBaseVar[i_n] != NOTBASIC)  // 在找入基变量，不用考虑基变量
            continue;
        for (int i_m = 0; i_m < m; i_m++)  // baseVarOfConstraint[i_m]是第i_m号方程对应的基变量
            pblm->C.at(i_n) -= pblm->C.at(base->baseVarOfConstraint[i_m]) * pblm->P.at(i_n).at(i_m);
        // cout << "Lamda[" << pblm->X.at(i_n).name << "] = " << pblm->C.at(i_n) << endl;
        if (largeEquals(pblm->C.at(i_n), maxLamda)) {
            enterBaseVar = i_n;  // 总是选择序号大的变量入基
            maxLamda = pblm->C.at(i_n);
        }
        if (largeThan(pblm->C.at(i_n), 0))
            for (int i_m = 0; i_m < m; i_m++) {
                if (largeThan(pblm->P.at(i_n).at(i_m), 0))  
                    break;
                if (i_m == m - 1) {
                    cout << "无有界最优解" << endl;
                    return UNBOUNDED;
                }
            }
    }
    for (int i_m = 0; i_m < m; i_m++)
        pblm->C.at(base->baseVarOfConstraint[i_m]) = 0;  //  基变量系数置为0
    // 至此已决定入基变量
    // cout << "maxLamda is " << maxLamda << ", enterBaseVar is " << enterBaseVar << endl;
    // cout << "    当前目标函数为";
    // pblm->OutputTarget();
    if (enterBaseVar == -1) {  // 所有检验数均为负
        cout << "    所有检验数均为负，到达最优解" << endl;
        return ONE_SOLUTION;
    }
    if (equals(maxLamda, 0)) {  // 最大检验数为0，可能为无穷多最优解
        cout << "    最大检验数为0" << endl;
        return MAYBE_MANY;
    }
    double minTheta = DBL_MAX;
    int leaveBaseVar = -1;
    for (int i_m = 0; i_m < m; i_m++) {
        if (pblm->P.at(enterBaseVar).at(i_m) < 0 || equals(pblm->P.at(enterBaseVar).at(i_m), 0))
            continue;
        if (pblm->B.at(i_m).second / pblm->P.at(enterBaseVar).at(i_m) < minTheta) {
            minTheta = pblm->B.at(i_m).second / pblm->P.at(enterBaseVar).at(i_m);
            leaveBaseVar = base->baseVarOfConstraint[i_m];  // i_m号方程对应的基变量出基
        }
    }
    // cout << "minTheta is " << minTheta << ", leaveBaseVar is " << leaveBaseVar << endl;
    assert(leaveBaseVar != -1);  // 某一列变量全部小于等于0
    base->enterBaseVar = enterBaseVar;
    base->leaveBaseVar = leaveBaseVar;

    // cout << "    出基变量为" << pblm->X.at(leaveBaseVar).name << "; 入基变量为" << pblm->X.at(enterBaseVar).name << endl;
    // pblm->OutputResult();
    // assert(pblm->TestConstraint() == true);
    return UNKNOWN;  // 继续进行
}

void Pivot(Problem* pblm, Base* base) {
    // 对给定的入基变量与出基变量，执行转轴(换基)操作
    // cout << "Pivot!" << endl;
    int m = pblm->B.size(), n = pblm->X.size();
    int rowChosen = base->constraintOfBaseVar[base->leaveBaseVar];  // 出基变量所在方程 // in [0,m)
    int colChosen = base->enterBaseVar;                             // 入基变量编号     // in [0,n)

    // 出基变量在rowChosen行，colChosen为入基变量，将rowChosen行colChosen列元素变为1
    PivotBase(pblm, base, rowChosen, colChosen);

    base->constraintOfBaseVar[colChosen] = rowChosen;          // k号变量是r号方程的基变量
    base->baseVarOfConstraint[rowChosen] = colChosen;          // r号方程以k号变量为基变量
    base->constraintOfBaseVar[base->leaveBaseVar] = NOTBASIC;  // 出基变量不再作为任何方程的基变量
}

Base* SimplexInitialize(Problem*& pblm) {  // 产生了新的pblm代替原有的，因此需要传引用
    if (!pblm->IsStandard())
        pblm = pblm->Standardlize();
    // pblm->OutputPblm();
    return GetBase(pblm);
}

Problem* SimplexMethod(Problem* pblm0) {
    // 对输入的对象进行单纯形法求解，返回变形结束的单纯形法
    // 前置要求：max、B>=0、X>=0、约束取等
    Problem* pblm = new Problem(*pblm0);  // 深拷贝

    Base* base = SimplexInitialize(pblm);
    if (pblm->result == UNKNOWN)
        while (Simplex(pblm, base) == UNKNOWN)
            Pivot(pblm, base);
    cout << "End!" << endl;
    pblm->SimplifyDouble();
    // pblm->OutputTarget();
    // pblm->OutputConstraint();

    // cout << "测试原始约束" << endl;
    // Problem* testPblm = new Problem(*pblm0);
    // int n = testPblm->X.size();
    // for (int i_n = 0; i_n < n; i_n++) {
    //     testPblm->X.at(i_n).value = pblm->X.at(i_n).value;
    // }
    // testPblm->TestConstraint();
    // delete testPblm;

    return pblm;
}
