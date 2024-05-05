/*
 * �����η�
 */
#pragma once

#include "./base.h"
#include "./problem.h"
using namespace std;

// ���ݽṹ��

// ������

ResultType Simplex(Problem*, Base*);  // �ҵ���������ͳ�������
void Pivot(Problem*, Base*);          // ���&����
Problem* SimplexMethod(Problem*);     // �������initialize��simplex��pivot

// ������

ResultType Simplex(Problem* pblm, Base* base) {
    // �ȶ�������ٶ�����
    // cout << "Simplex!";
    int m = pblm->B.size(), n = pblm->X.size();
    // cout << "m is " << m << ", n is " << n << endl;
    double maxLamda = 0;    // Ѱ�����ļ�������Ӧ�������
    int enterBaseVar = -1;  // �������
    for (int i_n = 0; i_n < n; i_n++)
        if (base->constraintOfBaseVar[i_n] != NOTBASIC)
            pblm->offset += pblm->C.at(i_n) * pblm->B.at(base->constraintOfBaseVar[i_n]).second;
    for (int i_n = 0; i_n < n; i_n++) {
        if (base->constraintOfBaseVar[i_n] != NOTBASIC)  // ����������������ÿ��ǻ�����
            continue;
        for (int i_m = 0; i_m < m; i_m++)  // baseVarOfConstraint[i_m]�ǵ�i_m�ŷ��̶�Ӧ�Ļ�����
            pblm->C.at(i_n) -= pblm->C.at(base->baseVarOfConstraint[i_m]) * pblm->P.at(i_n).at(i_m);
        // cout << "Lamda[" << pblm->X.at(i_n).name << "] = " << pblm->C.at(i_n) << endl;
        if (largeEquals(pblm->C.at(i_n), maxLamda)) {
            enterBaseVar = i_n;  // ����ѡ����Ŵ�ı������
            maxLamda = pblm->C.at(i_n);
        }
        if (largeThan(pblm->C.at(i_n), 0))
            for (int i_m = 0; i_m < m; i_m++) {
                if (largeThan(pblm->P.at(i_n).at(i_m), 0))  
                    break;
                if (i_m == m - 1) {
                    cout << "���н����Ž�" << endl;
                    return UNBOUNDED;
                }
            }
    }
    for (int i_m = 0; i_m < m; i_m++)
        pblm->C.at(base->baseVarOfConstraint[i_m]) = 0;  //  ������ϵ����Ϊ0
    // �����Ѿ����������
    // cout << "maxLamda is " << maxLamda << ", enterBaseVar is " << enterBaseVar << endl;
    // cout << "    ��ǰĿ�꺯��Ϊ";
    // pblm->OutputTarget();
    if (enterBaseVar == -1) {  // ���м�������Ϊ��
        cout << "    ���м�������Ϊ�����������Ž�" << endl;
        return ONE_SOLUTION;
    }
    if (equals(maxLamda, 0)) {  // ��������Ϊ0������Ϊ��������Ž�
        cout << "    ��������Ϊ0" << endl;
        return MAYBE_MANY;
    }
    double minTheta = DBL_MAX;
    int leaveBaseVar = -1;
    for (int i_m = 0; i_m < m; i_m++) {
        if (pblm->P.at(enterBaseVar).at(i_m) < 0 || equals(pblm->P.at(enterBaseVar).at(i_m), 0))
            continue;
        if (pblm->B.at(i_m).second / pblm->P.at(enterBaseVar).at(i_m) < minTheta) {
            minTheta = pblm->B.at(i_m).second / pblm->P.at(enterBaseVar).at(i_m);
            leaveBaseVar = base->baseVarOfConstraint[i_m];  // i_m�ŷ��̶�Ӧ�Ļ���������
        }
    }
    // cout << "minTheta is " << minTheta << ", leaveBaseVar is " << leaveBaseVar << endl;
    assert(leaveBaseVar != -1);  // ĳһ�б���ȫ��С�ڵ���0
    base->enterBaseVar = enterBaseVar;
    base->leaveBaseVar = leaveBaseVar;

    // cout << "    ��������Ϊ" << pblm->X.at(leaveBaseVar).name << "; �������Ϊ" << pblm->X.at(enterBaseVar).name << endl;
    // pblm->OutputResult();
    // assert(pblm->TestConstraint() == true);
    return UNKNOWN;  // ��������
}

void Pivot(Problem* pblm, Base* base) {
    // �Ը�����������������������ִ��ת��(����)����
    // cout << "Pivot!" << endl;
    int m = pblm->B.size(), n = pblm->X.size();
    int rowChosen = base->constraintOfBaseVar[base->leaveBaseVar];  // �����������ڷ��� // in [0,m)
    int colChosen = base->enterBaseVar;                             // ����������     // in [0,n)

    // ����������rowChosen�У�colChosenΪ�����������rowChosen��colChosen��Ԫ�ر�Ϊ1
    PivotBase(pblm, base, rowChosen, colChosen);

    base->constraintOfBaseVar[colChosen] = rowChosen;          // k�ű�����r�ŷ��̵Ļ�����
    base->baseVarOfConstraint[rowChosen] = colChosen;          // r�ŷ�����k�ű���Ϊ������
    base->constraintOfBaseVar[base->leaveBaseVar] = NOTBASIC;  // ��������������Ϊ�κη��̵Ļ�����
}

Base* SimplexInitialize(Problem*& pblm) {  // �������µ�pblm����ԭ�еģ������Ҫ������
    if (!pblm->IsStandard())
        pblm = pblm->Standardlize();
    // pblm->OutputPblm();
    return GetBase(pblm);
}

Problem* SimplexMethod(Problem* pblm0) {
    // ������Ķ�����е����η���⣬���ر��ν����ĵ����η�
    // ǰ��Ҫ��max��B>=0��X>=0��Լ��ȡ��
    Problem* pblm = new Problem(*pblm0);  // ���

    Base* base = SimplexInitialize(pblm);
    if (pblm->result == UNKNOWN)
        while (Simplex(pblm, base) == UNKNOWN)
            Pivot(pblm, base);
    cout << "End!" << endl;
    pblm->SimplifyDouble();
    // pblm->OutputTarget();
    // pblm->OutputConstraint();

    // cout << "����ԭʼԼ��" << endl;
    // Problem* testPblm = new Problem(*pblm0);
    // int n = testPblm->X.size();
    // for (int i_n = 0; i_n < n; i_n++) {
    //     testPblm->X.at(i_n).value = pblm->X.at(i_n).value;
    // }
    // testPblm->TestConstraint();
    // delete testPblm;

    return pblm;
}
