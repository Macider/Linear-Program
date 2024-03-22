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

// Problem* CutPlaneMethod(Problem* pblm0) {
//     Problem* pblm = new Problem(*pblm0);  // 深拷贝
//     {
//         // 确保pblm已整数化
//         for (vB::const_iterator itb = pblm->B.cbegin(); itb != pblm->B.cend(); ++itb) {
//             assert(IsInt((*itb).second));
//             int index = distance(pblm->B.cbegin(), itb);
//             for (vP::const_iterator itp = pblm->P.cbegin(); itp != pblm->P.cend(); ++itp)
//                 assert(IsInt((*itp).at(index)));
//         }
//         for (vX::const_iterator itx = pblm->X.cbegin(); itx != pblm->X.cend(); ++itx)
//             assert(IsInt((*itx).value));
//     }
//     pblm = pblm->Standardlize();
//     pblm->result = UNKNOWN;
//     while (true) {
//         Problem* pblm_tmp = SimplexMethod(pblm);  // pblm_tmp仅用于储存结果
//         pblm_tmp->OutputResult();
//         tResult* rst = pblm_tmp->GetResult();
//         // pblm_tmp->OutputResult();
//         pblm_tmp->OutputConstraint();
//         if (IsIntResult(rst))
//             return pblm_tmp;  // 无解怎么处理
//         int m = pblm->B.size();
//         bool flag = false;  // 判断是否继续
//         for (int i_m = 0; i_m < m; i_m++) {
//             // 对于解(pblm_tmp)结果的每一行得出新的约束，修改pblm
//             // cout << "i_m is " << i_m << endl;
//             if (IsInt(pblm_tmp->B.at(i_m).second))
//                 continue;
//             // cout << "new constraint!" << endl;
//             flag = true;
//             double f_tmp;  // tmp in (0,1)
//             cout << "新增的约束条件为" << endl;
//             for (vP::const_iterator itp = pblm_tmp->P.cbegin(); itp != pblm_tmp->P.cend(); ++itp) {
//                 if (IsInt((*itp).at(i_m)))
//                     f_tmp = 0;
//                 else
//                     f_tmp = (*itp).at(i_m) - floor((*itp).at(i_m));
//                 // cout << "f_tmp is " << f_tmp << endl;
//                 int index = distance(pblm_tmp->P.cbegin(), itp);
//                 pblm->P.at(index).push_back(f_tmp);
//                 if (f_tmp)
//                     cout << " +" << f_tmp << "*Var_" << index;
//             }
//             f_tmp = pblm_tmp->B.at(i_m).second - floor(pblm_tmp->B.at(i_m).second);
//             pblm->B.push_back(make_pair(LARGE_EQUAL, f_tmp));
//             cout << " >=" << f_tmp << ";" << endl;
//         }
//         if (flag == false)
//             break;
//     }
//     return pblm;
// }

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
        // 确保PBX都被整数化
    }
    pblm = SimplexMethod(pblm);

    while (true) {
        pblm->OutputResult();
        tResult* rst = pblm->GetResult();
        pblm->OutputConstraint();
        if (IsIntResult(rst))
            return pblm;
        pblm->result = UNKNOWN;
        int m = pblm->B.size();
        bool flag = false;  // 判断是否继续
        for (int i_m = 0; i_m < m; i_m++) {
            // 对于解结果的每一行得出新的约束，修改pblm
            // cout << "i_m is " << i_m << endl;
            if (IsInt(pblm->B.at(i_m).second))
                continue;
            flag = true;
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
            // 每新加入一个约束，都需要新增一个松弛变量
            tRightSide rhs(LARGE_EQUAL, 0);
            tVar var("__flabby_" + to_string(pblm->B.size() - 1) + "__", rhs, 0.0);  // 该行为新增，可以保证变量不重名
            pblm->X.push_back(var);
            pblm->C.push_back(0.0);
            pblm->P.push_back(vector(pblm->B.size(), 0.0));
            pblm->P.back().back() = 1;
            // cout << "新增约束为" << endl;
            // for (int i_n = 0; i_n < pblm->X.size(); i_n++) {
            //     if (equals(pblm->P.at(i_n).back(), 0))
            //         continue;
            //     cout << "+" << pblm->P.at(i_n).back() << "*" << pblm->X.at(i_n).name;
            // }
            // cout << " = " << pblm->B.back().second << endl;
        }
        if (flag == false)
            break;
        pblm = DualSimplexMethod(pblm);
    }
    return pblm;
}

// 所有变量均为整数才能保证算法正确运行
// 因此剩余变量与松弛变量也需要为整数
// 在求取新约束时，得到下①式：
// ①    N*x + f*x = B + b，其中f,b in [0,1)，从而得到下②式：
// ②    N*x - B = b - f*x，
// 考虑b<1且②式左侧为整数，显然右侧为整数，且取值非正
// 故可以得到新约束-f*x + x' = -b，且x'为非负整数   // 松弛变量，对偶单纯形法
// 或得到f*x - x'' = b，x'为非负整数               // 单纯形法