/*
 * 0-1规划，匈牙利法
 */

#pragma once

#include "./problem.h"
using namespace std;

typedef vector<double> vCol;
typedef vector<vCol> Matrix2d;
// 目前按列存储，先找列再找行
// 0 means not checked, -1 means chosen, -2 means not chosen

// 以下这些可以封装为结构体
static int matrixSize;
static Matrix2d efficiencyMatrix;
static vector<int> numOf0InCol;
static vector<int> numOf0InRow;
static int numOfIndivial0 = 0;
static vector<bool> colChosen;  // 即所谓打钩
static vector<bool> rowChosen;

// 输出当前效率矩阵
inline void OutputMatrix2d(Matrix2d m) {
    for (int i = 0; i < matrixSize; i++) {
        for (int j = 0; j < matrixSize; j++)
            cout << efficiencyMatrix.at(j).at(i) << ' ';
        cout << endl;
    }
}

// 步骤零，将问题初始化规范化
inline static void InitMatrix(Problem* pblm) {
    int Xsize = pblm->X.size();
    int Bsize = pblm->B.size();
    matrixSize = sqrt(Xsize);
    assert(Bsize == 2 * matrixSize);
    // 此外需要保证C均大于零

    // 整理为C矩阵形式(matrixSize*matrixSize)
    efficiencyMatrix.resize(matrixSize);
    for (int j = 0; j < matrixSize; j++) {
        efficiencyMatrix.at(j).resize(matrixSize);
        for (int i = 0; i < matrixSize; i++)
            efficiencyMatrix.at(j).at(i) = pblm->C.at(j + i * matrixSize);
    }
}

// 步骤一，整理C矩阵获得0元素
inline static void Generate0Elmnts() {
    // 对每一列产生0元素
    for (int i = 0; i < matrixSize; i++) {
        double minInCol = DBL_MAX;
        for (int j = 0; j < matrixSize; j++)  // 对每列找出最小值
            minInCol = min(efficiencyMatrix.at(j).at(i), minInCol);
        if (equals(0, minInCol))
            continue;
        for (int j = 0; j < matrixSize; j++)
            efficiencyMatrix.at(j).at(i) -= minInCol;  // 每列产生0元素
    }
    // 对每一行产生0元素
    for (int j = 0; j < matrixSize; j++) {
        double minInRow = DBL_MAX;
        for (int i = 0; i < matrixSize; i++)  // 对每行找出最小值
            minInRow = min(efficiencyMatrix.at(j).at(i), minInRow);
        if (equals(0, minInRow))
            continue;
        for (int i = 0; i < matrixSize; i++)
            efficiencyMatrix.at(j).at(i) -= minInRow;  // 每行产生0元素
    }
}

// 清点所有0元素
inline static void Count0Elmnts() {
    // 清点所有0元素
    numOfIndivial0 = 0;
    numOf0InCol.assign(matrixSize, 0);
    numOf0InRow.assign(matrixSize, 0);
    colChosen.assign(matrixSize, false);
    rowChosen.assign(matrixSize, false);
    for (int j = 0; j < matrixSize; j++)
        for (int i = 0; i < matrixSize; i++) {
            if (IsInt(efficiencyMatrix.at(j).at(i)))
                SimplifyToInt(efficiencyMatrix.at(j).at(i));
            if (efficiencyMatrix.at(j).at(i) > 0)
                continue;
            efficiencyMatrix.at(j).at(i) = 0;
            numOf0InCol.at(j)++;
            numOf0InRow.at(i)++;
        }
}

// 步骤二part1，确定性指派
inline static void DeterminAssign() {
    // 对于只能完成固定工作的人/只能由固定人完成的工作进行指派
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
                for (int j_ = 0; j_ < matrixSize; j_++) {
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
        if (!goOnLoop)
            break;
    }
}

// 步骤二part2，试探性指派
inline static void ExploreAssign() {
    // 对于有多种选择的情况进行试指派
    // 怎么“不同的方法试探”
    while (1) {
        if (numOfIndivial0 == matrixSize)
            break;
        // 先找到0元素最少的列
        int min0Col = -1;
        int min0ColNum = INT_MAX;
        for (int j = 0; j < matrixSize; j++) {
            if (numOf0InCol[j] == 0 || numOf0InCol[j] >= min0ColNum)
                continue;
            min0Col = j;
            min0ColNum = numOf0InCol[j];
        }
        if (min0Col == -1)
            break;
        // 再找到这一列各个0元素所在的行中，0最少的那一行
        int min0Row = -1;
        int min0RowNum = INT_MAX;
        for (int i = 0; i < matrixSize; i++) {
            if (efficiencyMatrix.at(min0Col).at(i) != 0)
                continue;
            if (numOf0InRow[i] == 0 || numOf0InRow[i] >= min0RowNum)
                continue;
            min0Row = i;
            min0RowNum = numOf0InRow[i];
        }
        if (min0Row == -1)
            break;
        // 选中该元素(选择最少的元素)
        numOfIndivial0++;
        efficiencyMatrix.at(min0Col).at(min0Row) = -1;
        numOf0InCol.at(min0Col)--;
        numOf0InRow.at(min0Row)--;
        // 划去同列0元素
        for (int i_ = 0; i_ < matrixSize; i_++) {
            if (efficiencyMatrix.at(min0Col).at(i_) != 0)
                continue;
            efficiencyMatrix.at(min0Col).at(i_) = -2;
            numOf0InCol.at(min0Col)--;
            numOf0InRow.at(i_)--;
        }
        // 划去同行0元素
        for (int j_ = 0; j_ < matrixSize; j_++) {
            if (efficiencyMatrix.at(j_).at(min0Row) != 0)
                continue;
            efficiencyMatrix.at(j_).at(min0Row) = -2;
            numOf0InCol.at(j_)--;
            numOf0InRow.at(min0Row)--;
        }
    }
}

// 步骤三，确定最多能找到的独立元素数
inline static void ChooseColnRow() {
    cout << "Step 3" << endl;
    colChosen.assign(matrixSize, false);
    rowChosen.assign(matrixSize, false);

    // 选中所有没有chosen 0的行
    for (int i = 0; i < matrixSize; i++)
        for (int j = 0; j < matrixSize; j++) {
            if (efficiencyMatrix.at(j).at(i) == -1)
                break;
            if (j + 1 == matrixSize) {
                rowChosen[i] = true;
                cout << "choose row " << i << endl;
            }
        }

    cout << "rowChosen is ";
    for (int i = 0; i < matrixSize; i++)
        cout << rowChosen[i] << ' ';
    cout << "\ncolChosen is ";
    for (int j = 0; j < matrixSize; j++)
        cout << colChosen[j] << ' ';
    cout << endl;

    while (1) {
        bool goOnLoop = false;
        // 对于已选中的行中，选中含有not chosen 0的列
        for (int i = 0; i < matrixSize; i++) {
            if (!rowChosen[i])
                continue;
            for (int j = 0; j < matrixSize; j++) {
                if (colChosen[j])  // 忽略已选中的列
                    continue;
                if (efficiencyMatrix.at(j).at(i) != -2)
                    continue;
                colChosen[j] = true;
                cout << "choose col " << j << endl;
                goOnLoop = true;
            }
        }
        // 对于已选中的列中，选中含有chosen 0的行
        for (int j = 0; j < matrixSize; j++) {
            if (!colChosen[j])
                continue;
            for (int i = 0; i < matrixSize; i++) {
                if (rowChosen[i])  // 忽略已选中的行
                    continue;
                if (efficiencyMatrix.at(j).at(i) != -1)
                    continue;
                rowChosen[i] = true;
                cout << "choose row " << i << endl;
                goOnLoop = true;
            }
        }
        if (!goOnLoop)
            break;
    }
    cout << "rowChosen is ";
    for (int i = 0; i < matrixSize; i++)
        cout << rowChosen[i] << ' ';
    cout << "\ncolChosen is ";
    for (int j = 0; j < matrixSize; j++)
        cout << colChosen[j] << ' ';
    cout << endl;
}

// 步骤四，增加更多0元素
inline static void Add0Elmnts() {
    cout << "Step 4" << endl;
    double minNoLine = DBL_MAX;
    for (int j = 0; j < matrixSize; j++) {
        if (colChosen[j])  // 要求该列未打钩
            continue;
        for (int i = 0; i < matrixSize; i++) {
            if (!rowChosen[i])  // 要求该行已打钩
                continue;
            if (efficiencyMatrix.at(j).at(i) > minNoLine)  // 可以确定此时所有形态的0都被跳过
                continue;
            minNoLine = efficiencyMatrix.at(j).at(i);
        }
    }
    // 已选中列加去最小值
    for (int j = 0; j < matrixSize; j++) {
        if (!colChosen[j])  // 要求该列已打钩
            continue;
        for (int i = 0; i < matrixSize; i++)
            efficiencyMatrix.at(j).at(i) += minNoLine;
    }
    // 未选中行减去最小值
    for (int i = 0; i < matrixSize; i++) {
        if (!rowChosen[i])  // 要求该行打钩
            continue;
        for (int j = 0; j < matrixSize; j++)
            efficiencyMatrix.at(j).at(i) -= minNoLine;
    }
}

Problem* HungarianMethod(Problem* pblm0) {
    Problem* pblm = new Problem(*pblm0);  // 深拷贝

    // 步骤零，将问题初始化规范化
    InitMatrix(pblm);

    // 步骤一，在每行列中产生0元素
    Generate0Elmnts();

    while (1) {
        // 清点0元素
        Count0Elmnts();

        // 步骤二其一，确定性指派
        DeterminAssign();

        while (numOfIndivial0 != matrixSize) {  // 得到n个独立0元素则结束
            cout << "numOfIndivial0 is " << numOfIndivial0 << endl;
            // 步骤二其二，试探性指派
            ExploreAssign();

            OutputMatrix2d(efficiencyMatrix);
            cout << endl;
            // 得到n个独立0元素则结束
            if (numOfIndivial0 == matrixSize)
                break;
            // 步骤三，打钩与划线
            ChooseColnRow();
            int lineNum = 0;
            for (int ij = 0; ij < matrixSize; ij++)
                lineNum += !rowChosen[ij] + colChosen[ij];
            if (lineNum < matrixSize)
                break;
            cout << "lineNum is " << lineNum << ", while matrixSize is " << matrixSize << endl;
        }
        if (numOfIndivial0 == matrixSize)
            break;
        // 第四步，增加0元素
        Add0Elmnts();
    }
    cout << "解为";
    double resultValue = 0;
    pblm->result = ONE_SOLUTION;
    for (int i = 0; i < matrixSize; i++)
        for (int j = 0; j < matrixSize; j++) {
            if (efficiencyMatrix.at(j).at(i) != -1)
                continue;
            pblm->X.at(i * matrixSize + j).value = 1;
            resultValue += pblm->C.at(i * matrixSize + j);
            cout << pblm->X.at(i * matrixSize + j).name << " = 1, ";
        }
    cout << "\nresultValue is " << resultValue << endl;

    // 得到解
    //OutputMatrix2d(efficiencyMatrix);

    return pblm;  // 如果没有合适的解，得到的应该是初始问题，result为NO_SOLUTION
}

// n件事(n条限制方程)，n个人

// 0 → 1 → 2.1 → 2.2 (→ 3 → 4) → 结果
// 3 → 2.2
// 4 → 2.1