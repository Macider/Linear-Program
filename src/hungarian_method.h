/*
 * 0-1规划，匈牙利法
 */

#pragma once

#include "./problem.h"
#include "./simplex_method.h"
using namespace std;

typedef vector<double> vCol;
typedef vector<vCol> Matrix2d;
// 目前按列存储
void OutputMatrix2d(Matrix2d m) {
    for (vCol col : m) {
        for (double d : col) {
            cout << d << " ";
        }
        cout << endl;
    }
}

Problem* HungarianMethod(Problem* pblm0) {
    Problem* pblm = new Problem(*pblm0);  // 深拷贝
    int Xsize = pblm->X.size();
    int Bsize = pblm->B.size();
    int matrixSize = sqrt(Xsize);
    assert(Bsize == 2 * matrixSize);
    // 需要保证C均大于零

    // 步骤零，将问题形式规范化

    Matrix2d efficiencyMatrix;
    // 整理为C矩阵形式(matrixSize*matrixSize)
    efficiencyMatrix.resize(matrixSize);
    for (int j = 0; j < matrixSize; j++) {
        efficiencyMatrix.at(j).resize(matrixSize);
        for (int i = 0; i < matrixSize; i++)
            efficiencyMatrix.at(j).at(i) = pblm->C.at(j * matrixSize + i);
    }

    // 步骤一，整理C矩阵
    for (int i = 0; i < matrixSize; i++) {
        double minInCol = DBL_MAX;
        for (int j = 0; j < matrixSize; j++)  // 对每列找出最小值
            minInCol = min(efficiencyMatrix.at(j).at(i), minInCol);
        if (equals(0, minInCol))
            continue;
        for (int j = 0; j < matrixSize; j++)
            efficiencyMatrix.at(j).at(i) -= minInCol;  // 每列产生0元素
    }
    for (int j = 0; j < matrixSize; j++) {
        double minInRow = DBL_MAX;
        for (int i = 0; i < matrixSize; i++)  // 对每行找出最小值
            minInRow = min(efficiencyMatrix.at(j).at(i), minInRow);
        if (equals(0, minInRow))
            continue;
        for (int i = 0; i < matrixSize; i++)
            efficiencyMatrix.at(j).at(i) -= minInRow;  // 每行产生0元素
    }

    vector<int> numOf0InCol(matrixSize, 0);
    vector<int> numOf0InRow(matrixSize, 0);
    int numOfIndivial0 = 0;

    for (int j = 0; j < matrixSize; j++)
        for (int i = 0; i < matrixSize; i++) {
            if (IsInt(efficiencyMatrix.at(j).at(i)))
                SimplifyToInt(efficiencyMatrix.at(j).at(i));
            assert(efficiencyMatrix.at(j).at(i) >= 0);
            if (efficiencyMatrix.at(j).at(i) == 0) {
                numOf0InCol.at(j)++;
                numOf0InRow.at(i)++;
            }
        }

    // 步骤二，试指派
    // 0 means not checked, -1 means chosen, -2 means not chosen
    while (1) {
        bool goOnLoop = false;  // true则继续
        for (int j = 0; j < matrixSize; j++) {
            if (numOf0InCol.at(j) != 1)
                continue;
            // 找到了第j列是只有1个0的列
            for (int i = 0; i < matrixSize; i++) {
                if (efficiencyMatrix.at(j).at(i) != 0)
                    continue;
                // 找到了第i行是这个0所在的行
                goOnLoop = true;
                numOfIndivial0++;
                efficiencyMatrix.at(j).at(i) = -1;
                numOf0InCol.at(j)--;
                numOf0InRow.at(i)--;
                for (int j_ = 0; j_ < matrixSize;j_++) {
                    if (efficiencyMatrix.at(j_).at(i) != 0)
                        continue;
                    efficiencyMatrix.at(j_).at(i) = -2;
                    numOf0InCol.at(j_)--;
                    numOf0InRow.at(i)--;
                }
                // 将这1行的0全部处理掉
                break;
            }
        }
        for (int i = 0; i < matrixSize; i++) {
            if (numOf0InRow.at(i) != 1)
                continue;
            // 找到了第i行是只有1个0的行
            for (int j = 0; j < matrixSize; j++) {
                if (efficiencyMatrix.at(j).at(i) != 0)
                    continue;
                // 找到了第j列是这个0所在的列
                goOnLoop = true;
                numOfIndivial0++;
                efficiencyMatrix.at(j).at(i) = -1;
                numOf0InCol.at(j)--;
                numOf0InRow.at(i)--;
                for (int i_ = 0; i_ < matrixSize; i_++) {
                    if (efficiencyMatrix.at(j).at(i_) != 0)
                        continue;
                    efficiencyMatrix.at(j).at(i_) = -2;
                    numOf0InCol.at(j)--;
                    numOf0InRow.at(i_)--;
                }
                // 将这1列的0全部处理掉
                break;
            }
        }
        if(!goOnLoop)
            break;
    }

    if(numOfIndivial0 < matrixSize){
        // 试指派
    }

    // 得到解

    OutputMatrix2d(efficiencyMatrix);

    return pblm;  // 如果没有合适的解，得到的应该是初始问题，result为NO_SOLUTION
}

// n件事(n条限制方程)，n个人