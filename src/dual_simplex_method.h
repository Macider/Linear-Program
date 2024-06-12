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

    // �����������ڵķ��̴���Ŀ�꺯����һ����˵Cbase=0��ֻ�иճ�Ϊ��������û����simplex�Ļ�������C�᲻Ϊ0
    // ԭĿ�꺯��Ϊz=sigma(i,CiXi)+CbXb , ����Xb = Bj-sigma(i,-PijXi)��z=sigma(i,(Ci-CbPij)Xi)+CbBj
    for (int i_m = 0; i_m < m; i_m++) {  // �������л�����
        double Cbase = pblm->C.at(base->baseVarOfConstraint[i_m]);
        if (equals(Cbase, 0))
            continue;
        pblm->offset += Cbase * pblm->B.at(i_m).second;
        for (int i_n = 0; i_n < n; i_n++)  // �������зǻ�����
            pblm->C.at(i_n) -= Cbase * pblm->P.at(i_n).at(i_m);
    }

    int leaveBaseVar = -1;  // ��������
    double minB = 0;
    for (int i_m = 0; i_m < m; i_m++)
        if (smallEquals(pblm->B.at(i_m).second, minB)) {
            leaveBaseVar = base->baseVarOfConstraint[i_m];
            minB = pblm->B.at(i_m).second;
        }

    // �����Ѿ�����������
    // cout << "leaveBaseVar is " << leaveBaseVar << endl;
    // cout << "m is " << m << ", n is " << n << endl;
    if (leaveBaseVar == -1) {
        cout << "��ż�������м�������Ϊ�����������Ž�" << endl;
        return ONE_SOLUTION;
    }
    if (equals(minB, 0)) {
        cout << "��ż������������Ϊ0" << endl;
        return MAYBE_MANY;
    }

    // theta��ʵ��Ϊ�������������ֵ(theta>0)
    double minTheta = DBL_MAX;  // Ѱ����С�ļ�������Ӧ������� //��ʵ����Ϊ����һ��a�������Ǽ�����
    int enterBaseVar = -1;
    // �������б������ҵ�C/A��С��һ����Ϊ�������
    for (int i_n = 0; i_n < n; i_n++) {
        if (base->constraintOfBaseVar[i_n] != NOTBASIC)  // �Ѿ��ǻ������ˣ������ٿ���
            continue;
        if (largeEquals(pblm->P.at(i_n).at(base->constraintOfBaseVar[leaveBaseVar]), 0))  // Aij(��Pij)<=0�Ĳ�����
            continue;
        assert(smallEquals(pblm->C.at(i_n), 0) && "��������ʽ�����϶�ż�����η�Ҫ�󣬿ɸ�Ϊʹ�õ����η�");
        double theta = pblm->C.at(i_n) / pblm->P.at(i_n).at(base->constraintOfBaseVar[leaveBaseVar]);
        if (smallThan(theta, minTheta)) {  // ����ѵȺż��ϻ���ô��
            minTheta = theta;
            enterBaseVar = i_n;
        }
    }

    assert(enterBaseVar != -1);
    if (enterBaseVar == -1) {
        cout << "�޽�" << endl;
        return NO_SOLUTION;
    }
    // cout << "������������ӵ�ֵΪ" << minTheta << "����ǰoffsetΪ" << pblm->offset << endl;
    // cout << "Ŀ�꺯��������" << minTheta * pblm->B.at(base->constraintOfBaseVar[leaveBaseVar]).second << "��Ԥ�ƽ�������" << pblm->offset + minTheta * pblm->B.at(base->constraintOfBaseVar[leaveBaseVar]).second << endl;  // ������һ��ҪС��0
    base->enterBaseVar = enterBaseVar;
    base->leaveBaseVar = leaveBaseVar;

    // cout << "��ǰ��\"��\"Ϊ" << endl;
    // pblm->OutputResult();
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
    // assert(CIsOK);

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
