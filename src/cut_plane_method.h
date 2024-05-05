/*
 * 整数线性规划，割平面法
 */

#pragma once

#include <queue>
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
        // 确保PBX都被整数化
    }
    pblm = SimplexMethod(pblm);

    while (true) {
        pblm->OutputResult();
        tResult* rst = pblm->GetResult();
        pblm->OutputConstraint();
        if (IsIntResult(rst)){
            cout << "已得到整数最优解" << endl;
            return pblm;
        }
        pblm->result = UNKNOWN;
        int m = pblm->B.size();
        int n = pblm->X.size();
        bool goOnLoop = false;  // 判断是否继续
        for (int i_m = 0; i_m < m; i_m++) {
            // 对于解结果的每一行得出新的约束，修改pblm
            // 实践上发现对于每一条都生成新约束会导致开销很大
            // 实际上的行为等价于将可行域割成整数顶点形成的凸包
            // 考虑对目前最“松弛”的变量进行约束
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

            // 以下为判断是否重复，可以考虑使用某种散列使它更高效
            bool alreadyUse = false;  // 约束是否已存在
            int now_m = pblm->B.size();
            // cout << "now_m is " << now_m << endl;
            for (int j_m = 0; j_m + 1 < now_m; j_m++) {
                if (!equals(pblm->B.back().second, pblm->B.at(j_m).second))
                    continue;
                for (int i_n = 0; i_n < n; i_n++) {  // 只需要比较非新增变量的列，对于i_n = [n,now_n)，对应列向量都为单位向量
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

            // 每新加入一个约束，都需要新增一个松弛变量
            tRightSide rhs(LARGE_EQUAL, 0);
            tVar var("__flabby_" + to_string(pblm->B.size() - 1) + "__", rhs, 0.0);  // 该行为新增，可以保证变量不重名
            pblm->X.push_back(var);
            pblm->C.push_back(0.0);
            pblm->P.push_back(vector<double>(pblm->B.size(), 0.0));
            pblm->P.back().back() = 1;
            int now_n = pblm->X.size();
            cout << "新增约束为" << endl;
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
    cout << "没有更多可以新增的约束" << endl;
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

// 防止出现完全相同的两行