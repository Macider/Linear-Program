/*
 * �������Թ滮����֧���編
 */

#pragma once

#include "./dual_simplex_method.h"
#include "./problem.h"
#include "./simplex_method.h"
#include <queue>
using namespace std;

// ()
struct cmp {
    bool operator()(pair<vX, tResult*> p1, pair<vX, tResult*> p2) {
        return p1.second->first < p2.second->first;  // �󶥶�
    }
};
static int upperBound;
static int lowerBound;
static queue<pair<vX, tResult*>> pblmStack;  // ջ��ֻ��Ҫ���vX
static priority_queue<pair<vX, tResult*>, vector<pair<vX, tResult*>>, cmp> pblmQueue;
static Problem* intPblm;  // һ��������

inline void OutputBound() {
    cout << "��ǰ�Ͻ�Ϊ" << upperBound << ", ��ǰ�½�Ϊ" << lowerBound << endl;
}

Problem* BranchBoundMethod(Problem* pblm0) {
    Problem* pblm = new Problem(*pblm0);  // ���
    pblm = SimplexMethod(pblm);
    tResult* rst = pblm->GetResult();
    upperBound = floor(rst->first);
    lowerBound = INT_MIN;
    intPblm = new Problem(*pblm);
    if (pblm->result != NO_SOLUTION && !IsIntResult(rst)) {
        intPblm->result = NO_SOLUTION;
        pblmQueue.push(make_pair(pblm->X, rst));
    }
    while (!pblmQueue.empty()) {
        // ����result.first�������򣬶���Ԫ�ص�result.first��Ϊ�Ͻ�
        rst = pblmQueue.top().second;
        upperBound = floor(rst->first);
        OutputBound();
        int firstNotInt = 0;
        while (IsInt(rst->second.at(firstNotInt)))
            firstNotInt++;
        cout << "��һ������������Ϊ" << firstNotInt << endl;

        vX* X_tmp;
        Problem* pblm_tmp;
        tResult* rst_tmp;

        cout << "�������ӽڵ�" << endl;
        X_tmp = new vX(pblmQueue.top().first);
        X_tmp->at(firstNotInt).rhs = make_pair(SMALL_EQUAL, floor(rst->second.at(firstNotInt)));
        pblm_tmp = new Problem(*pblm);
        pblm_tmp->X = *X_tmp;
        pblm_tmp = SimplexMethod(pblm_tmp);
        rst_tmp = pblm_tmp->GetResult();
        pblm_tmp->OutputResult();
        if (pblm_tmp->result != NO_SOLUTION) {
            if (IsIntResult(rst_tmp) && lowerBound <= rst_tmp->first) {
                lowerBound = rst_tmp->first;
                intPblm = new Problem(*pblm_tmp);
            }
            if (!IsIntResult(rst_tmp) && rst_tmp->first > lowerBound) {
                pblmQueue.push(make_pair(*X_tmp, rst_tmp));
                cout << "���ӽڵ����" << endl;
            }
        }

        cout << "�������ӽڵ�" << endl;
        X_tmp = new vX(pblmQueue.top().first);
        X_tmp->at(firstNotInt).rhs = make_pair(LARGE_EQUAL, floor(rst->second.at(firstNotInt)) + 1);
        pblm_tmp = new Problem(*pblm);
        pblm_tmp->X = *X_tmp;
        pblm_tmp = SimplexMethod(pblm_tmp);
        rst_tmp = pblm_tmp->GetResult();
        pblm_tmp->OutputResult();
        if (pblm_tmp->result != NO_SOLUTION) {
            if (IsIntResult(rst_tmp) && lowerBound <= rst_tmp->first) {
                lowerBound = rst_tmp->first;
                intPblm = new Problem(*pblm_tmp);
            }
            if (!IsIntResult(rst_tmp) && rst_tmp->first > lowerBound) {
                pblmQueue.push(make_pair(*X_tmp, rst_tmp));
                cout << "���ӽڵ����" << endl;
            }
        }

        delete pblm_tmp;
        OutputBound();
        pblmQueue.pop();
    }
    return intPblm;  // ���û�к��ʵĽ⣬�õ���Ӧ���ǳ�ʼ���⣬resultΪNO_SOLUTION
}

// ���������滮���⣬����һ��滮�����floorΪz+��ȡz-ΪINT_MIN
// ��֧�����ڲ�Ϊ�����ı��������μ���xi>=[a]+1��xi<=[a]��Լ���������
// ���磺��������������Ϊ��z+��������з��������������������Ϊz-
// ��֦�����з�֧�����С��z-���֦������z-�Ҳ������������������
// ������Ȼ���������ȣ�BFS��Ը�����
// Ӧ��ʹ�ð�����ֵ�����������ȶ��У�ʹ��z+����½���ʼ����Ϊ����Ԫ��
// �Ͻ粻����Ҫ��ʽά��
// ��Ҫ����pblmʵ����һ�����ڴ��������滮�����һ����Ϊ��ʱ����