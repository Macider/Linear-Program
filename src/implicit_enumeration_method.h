/*
 * 0-1规划，隐枚举法
 */

#pragma once

#include "./problem.h"
#include "./simplex_method.h"
using namespace std;

Problem* SortC(Problem* pblm0) {
    Problem* pblm = new Problem(*pblm0);  // 深拷贝
    pblm->X.clear();
    pblm->C.clear();
    pblm->P.clear();
    // 考虑到“交换”操作可能时间开销极大
    // 先进行排序确定位置再进行“交换”
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
    Problem* pblm = new Problem(*pblm0);  // 深拷贝
    // 需要用assert确保一些条件
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
    // arrayX 最多31位，这是问题，需要修改
    for (int arrayX = 0; arrayX < pow(2, Xsize); arrayX++) {  // arrayX的第0位对应x0
        // cout << "arrayX is " << arrayX << endl;
        // 计算当前X的目标函数值
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
        // 遍历各个约束条件，检查当前X是否符合要求
        for (int bNo = 0; bNo < Bsize; bNo++) {
            double lhs = 0;  // 左边项
            for (int xNo = 0; xNo < Xsize; xNo++) {
                int xValue = (arrayX >> xNo) & 1;
                lhs += pblm->P.at(xNo).at(bNo) * xValue;  // 第0位对应X中第0个变量
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
            // cout << "第" << bNo << "条限制不满足" << endl;
            isOK = false;
            break;
        }
        if (isOK) {
            solutionX = arrayX;
            maxValue = nowValue;
            // cout << "maxValue更新至" << maxValue << endl;
        }
    }
    for (int xNo = 0; xNo < Xsize; xNo++) {
        int xValue = (solutionX >> xNo) & 1;
        pblm->X.at(xNo).value = xValue;
    }
    // pblm->OutputPblm();
    return pblm;  // 如果没有合适的解，得到的应该是初始问题，result为NO_SOLUTION
}