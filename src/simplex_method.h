/*
 * �����η�
 */
#pragma once

#include "./problem.h"
using namespace std;

// ���ݽṹ��
class Base {
   public:
    int* baseVarOfConstraint;  // m member in [0,n)
    int* constraintOfBaseVar;  // n member in [0,m] //��Ϊ������
    int enterBaseVar;          // ��Ҫ����ı���
    int leaveBaseVar;          // ��Ҫ�����ı���
    double offset;             // ����Ӧ��Ŀ�꺯���ĳ�����
    Base(int n, int m, double ofst) {
        this->baseVarOfConstraint = new int[m];
        fill_n(this->baseVarOfConstraint, m, -1);  // -1 = uncheck, n is error
        this->constraintOfBaseVar = new int[n];
        fill_n(this->constraintOfBaseVar, n, -1);  // -1 = uncheck, m = nonbasic
        enterBaseVar = leaveBaseVar = -1;
        this->offset = ofst;
    }
};

// ������
Base* Initialize(Problem*);           // �ҵ�һ�����
ResultType Simplex(Problem*, Base*);  // �ҵ���������ͳ�������
void Pivot(Problem*, Base*);          // ���&����
Problem* SimplexMethod(Problem*);     // �������initialize��simplex��pivot

// ������
Base* Initialize(Problem* pblm) {
    // �ҵ�һ���������P�е�����Ϊ��λ��
    cout << "Init!" << endl;
    int m = pblm->B.size(), n = pblm->X.size();
    Base* base = new Base(n, m, pblm->offset);
    for (int i_n = 0; i_n < n; i_n++)
        pblm->X.at(i_n).value = 0;

    for (int i_m = 0; i_m < m; i_m++) {
        bool flag_tmp = equals(pblm->B.at(i_m).second, 0);      // Ӧ��-x1-x2=0����������ڳ�ª
        for (int i_n = 0; i_n < n; i_n++) {           // �������б����ҵ�������
            if (base->constraintOfBaseVar[i_n] >= 0)  // ����Ϊ������/������Ϊ������
                continue;
            if (pblm->P.at(i_n).at(i_m) == 0)
                continue;
            if (pblm->P.at(i_n).at(i_m) != 1 && !flag_tmp) {  // �ñ���������Ϊ�κ�Լ�������Ļ�����
                base->constraintOfBaseVar[i_n] = m;
                continue;
            }
            for (int j_m = i_m + 1; j_m < m; j_m++)
                if (pblm->P.at(i_n).at(j_m) != 0) {      // ����������Pij��Ϊ0
                    base->constraintOfBaseVar[i_n] = m;  // �ñ���������Ϊ�κ�Լ�������Ļ�����
                    break;
                }
            if (base->constraintOfBaseVar[i_n] == -1) {
                base->constraintOfBaseVar[i_n] = i_m;
                base->baseVarOfConstraint[i_m] = i_n;
                if (flag_tmp) {
                    double Pij = pblm->P.at(i_n).at(i_m);
                    for (int j_n = 0; j_n < n; j_n++)
                        pblm->P.at(j_n).at(i_m) /= Pij;
                }
                pblm->X.at(i_n).value = pblm->B.at(i_m).second;  // �޸�X��ȡֵ
            }
        }
    }
    int baseLack = 0;
    Problem* auxiliaryPblm = new Problem(*pblm);
    for (vValue::iterator itc_ = auxiliaryPblm->C.begin(); itc_ != auxiliaryPblm->C.end(); ++itc_)
        (*itc_) = 0;
    for (int i_m = 0; i_m < m; i_m++) {
        if (base->baseVarOfConstraint[i_m] != -1)  // ��Լ�����ҵ���Ӧ�Ļ�����
            continue;
        baseLack++;
        tRightSide rhs(LARGE_EQUAL, 0);
        tVar var("__auxiliary_" + to_string(i_m) + "__", rhs, 0.0);
        auxiliaryPblm->X.push_back(var);
        auxiliaryPblm->C.push_back(-1);
        auxiliaryPblm->P.push_back(vector<double>(auxiliaryPblm->B.size(), 0.0));
        auxiliaryPblm->P.back().at(i_m) = 1;
    }
    if (baseLack) {
        // cout << "��������������ʾ" << endl;
        // auxiliaryPblm->OutputPblm();
        cout << "��ʼ�⸨������" << endl;
        auxiliaryPblm = SimplexMethod(auxiliaryPblm);
        // cout << "��������������Ϊ" << endl;
        // auxiliaryPblm->OutputPblm();
        // auxiliaryPblm->OutputResult();
        // �Ƿ������޽�����������
        if (auxiliaryPblm->result != NO_SOLUTION) {
            vP::iterator itp = pblm->P.begin();
            vP::const_iterator itp_ = auxiliaryPblm->P.cbegin();
            for (; itp != pblm->P.end() && itp_ != auxiliaryPblm->P.cend(); ++itp, ++itp_)
                (*itp) = (*itp_);
            vB::iterator itb = pblm->B.begin();
            vB::const_iterator itb_ = auxiliaryPblm->B.cbegin();
            for (; itb != pblm->B.end() && itb_ != auxiliaryPblm->B.cend(); ++itb, ++itb_)
                (*itb) = (*itb_);
            vX::iterator itx = pblm->X.begin();
            vX::const_iterator itx_ = auxiliaryPblm->X.cbegin();
            for (; itx != pblm->X.end() && itx_ != auxiliaryPblm->X.cend(); ++itx, ++itx_)
                (*itx) = (*itx_);
        }
        cout << "ԭ�����ת��Ϊ" << endl;
        pblm->OutputTarget();
        // pblm->OutputConstraint();
        // pblm->OutputResult();
        // ����ʱ��X�����Ϸ���Լ������˵���������޽�
        if (!pblm->TestConstraint()) {
            cout << "No solution" << endl;
            pblm->result = NO_SOLUTION;
            return base;
        }
        base = Initialize(pblm);
    }
    // cout << "Base has been Gotten!" << endl;
    delete auxiliaryPblm;
    pblm->result = UNKNOWN;
    //pblm->OutputTarget();
    return base;
}

ResultType Simplex(Problem* pblm, Base* base) {
    // �ȶ�������ٶ�����
    cout << "Simplex!";
    int m = pblm->B.size(), n = pblm->X.size();
    double maxLamda = 0;    // Ѱ�����ļ�������Ӧ�������
    int enterBaseVar = -1;  // �������
    for (int i_n = 0; i_n < n; i_n++)
        if (base->constraintOfBaseVar[i_n] < m)
            pblm->offset += pblm->C.at(i_n) * pblm->B.at(base->constraintOfBaseVar[i_n]).second;
    for (int i_n = 0; i_n < n; i_n++) {
        if (base->constraintOfBaseVar[i_n] < m) {
            // cout << pblm->X.at(i_n).name << " is basic var" << endl;
            continue;
        }
        for (int i_m = 0; i_m < m; i_m++)  // baseVarOfConstraint[i_m]�ǵ�i_m�ŷ��̶�Ӧ�Ļ�����
            pblm->C.at(i_n) -= pblm->C.at(base->baseVarOfConstraint[i_m]) * pblm->P.at(i_n).at(i_m);
        // cout << "Lamda[" << pblm->X.at(i_n).name << "] = " << pblm->C.at(i_n) << endl;
        if (pblm->C.at(i_n) >= maxLamda) {
            enterBaseVar = i_n;  // ����ѡ����Ŵ�ı������
            maxLamda = pblm->C.at(i_n);
        }
        if (pblm->C.at(i_n) > 0)
            for (int i_m = 0; i_m < m; i_m++) {
                if (pblm->P.at(i_n).at(i_m) > 0)
                    break;
                // ����Ĭ����m��n�Ĵ�С��ϵ����Ӧ�� XXX ������
                if (i_m == m - 1) {
                    cout << "���н����Ž�" << endl;
                    return UNBOUNDED;
                }
            }
    }
    for (int i_m = 0; i_m < m; i_m++)
        pblm->C.at(base->baseVarOfConstraint[i_m]) = 0;  //  ������ϵ����Ϊ0
    if (enterBaseVar == -1) {                            // ���м�������Ϊ��
        cout << "���м�������Ϊ�����������Ž�" << endl;
        return ONE_SOLUTION;
    }
    if (maxLamda == 0) {  // ��������Ϊ0������Ϊ��������Ž�
        cout << "��������Ϊ0" << endl;
        return MAYBE_MANY;
    }
    double minTheta = DBL_MAX;
    int leaveBaseVar = -1;
    for (int i_m = 0; i_m < m; i_m++) {
        if (pblm->P.at(enterBaseVar).at(i_m) <= 0)
            continue;
        if (pblm->B.at(i_m).second / pblm->P.at(enterBaseVar).at(i_m) < minTheta) {
            minTheta = pblm->B.at(i_m).second / pblm->P.at(enterBaseVar).at(i_m);
            leaveBaseVar = base->baseVarOfConstraint[i_m];  // i_m�ŷ��̶�Ӧ�Ļ���������
        }
    }
    assert(leaveBaseVar != -1);
    base->enterBaseVar = enterBaseVar;
    base->leaveBaseVar = leaveBaseVar;

    cout << "    ��������Ϊ" << pblm->X.at(leaveBaseVar).name << "; �������Ϊ" << pblm->X.at(enterBaseVar).name << endl;
    // pblm->OutputTarget();
    return UNKNOWN;  // ��������
}

void Pivot(Problem* pblm, Base* base) {
    // �Ը�����������������������ִ��ת��(����)����
    // ����������r�У�xkΪ�����������r��k��Ԫ�ر�Ϊ1
    cout << "Pivot!";
    int m = pblm->B.size(), n = pblm->X.size();
    int r = base->constraintOfBaseVar[base->leaveBaseVar];  // �����������ڷ��� // in [0,m)
    int k = base->enterBaseVar;                             // ����������     // in [0,n)
    double Pkr = pblm->P.at(k).at(r);
    for (int i_n = 0; i_n < n; i_n++)
        pblm->P.at(i_n).at(r) /= Pkr;
    pblm->B.at(r).second /= Pkr;
    pblm->X.at(k).value += pblm->B.at(r).second;  // ����������
    pblm->X.at(base->leaveBaseVar).value = 0;     // ������������
    //pblm->OutputResult();

    for (int i_m = 0; i_m < m; i_m++) {
        if (i_m == r)
            continue;
        double Pkm = pblm->P.at(k).at(i_m);
        for (int i_n = 0; i_n < n; i_n++)
            pblm->P.at(i_n).at(i_m) -= Pkm * pblm->P.at(i_n).at(r);
        pblm->B.at(i_m).second -= Pkm * pblm->B.at(r).second;
        pblm->X.at(base->baseVarOfConstraint[i_m]).value -= Pkm * pblm->B.at(r).second;  // ����������������Ҫ��Ӧ��С
    }
    cout << "    ��ǰĿ�꺯��Ϊ";
    pblm->OutputTarget();

    base->constraintOfBaseVar[k] = r;                   // k�ű�����r�ŷ��̵Ļ�����
    base->baseVarOfConstraint[r] = k;                   // r�ŷ�����k�ű���Ϊ������
    base->constraintOfBaseVar[base->leaveBaseVar] = m;  // ��������������Ϊ�κη��̵Ļ�����
}

Problem* SimplexMethod(Problem* pblm0) {
    // ������Ķ�����е����η���⣬���ر��ν����ĵ����η�
    // ǰ��Ҫ��max��B>=0��X>=0��Լ��ȡ��
    Problem* pblm = new Problem(*pblm0);  // ���
    if (!pblm->IsStandard())
        pblm = pblm->Standardlize();
    int m = pblm->B.size(), n = pblm->X.size();

    Base* base = Initialize(pblm);
    if (pblm->result == UNKNOWN)
        while (Simplex(pblm, base) == UNKNOWN)
            Pivot(pblm, base);
    cout << "End!" << endl;
    pblm->SimplifyDouble();
    return pblm;
}
