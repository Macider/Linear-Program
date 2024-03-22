/*
 * 整数线性规划，割平面法
 */

#pragma once
#include <queue>

#include "./branch_bound_method.h"
#include "./dual_simplex_method.h"
#include "./problem.h"
#include "./simplex_method.h"
using namespace std;

Problem* CutPlaneMethod(Problem* pblm0) {
    Problem* pblm = new Problem(*pblm0);  // 深拷贝
    {
        // 确保pblm已整数化
        for (vB::const_iterator itb = pblm->B.cbegin(); itb != pblm->B.cend(); ++itb) {
            assert(IsInt((*itb).second));
            int index = distance(pblm->B.cbegin(), itb);
            for (vP::const_iterator itp = pblm->P.cbegin(); itp != pblm->P.cend(); ++itp)
                assert(IsInt((*itp).at(index)));
        }
        for (vX::const_iterator itx = pblm->X.cbegin(); itx != pblm->X.cend(); ++itx)
            assert(IsInt((*itx).value));
    }
    pblm = pblm->Standardlize();
    pblm->result = UNKNOWN;
    while (true) {
        Problem* pblm_tmp = SimplexMethod(pblm);  // pblm_tmp仅用于储存结果
        pblm_tmp->OutputResult();
        tResult* rst = pblm_tmp->GetResult();
        // pblm_tmp->OutputResult();
        pblm_tmp->OutputConstraint();
        if (IsIntResult(rst))
            return pblm_tmp;  // 无解怎么处理
        int m = pblm->B.size();
        bool flag = false;  // 判断是否继续
        for (int i_m = 0; i_m < m; i_m++) {
            // 对于解(pblm_tmp)结果的每一行得出新的约束，修改pblm
            // cout << "i_m is " << i_m << endl;
            if (IsInt(pblm_tmp->B.at(i_m).second))
                continue;
            // cout << "new constraint!" << endl;
            flag = true;
            double f_tmp;  // tmp in (0,1)
            cout << "新增的约束条件为" << endl;
            for (vP::const_iterator itp = pblm_tmp->P.cbegin(); itp != pblm_tmp->P.cend(); ++itp) {
                if (IsInt((*itp).at(i_m)))
                    f_tmp = 0;
                else
                    f_tmp = (*itp).at(i_m) - floor((*itp).at(i_m));
                // cout << "f_tmp is " << f_tmp << endl;
                int index = distance(pblm_tmp->P.cbegin(), itp);
                pblm->P.at(index).push_back(f_tmp);
                if (f_tmp)
                    cout << " +" << f_tmp << "*" << pblm->X.at(index).name;
            }
            f_tmp = pblm_tmp->B.at(i_m).second - floor(pblm_tmp->B.at(i_m).second);
            pblm->B.push_back(make_pair(LARGE_EQUAL, f_tmp));
            cout << " >=" << f_tmp << ";" << endl;
        }
        if (flag == false)
            break;
    }
    return pblm;
}