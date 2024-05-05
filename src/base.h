/*
 * ����������ݽṹ
 */
#pragma once
#include "./problem.h"

#define NOTBASIC -2
#define UNCHECK -1
#define LACKBASE -1

class Base {  // ���Կ��Ǻϲ���Problem���У���������ɾ���еĲ���
   public:
    vector<int> baseVarOfConstraint;  // m member in [0,n)
    vector<int> constraintOfBaseVar;  // n member in [0,m) //��Ϊ������
    // �������ķ����洢����ɾ���е�ʱ�����鷳��ɾ���˵�һ�������еĶ��ñ�
    int enterBaseVar;  // ��Ҫ����ı���
    int leaveBaseVar;  // ��Ҫ�����ı���
    Base(int n, int m) {
        this->baseVarOfConstraint.assign(m, LACKBASE);
        this->constraintOfBaseVar.assign(n, UNCHECK);
        // fill_n(this->baseVarOfConstraint, m, -1);  // -1 = uncheck,
        // fill_n(this->constraintOfBaseVar, n, -1);  // -1 = uncheck, -2 = nonbasic
        enterBaseVar = leaveBaseVar = -1;
    }
};

Base* GetBase(Problem*, WorkMode = SIMPLEX);     // �ҵ�һ���
Base* newGetBase(Problem*, WorkMode = SIMPLEX);  // �ҵ�һ���
void PivotBase(Problem*, Base*, int, int);       // ��ָ�����л�Ϊ1
extern Problem* SimplexMethod(Problem*);

Base* GetBase(Problem* pblm, WorkMode mode) {
    // �ҵ�һ���������P�е�����Ϊ��λ��
    // �������ͨ�㷨�����⿼����B����С��0
    // ������BС��0�����Բ���Ѱ��minTheta�������һ��������Ϊ�µĻ���������
    cout << "GetBase!" << endl;
    int m = pblm->B.size(), n = pblm->X.size();
    Base* base = new Base(n, m);
    pblm->result = UNKNOWN;
    for (int i_n = 0; i_n < n; i_n++)
        pblm->X.at(i_n).value = 0;
    int baseLack = m;  // ���˶��ٸ�������

    // ��һ����ɨ��һ��ȷ��������
    cout << "Ѱ�һ�����Step1" << endl;
    for (int i_m = 0; i_m < m; i_m++) {
        for (int i_n = 0; i_n < n; i_n++) {                 // �������б����ҵ�������
            if (base->constraintOfBaseVar[i_n] != UNCHECK)  // ����Ϊ������/������Ϊ������
                continue;
            if (equals(pblm->P.at(i_n).at(i_m), 0))  // eleChosen��0�򲻿��ܳ�Ϊ������
                continue;
            if (!equals(pblm->P.at(i_n).at(i_m), 1)) {  // �ñ���������Ϊ�κ�Լ�������Ļ�����
                base->constraintOfBaseVar[i_n] = NOTBASIC;
                continue;
            }
            for (int j_m = 0; j_m < m; j_m++)
                if (j_m != i_m && pblm->P.at(i_n).at(j_m) != 0) {  // ����������Pij��Ϊ0
                    base->constraintOfBaseVar[i_n] = NOTBASIC;     // �ñ���������Ϊ�κ�Լ�������Ļ�����
                    break;
                }
            if (base->constraintOfBaseVar[i_n] == UNCHECK) {  // �ҵ������������if�ƺ���ȷ���ģ����Ըĳ�assert
                base->constraintOfBaseVar[i_n] = i_m;
                base->baseVarOfConstraint[i_m] = i_n;
                baseLack--;
                // cout << "���ӱ���" << i_n << "��ΪԼ��" << i_m << "�Ļ�����";
                // cout << "����ȱ��" << baseLack << "��������" << endl;
                pblm->X.at(i_n).value = pblm->B.at(i_m).second;  // �޸�X��ȡֵ
                // cout << pblm->X.at(i_n).name << "��ֵΪ" << pblm->X.at(i_n).value << endl;
                break;
            }
        }
    }
    for (int i_n = 0; i_n < n; i_n++)
        if (base->constraintOfBaseVar[i_n] == UNCHECK)
            base->constraintOfBaseVar[i_n] = NOTBASIC;
    if (!baseLack)
        return base;
    cout << "baseLack is " << baseLack << endl;

    // ��Ϊ�Ľ�����������ֱ�������ڵ����η�
    // �ڶ�����ѡ��b/a��С�ķǻ���������ת���Ϊ������
    cout << "Ѱ�һ�����Step2" << endl;
    for (int i_n = 0; i_n < n; i_n++) {  // ����i_n�ű����Ƿ������ΪĳԼ���Ļ�����
        if (!baseLack)
            break;
        // cout << "i_n is " << i_n << endl;
        if (base->constraintOfBaseVar[i_n] != NOTBASIC)
            continue;
        int colChosen = i_n;
        // ȷ�ϸñ����Ƿ������Ϊ��Լ���Ļ�����
        int rowChosen = -1;
        if (mode == SIMPLEX) {
            double minTheta = DBL_MAX;
            for (int i_m = 0; i_m < m; i_m++) {
                // cout << "   i_m is " << i_m << endl;
                // if (base->baseVarOfConstraint[i_m] != LACKBASE)
                //     continue;
                // ������Ϊ�Ǵ���ģ���Ҫ�ҵ�b/ai��С���У��Է�ֹ��ĳ��b��ɸ�������˲�������

                if (equals(pblm->P.at(i_n).at(i_m), 0))
                    continue;
                if (smallThan(pblm->P.at(i_n).at(i_m), 0) && !equals(pblm->B.at(i_m).second, 0))  // other - ax = 0��������Ƿ���Ҫ���
                    continue;
                double theta = pblm->B.at(i_m).second / pblm->P.at(i_n).at(i_m);
                if (equals(theta, 0) && (rowChosen == -1 || base->constraintOfBaseVar[rowChosen] != NOTBASIC)) {  // ��0�Ϳ���ֱ��������
                    rowChosen = i_m;
                    break;
                }
                if (theta < minTheta) {
                    minTheta = theta;
                    rowChosen = i_m;
                } else if (equals(theta, minTheta))  // ֻ������������²�����滻
                    if (rowChosen == -1 || base->constraintOfBaseVar[rowChosen] != NOTBASIC)
                        rowChosen = i_m;
                // cout << "       theta is " << theta << ", minTheta is " << minTheta << ", rowChosen is " << rowChosen << endl;
            }
            if (rowChosen == -1 || base->baseVarOfConstraint[rowChosen] != LACKBASE)
                continue;
        } else if (mode == DUAL_SIMPLEX) {
            for (int i_m = 0; i_m < m; i_m++) {
                if (base->baseVarOfConstraint[i_m] != LACKBASE)
                    continue;
                if (equals(pblm->P.at(i_n).at(i_m), 0))
                    continue;
                rowChosen = i_m;
                break;
            }
            if (rowChosen == -1)
                continue;
        }
        // cout << "row Chosen is" << rowChosen << endl;

        // ���²���ʵ��Ϊת��
        PivotBase(pblm, base, rowChosen, colChosen);

        base->baseVarOfConstraint[rowChosen] = colChosen;
        base->constraintOfBaseVar[colChosen] = rowChosen;
        baseLack--;
        // cout << "���ӱ���" << colChosen << "��ΪԼ��" << rowChosen << "�Ļ�����";
        // cout << "����ȱ��" << baseLack << "��������" << endl;
    }
    if (!baseLack)
        return base;
    cout << "baseLack is " << baseLack << endl;
    // ���ǵ���ż�����η������������ʼ��������������������ҵ��㹻�Ļ�����

    if (mode == DUAL_SIMPLEX)
        cout << "!!!!!!!!!!!!!!!!!" << endl;

    // ������������0Ԫ������������a!=0�ķǻ�������Ϊ������
    cout << "Ѱ�һ�����Step3" << endl;
    int deleteConstraint = 0;
    for (int i_m = 0; i_m < m; i_m++) {  // ��Ȼ��Լ��û�ҵ���Ӧ�Ļ�����
        // cout << "i_m is" << i_m << endl;
        // if (!baseLack)
        //     break;
        int firstNoBaseRow = -1;

        if (base->baseVarOfConstraint[i_m] != LACKBASE)  // ��Լ�����л�����
            continue;
        firstNoBaseRow = i_m;
        // cout << "firstNoBaseRow is" << firstNoBaseRow << endl;
        // ���������� sigma x = 0������ֱ�������������һ�����
        if (equals(pblm->B.at(firstNoBaseRow).second, 0)) {
            int rowChosen = firstNoBaseRow;
            int colChosen = -1;
            for (int i_n = 0; i_n < n; i_n++) {
                if (base->constraintOfBaseVar[i_n] != NOTBASIC)  // ��������Ѿ���������������
                    continue;
                if (equals(pblm->P.at(i_n).at(rowChosen), 0))  // 1e-10 x1 + 1e-10 x2 = 0��ô����
                    continue;
                colChosen = i_n;
                break;
            }
            if (colChosen == -1) {  // �������б�������ζ�ŵ�ǰ����Ϊbase+0x=0����0x=0
                // ��ǰ����������

                cout << "��ǰɾ����Լ��Ϊ";
                int index = i_m;
                int Psz = pblm->P.size();
                vB::iterator itb = pblm->B.begin() + index;
                for (int i = 0; i < Psz; ++i) {
                    if (!pblm->P.at(i).at(index))
                        continue;
                    if (i)
                        cout << " ";
                    if (pblm->P.at(i).at(index) > 0 && i)
                        cout << "+";
                    cout << pblm->P.at(i).at(index) << "*" << pblm->X.at(i).name;
                }
                if ((*itb).first == EQUAL)
                    cout << " = " << (*itb).second << endl;
                else if ((*itb).first == LARGE_EQUAL)
                    cout << " >= " << (*itb).second << endl;
                else if ((*itb).first == SMALL_EQUAL)
                    cout << " <= " << (*itb).second << endl;

                deleteConstraint++;
                pblm->B.erase(pblm->B.begin() + i_m);
                for (int i_n = 0; i_n < n; i_n++)
                    pblm->P.at(i_n).erase(pblm->P.at(i_n).begin() + i_m);
                for (int j_m = i_m + 1; j_m < m; j_m++) {
                    if (base->baseVarOfConstraint[j_m] == LACKBASE)
                        continue;
                    int baseVar4jm = base->baseVarOfConstraint[j_m];
                    base->constraintOfBaseVar[baseVar4jm]--;
                }
                base->baseVarOfConstraint.erase(base->baseVarOfConstraint.begin() + i_m);
                m--;
                baseLack--;
                i_m--;
                continue;
            }
            assert(!equals(pblm->P.at(colChosen).at(rowChosen), 0) && "line 176");
            PivotBase(pblm, base, rowChosen, colChosen);
            base->baseVarOfConstraint[rowChosen] = colChosen;
            base->constraintOfBaseVar[colChosen] = rowChosen;
            baseLack--;
            // cout << "���ӱ���" << colChosen << "��ΪԼ��" << rowChosen << "�Ļ�����";
            // cout << "����ȱ��" << baseLack << "��������" << endl;
            continue;
        }

        int secondNoBaseRow = -1;  // ��һ������
        for (int j_m = 0; j_m < m; j_m++) {
            if (j_m == firstNoBaseRow)
                continue;
            if (base->baseVarOfConstraint[j_m] != LACKBASE)
                continue;
            secondNoBaseRow = j_m;
            break;
        }
        // cout << "secondNoBaseRow is" << secondNoBaseRow << endl;
        if (secondNoBaseRow == -1) {  // ֻʣһ��û�л�������Լ���ˣ�����תstep4?
            cout << "ֻʣһ��û�л�������Լ��" << endl;
            break;
        }
        // ���������� sigma x = 0������ֱ�������������һ�����
        if (equals(pblm->B.at(secondNoBaseRow).second, 0)) {
            int rowChosen = secondNoBaseRow;
            int colChosen = -1;
            for (int i_n = 0; i_n < n; i_n++) {
                if (base->constraintOfBaseVar[i_n] != NOTBASIC)  // ��������Ѿ���������������
                    continue;
                if (equals(pblm->P.at(i_n).at(rowChosen), 0))  // 1e-10 x1 + 1e-10 x2 = 0��ô����
                    continue;
                colChosen = i_n;
                break;
            }
            if (colChosen == -1) {  // �������б�������ζ�ŵ�ǰ����Ϊbase+0x=0����0x=0
                // ��ǰ����������
                cout << "��ǰɾ����Լ��Ϊ";
                int index = i_m;
                int Psz = pblm->P.size();
                vB::iterator itb = pblm->B.begin() + index;
                for (int i = 0; i < Psz; ++i) {
                    if (!pblm->P.at(i).at(index))
                        continue;
                    if (i)
                        cout << " ";
                    if (pblm->P.at(i).at(index) > 0 && i)
                        cout << "+";
                    cout << pblm->P.at(i).at(index) << "*" << pblm->X.at(i).name;
                }
                if ((*itb).first == EQUAL)
                    cout << " = " << (*itb).second << endl;
                else if ((*itb).first == LARGE_EQUAL)
                    cout << " >= " << (*itb).second << endl;
                else if ((*itb).first == SMALL_EQUAL)
                    cout << " <= " << (*itb).second << endl;

                deleteConstraint++;
                pblm->B.erase(pblm->B.begin() + i_m);
                for (int i_n = 0; i_n < n; i_n++)
                    pblm->P.at(i_n).erase(pblm->P.at(i_n).begin() + i_m);
                for (int j_m = i_m + 1; j_m < m; j_m++) {
                    if (base->baseVarOfConstraint[j_m] == LACKBASE)
                        continue;
                    int baseVar4jm = base->baseVarOfConstraint[j_m];
                    base->constraintOfBaseVar[baseVar4jm]--;
                }
                base->baseVarOfConstraint.erase(base->baseVarOfConstraint.begin() + i_m);
                m--;
                baseLack--;
                i_m--;
                continue;
            }
            assert(!equals(pblm->P.at(colChosen).at(rowChosen), 0) && "line 176");
            PivotBase(pblm, base, rowChosen, colChosen);
            base->baseVarOfConstraint[rowChosen] = colChosen;
            base->constraintOfBaseVar[colChosen] = rowChosen;
            baseLack--;
            // cout << "���ӱ���" << colChosen << "��ΪԼ��" << rowChosen << "�Ļ�����";
            // cout << "����ȱ��" << baseLack << "��������" << endl;
            continue;
        }

        // fNBR += Multiplier*sNBR��fNBR��Bֵ����
        // cout << "��ͨ���б任ʹ�ұ����Ϊ0" << endl;
        double Multiplier = -pblm->B.at(firstNoBaseRow).second / pblm->B.at(secondNoBaseRow).second;
        pblm->B.at(firstNoBaseRow).second += Multiplier * pblm->B.at(secondNoBaseRow).second;
        // if (!equals(pblm->B.at(firstNoBaseRow).second, 0)) {
        //     cout << "firstNoBaseRow is " << pblm->B.at(firstNoBaseRow).second << endl;
        //     cout << "Multiplier is " << Multiplier << endl;
        // }
        for (int i_n = 0; i_n < n; i_n++)
            pblm->P.at(i_n).at(firstNoBaseRow) += Multiplier * pblm->P.at(i_n).at(secondNoBaseRow);
        // ��ѭ������Ϊֹ������һ��sigmaAX = 0�������������ظ�ǰ�ĵ�Ѱ�һ���������
        int rowChosen = firstNoBaseRow;
        int colChosen = -1;
        for (int i_n = 0; i_n < n; i_n++) {
            if (base->constraintOfBaseVar[i_n] != NOTBASIC)
                continue;
            if (equals(pblm->P.at(i_n).at(rowChosen), 0))  // �������б�������ζ������������ȫ��ͬ
                continue;
            colChosen = i_n;
            break;
        }
        // ��ǰԼ����û���κ�һ��������Ϊ�������ı���
        if (colChosen == -1) {
            cout << "��ǰɾ����Լ��Ϊ";
            int index = i_m;
            int Psz = pblm->P.size();
            vB::iterator itb = pblm->B.begin() + index;
            for (int i = 0; i < Psz; ++i) {
                if (!pblm->P.at(i).at(index))
                    continue;
                if (i)
                    cout << " ";
                if (pblm->P.at(i).at(index) > 0 && i)
                    cout << "+";
                cout << pblm->P.at(i).at(index) << "*" << pblm->X.at(i).name;
            }
            if ((*itb).first == EQUAL)
                cout << " = " << (*itb).second << endl;
            else if ((*itb).first == LARGE_EQUAL)
                cout << " >= " << (*itb).second << endl;
            else if ((*itb).first == SMALL_EQUAL)
                cout << " <= " << (*itb).second << endl;

            deleteConstraint++;
            cout << "����ʵ�����ظ���Լ��" << endl;
            // ��ǰ���̱��sigma 0x = 0��
            pblm->B.erase(pblm->B.begin() + i_m);
            for (int i_n = 0; i_n < n; i_n++)
                pblm->P.at(i_n).erase(pblm->P.at(i_n).begin() + i_m);
            base->baseVarOfConstraint.erase(base->baseVarOfConstraint.begin() + i_m);
            m--;
            baseLack--;
            i_m--;
            continue;
        }

        assert(mode != DUAL_SIMPLEX);  // �����϶�ż�����η�ִ�е�����ֻ������Ϊ��ɾ������Լ��
        assert(!equals(pblm->P.at(colChosen).at(rowChosen), 0) && "line 231");
        PivotBase(pblm, base, rowChosen, colChosen);
        base->baseVarOfConstraint[rowChosen] = colChosen;
        base->constraintOfBaseVar[colChosen] = rowChosen;
        baseLack--;
        // cout << "���ӱ���" << colChosen << "��ΪԼ��" << rowChosen << "�Ļ�����";
        // cout << "����ȱ��" << baseLack << "��������" << endl;
    }
    if (deleteConstraint)
        cout << "ɾ����" << deleteConstraint << "�������Լ����������ǰʣ��" << pblm->B.size() << "��Լ��������m is " << m << endl;
    if (!baseLack)
        return base;
    cout << "baseLack is " << baseLack << endl;

    // ���Ĳ���ʹ�����׶η�Ѱ��ʣ�������
    cout << "Ѱ�һ�����Step4" << endl;
    if (baseLack) {
        Problem* auxiliaryPblm = new Problem(*pblm);
        for (vValue::iterator itc_ = auxiliaryPblm->C.begin(); itc_ != auxiliaryPblm->C.end(); ++itc_)
            (*itc_) = 0;
        for (int i_m = 0; i_m < m; i_m++) {
            if (base->baseVarOfConstraint[i_m] != LACKBASE)  // ��Լ�����ҵ���Ӧ�Ļ�����
                continue;
            tRightSide rhs(LARGE_EQUAL, 0);
            tVar var("__auxiliary_" + to_string(i_m) + "__", rhs, 0.0);
            auxiliaryPblm->X.push_back(var);
            auxiliaryPblm->C.push_back(-1);
            auxiliaryPblm->P.push_back(vector<double>(m, 0.0));
            auxiliaryPblm->P.back().at(i_m) = 1;
        }
        cout << "��ʼ�⸨������" << endl;
        auxiliaryPblm = SimplexMethod(auxiliaryPblm);
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
        // cout << "ԭ�����ת��Ϊ" << endl;
        // pblm->OutputTarget();
        // pblm->OutputConstraint();
        // ����ʱ��X�����Ϸ���Լ������˵���������޽�
        // auxiliaryPblm->OutputResult();
        // if (!pblm->TestConstraint()) {
        //     cout << "No solution" << endl;
        //     pblm->result = NO_SOLUTION;
        //     return base;
        // }
        base = GetBase(pblm);
        delete auxiliaryPblm;
        pblm->result = UNKNOWN;
        return base;
    }

    return base;
}

/*
Base* newGetBase(Problem* pblm) {
    // �ҵ�һ���������P�е�����Ϊ��λ��
    cout << "GetBase!" << endl;
    int m = pblm->B.size(), n = pblm->X.size();
    Base* base = new Base(n, m);
    for (int i_n = 0; i_n < n; i_n++)
        pblm->X.at(i_n).value = 0;

    int baseLack = m;

    for (int i_m = 0; i_m < m; i_m++) {
        bool flag_tmp = equals(pblm->B.at(i_m).second, 0);  // Ӧ��-x1-x2=0����������ڳ�ª
        for (int i_n = 0; i_n < n; i_n++) {                 // �������б����ҵ�������
            if (base->constraintOfBaseVar[i_n] >= 0)        // ����Ϊ������/������Ϊ������
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
                baseLack--;
                cout << "���ӱ���" << i_n << "��ΪԼ��" << i_m << "�Ļ�����";
                cout << "����ȱ��" << baseLack << "��������" << endl;
                if (flag_tmp) {
                    double Pij = pblm->P.at(i_n).at(i_m);
                    for (int j_n = 0; j_n < n; j_n++)
                        pblm->P.at(j_n).at(i_m) /= Pij;
                }
                pblm->X.at(i_n).value = pblm->B.at(i_m).second;  // �޸�X��ȡֵ
                break;
            }
        }
    }
    for (int i_n = 0; i_n < n; i_n++)
        if (base->constraintOfBaseVar[i_n] == -1)
            base->constraintOfBaseVar[i_n] = m;

    // ��ʵ�ϸ�������ֻ��XCPĩβ����������Ԫ�أ����Կ���ֱ�Ӹ�дԭpblm��ֻ�轫C���д��棡����
    Problem* auxiliaryPblm = new Problem(*pblm);
    for (vValue::iterator itc_ = auxiliaryPblm->C.begin(); itc_ != auxiliaryPblm->C.end(); ++itc_)
        (*itc_) = 0;
    for (int i_m = 0; i_m < m; i_m++) {
        if (base->baseVarOfConstraint[i_m] != -1)  // ��Լ�����ҵ���Ӧ�Ļ�����
            continue;
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
        // cout << "ԭ�����ת��Ϊ" << endl;
        // pblm->OutputTarget();
        // pblm->OutputConstraint();
        // ����ʱ��X�����Ϸ���Լ������˵���������޽�
        if (!pblm->TestConstraint()) {
            cout << "No solution" << endl;
            pblm->result = NO_SOLUTION;
            return base;
        }
        base = newGetBase(pblm);
    }
    // cout << "Base has been Gotten!" << endl;
    delete auxiliaryPblm;
    pblm->result = UNKNOWN;
    // pblm->OutputTarget();
    return base;
}
*/

Base* newGetBase(Problem* pblm, WorkMode mode) {
    // �ҵ�һ���������P�е�����Ϊ��λ��
    // �������ͨ�㷨�����⿼����B����С��0
    // ������BС��0�����Բ���Ѱ��minTheta�������һ��������Ϊ�µĻ���������
    cout << "newGetBase!" << endl;
    int m = pblm->B.size(), n = pblm->X.size();
    Base* base = new Base(n, m);
    pblm->result = UNKNOWN;
    for (int i_n = 0; i_n < n; i_n++)
        pblm->X.at(i_n).value = 0;
    int baseLack = m;  // ���˶��ٸ�������

    // ��һ����ɨ��һ��ȷ��������
    cout << "Ѱ�һ�����Step1" << endl;
    for (int i_m = 0; i_m < m; i_m++) {
        bool bIsZero = equals(pblm->B.at(i_m).second, 0);   // Ӧ��-x1-x2=0����������ڳ�ª // ���ǵ��еڶ������������ʵ���Ͽ���ȥ��
        for (int i_n = 0; i_n < n; i_n++) {                 // �������б����ҵ�������
            if (base->constraintOfBaseVar[i_n] != UNCHECK)  // ����Ϊ������/������Ϊ������
                continue;
            if (equals(pblm->P.at(i_n).at(i_m), 0))  // eleChosen��0�򲻿��ܳ�Ϊ������
                continue;
            if (!equals(pblm->P.at(i_n).at(i_m), 1) && !bIsZero) {  // �ñ���������Ϊ�κ�Լ�������Ļ�����
                base->constraintOfBaseVar[i_n] = NOTBASIC;
                continue;
            }
            for (int j_m = 0; j_m < m; j_m++)
                if (j_m != i_m && !equals(pblm->P.at(i_n).at(j_m), 0)) {  // ����������Pij��Ϊ0
                    base->constraintOfBaseVar[i_n] = NOTBASIC;            // �ñ���������Ϊ�κ�Լ�������Ļ�����
                    break;
                }
            if (base->constraintOfBaseVar[i_n] == LACKBASE) {  // �ҵ�������
                base->constraintOfBaseVar[i_n] = i_m;
                base->baseVarOfConstraint[i_m] = i_n;
                baseLack--;

                if (bIsZero) {
                    double Pij = pblm->P.at(i_n).at(i_m);
                    for (int j_n = 0; j_n < n; j_n++)
                        pblm->P.at(j_n).at(i_m) /= Pij;
                }
                // cout << "���ӱ���" << i_n << "��ΪԼ��" << i_m << "�Ļ�����";
                // cout << "����ȱ��" << baseLack << "��������" << endl;
                pblm->X.at(i_n).value = pblm->B.at(i_m).second;  // �޸�X��ȡֵ
                // cout << pblm->X.at(i_n).name << "��ֵΪ" << pblm->X.at(i_n).value << endl;
                break;
            }
        }
    }
    for (int i_n = 0; i_n < n; i_n++)
        if (base->constraintOfBaseVar[i_n] == UNCHECK)
            base->constraintOfBaseVar[i_n] = NOTBASIC;
    if (!baseLack)
        return base;
    cout << "baseLack is " << baseLack << endl;

    if (mode == OTHER) {
        // ֻ�����Ѿ�ʹ�ù�һ�����׶η�֮��Ż����ķ�֧
        // ����ɾ����δ�ҵ���������Լ����������ЩԼ���������Ա�����Լ���������Ա��
        cout << "��ǰԼ�������������������ԣ�����Ϊ" << m - baseLack << "��������" << baseLack << "��Լ������" << endl;
        for (int i_m = 0; i_m < m; i_m++) {
            if (base->baseVarOfConstraint[i_m] != LACKBASE)
                continue;
            pblm->B.erase(pblm->B.begin() + i_m);
            for (int i_n = 0; i_n < n; i_n++)
                pblm->P.at(i_n).erase(pblm->P.at(i_n).begin() + i_m);
            // ���ں����ÿһ�У�����Ҫ�������еļ�¼
            for (int j_m = i_m + 1; j_m < m; j_m++) {
                if (base->baseVarOfConstraint[j_m] == LACKBASE)
                    continue;
                int baseVar4jm = base->baseVarOfConstraint[j_m];
                base->constraintOfBaseVar[baseVar4jm]--;
            }
            base->baseVarOfConstraint.erase(base->baseVarOfConstraint.begin() + i_m);
            m--;
            baseLack--;
            i_m--;
        }
        if (!baseLack)
            return base;
        cout << "baseLack is " << baseLack << endl;
    }

    // ���Ĳ���ʹ�����׶η�Ѱ��ʣ�������
    cout << "Ѱ�һ�����Step4" << endl;
    if (baseLack) {
        Problem* auxiliaryPblm = new Problem(*pblm);
        for (vValue::iterator itc_ = auxiliaryPblm->C.begin(); itc_ != auxiliaryPblm->C.end(); ++itc_)
            (*itc_) = 0;
        for (int i_m = 0; i_m < m; i_m++) {
            if (base->baseVarOfConstraint[i_m] != LACKBASE)  // ��Լ�����ҵ���Ӧ�Ļ�����
                continue;
            tRightSide rhs(LARGE_EQUAL, 0);
            tVar var("__auxiliary_" + to_string(i_m) + "__", rhs, 0.0);
            auxiliaryPblm->X.push_back(var);
            auxiliaryPblm->C.push_back(-1);
            auxiliaryPblm->P.push_back(vector<double>(m, 0.0));
            auxiliaryPblm->P.back().at(i_m) = 1;
        }
        cout << "��ʼ�⸨������" << endl;
        auxiliaryPblm = SimplexMethod(auxiliaryPblm);
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
        // cout << "ԭ�����ת��Ϊ" << endl;
        // pblm->OutputTarget();
        // pblm->OutputConstraint();
        // ����ʱ��X�����Ϸ���Լ������˵���������޽�
        // cout << "��������������ʽΪ" << endl;
        // auxiliaryPblm->OutputConstraint();
        // auxiliaryPblm->OutputResult();
        // if (!pblm->TestConstraint()) {
        //     cout << "No solution" << endl;
        //     pblm->result = NO_SOLUTION;
        //     return base;
        // }

        base = newGetBase(pblm, OTHER);
        delete auxiliaryPblm;
        pblm->result = UNKNOWN;
        return base;
    }

    return base;
}

void PivotBase(Problem* pblm, Base* base, int rowChosen, int colChosen) {
    int m = pblm->B.size();
    int n = pblm->X.size();
    // rowChosen *= 1 / a������1Ԫ��
    double eleChosen = pblm->P.at(colChosen).at(rowChosen);
    assert(!equals(eleChosen, 0));

    pblm->B.at(rowChosen).second /= eleChosen;
    pblm->X.at(colChosen).value = pblm->B.at(rowChosen).second;
    // cout << pblm->X.at(colChosen).name << "��ֵΪ" << pblm->X.at(colChosen).value << endl;
    if (base->baseVarOfConstraint[rowChosen] != LACKBASE)
        pblm->X.at(base->baseVarOfConstraint[rowChosen]).value = 0;  // ԭ�����л������������
    for (int j_n = 0; j_n < n; j_n++)
        pblm->P.at(j_n).at(rowChosen) /= eleChosen;
    // ���������У�i_m += rowChosen * Multiplier����������Ԫ�ر�Ϊ0
    for (int i_m = 0; i_m < m; i_m++) {
        if (i_m == rowChosen)
            continue;
        double Multiplier = -pblm->P.at(colChosen).at(i_m);
        pblm->B.at(i_m).second += Multiplier * pblm->B.at(rowChosen).second;
        for (int j_n = 0; j_n < n; j_n++)
            pblm->P.at(j_n).at(i_m) += Multiplier * pblm->P.at(j_n).at(rowChosen);
        if (base->baseVarOfConstraint[i_m] == LACKBASE)
            continue;
        pblm->X.at(base->baseVarOfConstraint[i_m]).value += Multiplier * pblm->B.at(rowChosen).second;
        // if (!equals(Multiplier * pblm->B.at(rowChosen).second, 0))
        //     cout << pblm->X.at(base->baseVarOfConstraint[i_m]).name << "������" << Multiplier * pblm->B.at(rowChosen).second << "������" << pblm->X.at(base->baseVarOfConstraint[i_m]).value << endl;
    }
}
