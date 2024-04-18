/*
 * 整数线性规划，分支定界法
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
        return p1.second->first < p2.second->first;  // 大顶堆
    }
};
static int upperBound;
static int lowerBound;
static queue<pair<vX, tResult*>> pblmStack;  // 栈中只需要存放vX
static priority_queue<pair<vX, tResult*>, vector<pair<vX, tResult*>>, cmp> pblmQueue;
static Problem* intPblm;  // 一定会有吗

inline void OutputBound() {
    cout << "当前上界为" << upperBound << ", 当前下界为" << lowerBound << endl;
}

Problem* BranchBoundMethod(Problem* pblm0) {
    Problem* pblm = new Problem(*pblm0);  // 深拷贝
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
        // 按照result.first进行排序，队首元素的result.first即为上界
        rst = pblmQueue.top().second;
        upperBound = floor(rst->first);
        OutputBound();
        int firstNotInt = 0;
        while (IsInt(rst->second.at(firstNotInt)))
            firstNotInt++;
        cout << "第一个非整数变量为" << firstNotInt << endl;

        vX* X_tmp;
        Problem* pblm_tmp;
        tResult* rst_tmp;

        cout << "访问左子节点" << endl;
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
                cout << "左子节点入队" << endl;
            }
        }

        cout << "访问右子节点" << endl;
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
                cout << "右子节点入队" << endl;
            }
        }

        delete pblm_tmp;
        OutputBound();
        pblmQueue.pop();
    }
    return intPblm;  // 如果没有合适的解，得到的应该是初始问题，result为NO_SOLUTION
}

// 对于整数规划问题，求其一般规划结果的floor为z+，取z-为INT_MIN
// 分支：对于不为整数的变量，依次加上xi>=[a]+1和xi<=[a]的约束进行求解
// 定界：求解结果中最大者作为新z+，求解结果中符合整数条件的最大者作为z-
// 剪枝：若有分支求解结果小于z-则剪枝，大于z-且不符合整数条件则继续
// 宽度优先还是深度优先？BFS相对更合适
// 应该使用按函数值结果排序的优先队列，使得z+最快下降，始终作为队首元素
// 上界不再需要显式维护
// 需要两个pblm实例，一个用于储存整数规划结果，一个作为临时变量