/*
 * ��ż�����η�
 */
// ���޽�ʱ����ô��
#pragma once

#include "./problem.h"
#include "./simplex_method.h"
using namespace std;

// ������
ResultType DualSimplex(Problem*, Base*);  // �ҵ���������ͳ�������
Problem* DualSimplexMethod(Problem*);     // �������initialize��simplex��pivot

// ������
ResultType DualSimplex(Problem* pblm, Base* base) {
    // �ȶ��������ٶ����
    // cout << "DualSimplex!" << endl;
    int m = pblm->B.size(), n = pblm->X.size();

    // double ofst = 0;
    // for (int i_m = 0; i_m < m; i_m++) {
    //     // cout << "C = " << pblm->C.at(base->baseVarOfConstraint[i_m]) << ",B = " << pblm->B.at(i_m).second << endl;
    //     ofst -= pblm->C.at(base->baseVarOfConstraint[i_m]) * pblm->B.at(i_m).second;
    // }
    // pblm->offset = ofst;

    int leaveBaseVar = -1;  // ��������
    double minB = 0;
    for (int i_m = 0; i_m < m; i_m++)
        if (smallThan(pblm->B.at(i_m).second, minB)) {
            leaveBaseVar = base->baseVarOfConstraint[i_m];
            minB = pblm->B.at(i_m).second;
        }
    if (leaveBaseVar == -1) {
        cout << "��ż�������м�������Ϊ�����������Ž�" << endl;
        return ONE_SOLUTION;
    }
    if (equals(minB, 0)) {
        cout << "��ż������������Ϊ0" << endl;
        return MAYBE_MANY;
    }
    // cout << "leaveBaseVar is " << leaveBaseVar << endl;
    // cout << "m is " << m << ", n is " << n << endl;
    double minLamda = DBL_MAX;  // Ѱ����С�ļ�������Ӧ������� //��ʵ����Ϊ����һ��a�������Ǽ�����
    int enterBaseVar = -1;
    // pblm->OutputConstraint();
    for (int i_n = 0; i_n < n; i_n++) {  // �������б������ҵ�lamda��С��һ��
        // cout << "i_n is " << i_n << endl;
        // cout << "constraintOfBaseVar[leaveBaseVar] is " << base->constraintOfBaseVar[leaveBaseVar] << endl;
        if (largeEquals(pblm->P.at(i_n).at(base->constraintOfBaseVar[leaveBaseVar]), 0))
            continue;
        double lamda = pblm->C.at(i_n);
        for (int i_m = 0; i_m < m; i_m++)
            lamda -= pblm->C.at(base->baseVarOfConstraint[i_m]) * pblm->P.at(i_n).at(i_m);
        lamda /= pblm->P.at(i_n).at(base->constraintOfBaseVar[leaveBaseVar]);
        // cout << "lamda is " << lamda << endl;
        if (smallThan(lamda, minLamda)) {       // ����ѵȺż��ϻ���ô��
            minLamda = lamda;
            enterBaseVar = i_n;
        }
    }

    assert(enterBaseVar != -1);
    if (enterBaseVar == -1) {
        cout << "�޽�" << endl;
        return NO_SOLUTION;
    }
    base->enterBaseVar = enterBaseVar;
    base->leaveBaseVar = leaveBaseVar;

    // cout << "��������Ϊ" << pblm->X.at(leaveBaseVar).name << "; �������Ϊ" << pblm->X.at(enterBaseVar).name << endl;
    return UNKNOWN;  // ��������
}

Base* DualInitialize(Problem*& pblm) {
    pblm = pblm->Standardlize(DUAL_SIMPLEX);  // ��Ҫ���������׼��

    int m = pblm->B.size();
    int n = pblm->X.size();

    bool CIsOK = true;  // Ҫ������C<=0
    for (int i_n = 0; i_n < n; i_n++) {
        if (smallEquals(pblm->C.at(i_n), 0))
            continue;
        CIsOK = false;
    }
    assert(CIsOK);

    // ����Ҫ��֤����b<0������ҵ�һ����������������c<=0��˵���㷨ֱ�ӵõ����Ž�
    // �������׼������֤һ�����ҵ�һ���

    return GetBase(pblm, DUAL_SIMPLEX);
}

Problem* DualSimplexMethod(Problem* pblm0) {
    // ������Ķ�����ж�ż�����η���⣬���ر��ν����ĵ����η�
    // ǰ��Ҫ��max������C<=0������B<0������X>=0������Լ��ȡ��
    // ����������ͨ����׼��ʵ�֣�������C<=0�Ƚϸ�������ʵ��
    Problem* pblm = new Problem(*pblm0);  // ���

    Base* base = DualInitialize(pblm);
    // cout << "DualInitialize ����������" << endl;
    // pblm->OutputPblm();
    if (pblm->result == UNKNOWN)
        while (DualSimplex(pblm, base) == UNKNOWN)
            Pivot(pblm, base);
    cout << "End!" << endl;

    cout << "����ԭʼԼ��" << endl;
    Problem* testPblm = new Problem(*pblm0);
    int n = testPblm->X.size();
    for (int i_n = 0; i_n < n; i_n++) {
        testPblm->X.at(i_n).value = pblm->X.at(i_n).value;
    }
    testPblm->TestConstraint();
    delete testPblm;

    return pblm;
}
