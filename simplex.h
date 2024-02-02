#pragma once
#include <cassert>
#include <cfloat>
#include <iostream>
#include <string>
#include <vector>

#include "inputdata.h"
using namespace std;

// 数据结构区
class Base {
   public:
    int* baseVarOfConstraint;  // m member in [0,n)
    int* constraintOfBaseVar;  // n member in [0,m) //互为逆运算
    int enterBaseVar;          // 将要入基的变量
    int leaveBaseVar;          // 将要出基的变量
    double offset;             // 基对应的目标函数的常数项
    Base(int n, int m, double ofst) {
        this->baseVarOfConstraint = new int[m];
        fill_n(this->baseVarOfConstraint, m, -1);  // -1 = uncheck, n is error
        this->constraintOfBaseVar = new int[n];
        fill_n(this->constraintOfBaseVar, n, -1);  // -1 = uncheck, m = nonbasic
        enterBaseVar = leaveBaseVar = -1;
        this->offset = ofst;
    }
};

// 声明区
Base* Initialize(Problem*);        // 找到一组基解
int Simplex(Problem*, Base*);      // 找到入基变量和出基变量
void Pivot(Problem*, Base*);       // 入基&出基
Problem* SimplexMethod(Problem*);  // 依序调用initialize、simplex、pivot

// 函数区
Base* Initialize(Problem* pblm) {
    cout << "Init!" << endl;
    int m = pblm->B.size(), n = pblm->X.size();
    Base* base = new Base(n, m, pblm->offset);
    for (int i_m = 0; i_m < m; i_m++)
        for (int i_n = 0; i_n < n; i_n++) {           // 遍历所有变量找到基变量
            if (base->constraintOfBaseVar[i_n] >= 0)  // 已作为基变量/不会作为基变量
                continue;
            if (pblm->P.at(i_n).at(i_m) == 0)
                continue;
            if (pblm->P.at(i_n).at(i_m) != 1) {  // 该变量不会作为任何约束条件的基变量
                base->constraintOfBaseVar[i_n] = m;
                continue;
            }
            assert(pblm->P.at(i_n).at(i_m) == 1);
            for (int j_m = i_m + 1; j_m < m; j_m++)
                if (pblm->P.at(i_n).at(j_m) != 0) {      // 检查该列其他Pij不为0
                    base->constraintOfBaseVar[i_n] = m;  // 该变量不会作为任何约束条件的基变量
                    break;
                }
            if (base->constraintOfBaseVar[i_n] == -1) {
                base->constraintOfBaseVar[i_n] = i_m;
                base->baseVarOfConstraint[i_m] = i_n;
            }
        }
    // int baseCount = 0;
    // for (int i_n = 0; i_n < n; i_n++) {
    //     assert(base->constraintOfBaseVar[i_n] != -1);
    //     if (base->constraintOfBaseVar[i_n] < m)
    //         baseCount++;
    // }
    // if (baseCount < m)
    //     cout << "缺少" << m - baseCount << "个基变量" << endl;
    int baseLack = 0;
    Problem* auxiliaryPblm = new Problem(*pblm);
    for (vValue::iterator itc_ = auxiliaryPblm->C.begin(); itc_ != auxiliaryPblm->C.end(); ++itc_)
        (*itc_) = 0;
    for (int i_m = 0; i_m < m; i_m++) {
        if (base->baseVarOfConstraint[i_m] != -1)  // 该约束已找到对应的基变量
            continue;
        baseLack++;
        auxiliaryPblm->X.push_back(make_pair(0, "__auxiliary_" + to_string(i_m) + "__"));
        auxiliaryPblm->Xrange.push_back(make_pair(LARGE_EQUAL, 0));
        auxiliaryPblm->C.push_back(-1);
        auxiliaryPblm->P.push_back(vector(auxiliaryPblm->B.size(), 0.0));
        auxiliaryPblm->P.back().at(i_m) = 1;
    }
    if (baseLack) {
        cout << "辅助问题如下所示" << endl;
        auxiliaryPblm->OutputPblm();
        // cout << "开始解辅助问题" << endl;
        auxiliaryPblm = SimplexMethod(auxiliaryPblm);
        // cout << "辅助问题约束条件可转化为" << endl;
        // auxiliaryPblm->OutputConstraint();
        vP::iterator itp = pblm->P.begin();
        vP::const_iterator itp_ = auxiliaryPblm->P.cbegin();
        for (; itp != pblm->P.end() && itp_ != auxiliaryPblm->P.cend(); ++itp, ++itp_)
            (*itp) = (*itp_);
        vRange::iterator itb = pblm->B.begin();
        vRange::const_iterator itb_ = auxiliaryPblm->B.cbegin();
        for (; itb != pblm->B.end() && itb_ != auxiliaryPblm->B.cend(); ++itb, ++itb_)
            (*itb) = (*itb_);
        cout << "原问题可转化为" << endl;
        pblm->OutputPblm();
        base = Initialize(pblm);
    }
    delete auxiliaryPblm;
    return base;
}

int Simplex(Problem* pblm, Base* base) {
    cout << "Simplex!" << endl;
    int m = pblm->B.size(), n = pblm->X.size();
    // double lamda[n] = {0};  // 检验系数
    // double lamda = 0;
    double maxLamda = 0;
    int enterBaseVar = -1;  // 入基变量
    for (int i_n = 0; i_n < n; i_n++)
        if (base->constraintOfBaseVar[i_n] < m)
            pblm->offset += pblm->C.at(i_n) * pblm->B.at(base->constraintOfBaseVar[i_n]).second;
    for (int i_n = 0; i_n < n; i_n++) {
        if (base->constraintOfBaseVar[i_n] < m) {
            cout << pblm->X.at(i_n).second << " is basic var" << endl;
            continue;
        }
        // lamda[i_n] = pblm->C.at(i_n);
        for (int i_m = 0; i_m < m; i_m++)  // baseVarOfConstraint[i_m]是第i_m号方程对应的基变量
            pblm->C.at(i_n) -= pblm->C.at(base->baseVarOfConstraint[i_m]) * pblm->P.at(i_n).at(i_m);
        cout << "Lamda[" << pblm->X.at(i_n).second << "] = " << pblm->C.at(i_n) << endl;
        if (pblm->C.at(i_n) >= maxLamda) {
            enterBaseVar = i_n;  // 总是选择序号大的变量入基
            maxLamda = pblm->C.at(i_n);
        }
        if (pblm->C.at(i_n) > 0)
            for (int i_m = 0; i_m < m; i_m++) {
                if (pblm->P.at(i_n).at(i_m) > 0)
                    break;
                // 这里默认了m和n的大小关系，不应该 XXX ？？？
                if (i_m == m - 1) {
                    cout << "无有界最优解" << endl;
                    return 3;
                }
            }
    }
    for (int i_m = 0; i_m < m; i_m++)
        pblm->C.at(base->baseVarOfConstraint[i_m]) = 0;  //  基变量系数置为0
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
    for (int i_m = 0; i_m < m; i_m++)
        if (pblm->P.at(enterBaseVar).at(i_m) > 0)
            if (pblm->B.at(i_m).second / pblm->P.at(enterBaseVar).at(i_m) < minTheta) {
                minTheta = pblm->B.at(i_m).second / pblm->P.at(enterBaseVar).at(i_m);
                leaveBaseVar = base->baseVarOfConstraint[i_m];  // i_m号方程对应的基变量出基
            }
    base->enterBaseVar = enterBaseVar;
    base->leaveBaseVar = leaveBaseVar;
    cout << "当前目标函数为";
    pblm->OutputTarget();
    cout << "出基变量为" << pblm->X.at(leaveBaseVar).second << "; 入基变量为" << pblm->X.at(enterBaseVar).second << endl;
    return 0;  // 继续进行
}

void Pivot(Problem* pblm, Base* base) {
    cout << "Pivot!" << endl;
    int m = pblm->B.size(), n = pblm->X.size();
    int r = base->constraintOfBaseVar[base->leaveBaseVar];  // 出基变量所在方程 // in [0,m)
    int k = base->enterBaseVar;                             // 入基变量编号     // in [0,n)
    double Pkr = pblm->P.at(k).at(r);
    for (int i_n = 0; i_n < n; i_n++)
        pblm->P.at(i_n).at(r) /= Pkr;
    pblm->B.at(r).second /= Pkr;

    for (int i_m = 0; i_m < m; i_m++) {
        if (i_m == r)
            continue;
        double Pkm = pblm->P.at(k).at(i_m);
        for (int i_n = 0; i_n < n; i_n++)
            pblm->P.at(i_n).at(i_m) -= Pkm * pblm->P.at(i_n).at(r);
        pblm->B.at(i_m).second -= Pkm * pblm->B.at(r).second;
    }

    pblm->OutputTarget();
    pblm->OutputConstraint();

    base->constraintOfBaseVar[k] = r;                   // k号变量是r号方程的基变量
    base->baseVarOfConstraint[r] = k;                   // r号方程以k号变量为基变量
    base->constraintOfBaseVar[base->leaveBaseVar] = m;  // 出基变量不再作为任何方程的基变量
}

Problem* SimplexMethod(Problem* pblm0) {
    // 对输入的对象进行单纯形法求解，返回变形结束的单纯形法
    Problem* pblm = new Problem(*pblm0);  // 深拷贝
    if (!pblm->IsStandard())
        pblm = pblm->Standandlize();
    int m = pblm->B.size(), n = pblm->X.size();
    Base* base = Initialize(pblm);
    int result = -1;
    while (!(result = Simplex(pblm, base)))
        Pivot(pblm, base);
    cout << "End!" << endl;
    if (result == 1) {
        pblm->OutputTarget();
        cout << "result is " << pblm->offset << " ";
        cout << ", got at (";
        for (int i_n = 0; i_n < n; i_n++) {
            cout << pblm->X.at(i_n).second << "=";
            if (base->constraintOfBaseVar[i_n] >= 0 && base->constraintOfBaseVar[i_n] < m)
                cout << pblm->B.at(base->constraintOfBaseVar[i_n]).second;
            else
                cout << "0";
            if (i_n == n - 1)
                cout << ")" << endl;
            else
                cout << ", ";
        }
    }
    return pblm;
}
