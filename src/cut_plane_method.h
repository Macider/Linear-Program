/*
 * �������Թ滮����ƽ�淨
 */

#pragma once

#include <queue>
#include "./dual_simplex_method.h"
#include "./problem.h"
#include "./simplex_method.h"
using namespace std;

Problem* CutPlaneMethod(Problem* pblm0) {
    Problem* pblm = new Problem(*pblm0);  // ���
    {
        // ȷ��pblm��������
        for (vB::const_iterator itb = pblm->B.cbegin(); itb != pblm->B.cend(); ++itb) {
            assert(IsInt((*itb).second));
            int index = distance(pblm->B.cbegin(), itb);
            for (vP::const_iterator itp = pblm->P.cbegin(); itp != pblm->P.cend(); ++itp)
                assert(IsInt((*itp).at(index)));
        }
        for (vX::const_iterator itx = pblm->X.cbegin(); itx != pblm->X.cend(); ++itx)
            assert(IsInt((*itx).value));
        // ȷ��PBX����������
    }
    pblm = SimplexMethod(pblm);

    while (true) {
        pblm->OutputResult();
        tResult* rst = pblm->GetResult();
        pblm->OutputConstraint();
        if (IsIntResult(rst)){
            cout << "�ѵõ��������Ž�" << endl;
            return pblm;
        }
        pblm->result = UNKNOWN;
        int m = pblm->B.size();
        int n = pblm->X.size();
        bool goOnLoop = false;  // �ж��Ƿ����
        for (int i_m = 0; i_m < m; i_m++) {
            // ���ڽ�����ÿһ�еó��µ�Լ�����޸�pblm
            // ʵ���Ϸ��ֶ���ÿһ����������Լ���ᵼ�¿����ܴ�
            // ʵ���ϵ���Ϊ�ȼ��ڽ������������������γɵ�͹��
            // ���Ƕ�Ŀǰ��ɳڡ��ı�������Լ��
            if (IsInt(pblm->B.at(i_m).second))
                continue;
            goOnLoop = true;
            double f_tmp;  // f_tmp in (0,1)
            for (vP::iterator itp = pblm->P.begin(); itp != pblm->P.end(); ++itp) {
                if (IsInt((*itp).at(i_m)))
                    f_tmp = 0;
                else
                    f_tmp = (*itp).at(i_m) - floor((*itp).at(i_m));
                (*itp).push_back(-f_tmp);
            }
            f_tmp = pblm->B.at(i_m).second - floor(pblm->B.at(i_m).second);
            pblm->B.push_back(make_pair(EQUAL, -f_tmp));

            // ����Ϊ�ж��Ƿ��ظ������Կ���ʹ��ĳ��ɢ��ʹ������Ч
            bool alreadyUse = false;  // Լ���Ƿ��Ѵ���
            int now_m = pblm->B.size();
            // cout << "now_m is " << now_m << endl;
            for (int j_m = 0; j_m + 1 < now_m; j_m++) {
                if (!equals(pblm->B.back().second, pblm->B.at(j_m).second))
                    continue;
                for (int i_n = 0; i_n < n; i_n++) {  // ֻ��Ҫ�ȽϷ������������У�����i_n = [n,now_n)����Ӧ��������Ϊ��λ����
                    if (!equals(pblm->P.at(i_n).back(), pblm->P.at(i_n).at(j_m)))
                        break;
                    if (i_n + 1 == n) {
                        alreadyUse = true;
                        cout << "used at " << j_m << endl;
                    }
                }
                if (alreadyUse)
                    break;
            }
            if (alreadyUse) {
                pblm->B.pop_back();
                for (vP::iterator itp = pblm->P.begin(); itp != pblm->P.end(); ++itp)
                    (*itp).pop_back();
                continue;
            }

            // ÿ�¼���һ��Լ��������Ҫ����һ���ɳڱ���
            tRightSide rhs(LARGE_EQUAL, 0);
            tVar var("__flabby_" + to_string(pblm->B.size() - 1) + "__", rhs, 0.0);  // ����Ϊ���������Ա�֤����������
            pblm->X.push_back(var);
            pblm->C.push_back(0.0);
            pblm->P.push_back(vector<double>(pblm->B.size(), 0.0));
            pblm->P.back().back() = 1;
            int now_n = pblm->X.size();
            cout << "����Լ��Ϊ" << endl;
            for (int i_n = 0; i_n < now_n; i_n++) {
                if (equals(pblm->P.at(i_n).back(), 0))
                    continue;
                cout << "+" << pblm->P.at(i_n).back() << "*" << pblm->X.at(i_n).name;
            }
            cout << " = " << pblm->B.back().second << endl;
        }
        if (!goOnLoop)
            break;
        pblm = DualSimplexMethod(pblm);
    }
    cout << "û�и������������Լ��" << endl;
    return pblm;
}

// ���б�����Ϊ�������ܱ�֤�㷨��ȷ����
// ���ʣ��������ɳڱ���Ҳ��ҪΪ����
// ����ȡ��Լ��ʱ���õ��¢�ʽ��
// ��    N*x + f*x = B + b������f,b in [0,1)���Ӷ��õ��¢�ʽ��
// ��    N*x - B = b - f*x��
// ����b<1�Ң�ʽ���Ϊ��������Ȼ�Ҳ�Ϊ��������ȡֵ����
// �ʿ��Եõ���Լ��-f*x + x' = -b����x'Ϊ�Ǹ�����   // �ɳڱ�������ż�����η�
// ��õ�f*x - x'' = b��x'Ϊ�Ǹ�����               // �����η�

// ��ֹ������ȫ��ͬ������