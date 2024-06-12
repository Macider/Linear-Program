/*
 * 0-1�滮����ö�ٷ�
 */

#pragma once

#include "./problem.h"
#include "./simplex_method.h"
using namespace std;

Problem* SortC(Problem* pblm0) {
    Problem* pblm = new Problem(*pblm0);  // ���
    pblm->X.clear();
    pblm->C.clear();
    pblm->P.clear();
    // ���ǵ�����������������ʱ�俪������
    // �Ƚ�������ȷ��λ���ٽ��С�������
    vector<pair<int, int>> vec_tmp;
    int Xsize = pblm0->X.size();
    for (int index = 0; index < Xsize; index++)
        vec_tmp.push_back(make_pair(pblm0->C.at(index), index));
    sort(vec_tmp.begin(), vec_tmp.end());
    for (int i = 0; i < Xsize; i++) {
        int index = vec_tmp.at(i).second;
        pblm->X.push_back(pblm0->X.at(index));
        pblm->C.push_back(pblm0->C.at(index));
        pblm->P.push_back(pblm0->P.at(index));
    }
    return pblm;
}

Problem* ImplicitEnumerationMethod(Problem* pblm0) {
    Problem* pblm = new Problem(*pblm0);  // ���
    // ��Ҫ��assertȷ��һЩ����
    if (pblm->maxOrMin == Min) {
        for (vValue::iterator itc = pblm->C.begin(); itc != pblm->C.end(); itc++)
            *itc = -*itc;
        pblm->maxOrMin = Max;
        pblm->offset = -pblm->offset;
    }
    pblm = SortC(pblm);
    // pblm->OutputPblm();
    double maxValue = -DBL_MAX;
    int solutionX = 0;
    int Xsize = pblm->X.size();
    int Bsize = pblm->B.size();
    // arrayX ���31λ���������⣬��Ҫ�޸�
    for (int arrayX = 0; arrayX < pow(2, Xsize); arrayX++) {  // arrayX�ĵ�0λ��Ӧx0
        // cout << "arrayX is " << arrayX << endl;
        // ���㵱ǰX��Ŀ�꺯��ֵ
        double nowValue = pblm->offset;
        for (int xNo = 0; xNo < Xsize; xNo++) {
            int xValue = (arrayX >> xNo & 1);
            nowValue += pblm->C.at(xNo) * xValue;
        }
        if (nowValue < maxValue) {
            // cout << "nowValue is " << nowValue << " < maxValue is " << maxValue << endl;
            continue;
        }
        bool isOK = true;
        // ��������Լ����������鵱ǰX�Ƿ����Ҫ��
        for (int bNo = 0; bNo < Bsize; bNo++) {
            double lhs = 0;  // �����
            for (int xNo = 0; xNo < Xsize; xNo++) {
                int xValue = (arrayX >> xNo) & 1;
                lhs += pblm->P.at(xNo).at(bNo) * xValue;  // ��0λ��ӦX�е�0������
            }
            double rhs = pblm->B.at(bNo).second;
            // cout << "lhs is " << lhs << ", rhs is " << rhs << endl;
            Range relation = pblm->B.at(bNo).first;
            if (relation == LARGE_EQUAL && (equals(lhs, rhs) || lhs >= rhs))
                continue;
            if (relation == SMALL_EQUAL && (equals(lhs, rhs) || lhs <= rhs))
                continue;
            if (relation == EQUAL && equals(lhs, rhs))
                continue;
            // cout << "��" << bNo << "�����Ʋ�����" << endl;
            isOK = false;
            break;
        }
        if (isOK) {
            solutionX = arrayX;
            maxValue = nowValue;
            // cout << "maxValue������" << maxValue << endl;
        }
    }
    for (int xNo = 0; xNo < Xsize; xNo++) {
        int xValue = (solutionX >> xNo) & 1;
        pblm->X.at(xNo).value = xValue;
    }
    // pblm->OutputPblm();
    return pblm;  // ���û�к��ʵĽ⣬�õ���Ӧ���ǳ�ʼ���⣬resultΪNO_SOLUTION
}