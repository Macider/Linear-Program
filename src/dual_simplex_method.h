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

    // 将基变量所在的方程代入目标函数，一般来说Cbase=0，只有刚成为基变量还没进行simplex的基变量的C会不为0
    // 原目标函数为z=sigma(i,CiXi)+CbXb , 代入Xb = Bj-sigma(i,-PijXi)得z=sigma(i,(Ci-CbPij)Xi)+CbBj
    for (int i_m = 0; i_m < m; i_m++) {  // 遍历所有基变量
        double Cbase = pblm->C.at(base->baseVarOfConstraint[i_m]);
        if (equals(Cbase, 0))
            continue;
        pblm->offset += Cbase * pblm->B.at(i_m).second;
        for (int i_n = 0; i_n < n; i_n++)  // 遍历所有非基变量
            pblm->C.at(i_n) -= Cbase * pblm->P.at(i_n).at(i_m);
    }

    int leaveBaseVar = -1;  // 出基变量
    double minB = 0;
    for (int i_m = 0; i_m < m; i_m++)
        if (smallEquals(pblm->B.at(i_m).second, minB)) {
            leaveBaseVar = base->baseVarOfConstraint[i_m];
            minB = pblm->B.at(i_m).second;
        }

    // 至此已决定出基变量
    // cout << "leaveBaseVar is " << leaveBaseVar << endl;
    // cout << "m is " << m << ", n is " << n << endl;
    if (leaveBaseVar == -1) {
        cout << "对偶问题所有检验数均为负，到达最优解" << endl;
        return ONE_SOLUTION;
    }
    if (equals(minB, 0)) {
        cout << "对偶问题最大检验数为0" << endl;
        return MAYBE_MANY;
    }

    // theta的实质为入基变量的增加值(theta>0)
    double minTheta = DBL_MAX;  // 寻找最小的检验数对应变量入基 //事实上因为除了一个a，并不是检验数
    int enterBaseVar = -1;
    // 遍历所有变量，找到C/A最小的一个作为入基变量
    for (int i_n = 0; i_n < n; i_n++) {
        if (base->constraintOfBaseVar[i_n] != NOTBASIC)  // 已经是基变量了，不用再考虑
            continue;
        if (largeEquals(pblm->P.at(i_n).at(base->constraintOfBaseVar[leaveBaseVar]), 0))  // Aij(即Pij)<=0的不考虑
            continue;
        assert(smallEquals(pblm->C.at(i_n), 0) && "该问题形式不符合对偶单纯形法要求，可改为使用单纯形法");
        double theta = pblm->C.at(i_n) / pblm->P.at(i_n).at(base->constraintOfBaseVar[leaveBaseVar]);
        if (smallThan(theta, minTheta)) {  // 这里把等号加上会怎么样
            minTheta = theta;
            enterBaseVar = i_n;
        }
    }

    assert(enterBaseVar != -1);
    if (enterBaseVar == -1) {
        cout << "无解" << endl;
        return NO_SOLUTION;
    }
    // cout << "入基变量将增加的值为" << minTheta << "，当前offset为" << pblm->offset << endl;
    // cout << "目标函数将增加" << minTheta * pblm->B.at(base->constraintOfBaseVar[leaveBaseVar]).second << "，预计将增加至" << pblm->offset + minTheta * pblm->B.at(base->constraintOfBaseVar[leaveBaseVar]).second << endl;  // 理论上一定要小于0
    base->enterBaseVar = enterBaseVar;
    base->leaveBaseVar = leaveBaseVar;

    // cout << "当前的\"解\"为" << endl;
    // pblm->OutputResult();
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
    // assert(CIsOK);

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
