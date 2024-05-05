/*
 * 0-1�滮����������
 */

#pragma once

#include "./problem.h"
using namespace std;

typedef vector<double> vCol;
typedef vector<vCol> Matrix2d;
// Ŀǰ���д洢��������������
// 0 means not checked, -1 means chosen, -2 means not chosen

// ������Щ���Է�װΪ�ṹ��
static int matrixSize;
static Matrix2d efficiencyMatrix;
static vector<int> numOf0InCol;
static vector<int> numOf0InRow;
static int numOfIndivial0 = 0;
static vector<bool> colChosen;  // ����ν��
static vector<bool> rowChosen;

// �����ǰЧ�ʾ���
inline void OutputMatrix2d(Matrix2d m) {
    for (int i = 0; i < matrixSize; i++) {
        for (int j = 0; j < matrixSize; j++)
            cout << efficiencyMatrix.at(j).at(i) << ' ';
        cout << endl;
    }
}

// �����㣬�������ʼ���淶��
inline static void InitMatrix(Problem* pblm) {
    int Xsize = pblm->X.size();
    int Bsize = pblm->B.size();
    matrixSize = sqrt(Xsize);
    assert(Bsize == 2 * matrixSize);
    // ������Ҫ��֤C��������

    // ����ΪC������ʽ(matrixSize*matrixSize)
    efficiencyMatrix.resize(matrixSize);
    for (int j = 0; j < matrixSize; j++) {
        efficiencyMatrix.at(j).resize(matrixSize);
        for (int i = 0; i < matrixSize; i++)
            efficiencyMatrix.at(j).at(i) = pblm->C.at(j + i * matrixSize);
    }
}

// ����һ������C������0Ԫ��
inline static void Generate0Elmnts() {
    // ��ÿһ�в���0Ԫ��
    for (int i = 0; i < matrixSize; i++) {
        double minInCol = DBL_MAX;
        for (int j = 0; j < matrixSize; j++)  // ��ÿ���ҳ���Сֵ
            minInCol = min(efficiencyMatrix.at(j).at(i), minInCol);
        if (equals(0, minInCol))
            continue;
        for (int j = 0; j < matrixSize; j++)
            efficiencyMatrix.at(j).at(i) -= minInCol;  // ÿ�в���0Ԫ��
    }
    // ��ÿһ�в���0Ԫ��
    for (int j = 0; j < matrixSize; j++) {
        double minInRow = DBL_MAX;
        for (int i = 0; i < matrixSize; i++)  // ��ÿ���ҳ���Сֵ
            minInRow = min(efficiencyMatrix.at(j).at(i), minInRow);
        if (equals(0, minInRow))
            continue;
        for (int i = 0; i < matrixSize; i++)
            efficiencyMatrix.at(j).at(i) -= minInRow;  // ÿ�в���0Ԫ��
    }
}

// �������0Ԫ��
inline static void Count0Elmnts() {
    // �������0Ԫ��
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

// �����part1��ȷ����ָ��
inline static void DeterminAssign() {
    // ����ֻ����ɹ̶���������/ֻ���ɹ̶�����ɵĹ�������ָ��
    while (1) {
        bool goOnLoop = false;  // true�����
        for (int j = 0; j < matrixSize; j++) {
            if (numOf0InCol.at(j) != 1)
                continue;
            // �ҵ��˵�j����ֻ��1��0����
            for (int i = 0; i < matrixSize; i++) {
                if (efficiencyMatrix.at(j).at(i) != 0)
                    continue;
                // �ҵ��˵�i�������0���ڵ���
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
                // ����1�е�0ȫ�������
                break;
            }
        }
        for (int i = 0; i < matrixSize; i++) {
            if (numOf0InRow.at(i) != 1)
                continue;
            // �ҵ��˵�i����ֻ��1��0����
            for (int j = 0; j < matrixSize; j++) {
                if (efficiencyMatrix.at(j).at(i) != 0)
                    continue;
                // �ҵ��˵�j�������0���ڵ���
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
                // ����1�е�0ȫ�������
                break;
            }
        }
        if (!goOnLoop)
            break;
    }
}

// �����part2����̽��ָ��
inline static void ExploreAssign() {
    // �����ж���ѡ������������ָ��
    // ��ô����ͬ�ķ�����̽��
    while (1) {
        if (numOfIndivial0 == matrixSize)
            break;
        // ���ҵ�0Ԫ�����ٵ���
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
        // ���ҵ���һ�и���0Ԫ�����ڵ����У�0���ٵ���һ��
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
        // ѡ�и�Ԫ��(ѡ�����ٵ�Ԫ��)
        numOfIndivial0++;
        efficiencyMatrix.at(min0Col).at(min0Row) = -1;
        numOf0InCol.at(min0Col)--;
        numOf0InRow.at(min0Row)--;
        // ��ȥͬ��0Ԫ��
        for (int i_ = 0; i_ < matrixSize; i_++) {
            if (efficiencyMatrix.at(min0Col).at(i_) != 0)
                continue;
            efficiencyMatrix.at(min0Col).at(i_) = -2;
            numOf0InCol.at(min0Col)--;
            numOf0InRow.at(i_)--;
        }
        // ��ȥͬ��0Ԫ��
        for (int j_ = 0; j_ < matrixSize; j_++) {
            if (efficiencyMatrix.at(j_).at(min0Row) != 0)
                continue;
            efficiencyMatrix.at(j_).at(min0Row) = -2;
            numOf0InCol.at(j_)--;
            numOf0InRow.at(min0Row)--;
        }
    }
}

// ��������ȷ��������ҵ��Ķ���Ԫ����
inline static void ChooseColnRow() {
    cout << "Step 3" << endl;
    colChosen.assign(matrixSize, false);
    rowChosen.assign(matrixSize, false);

    // ѡ������û��chosen 0����
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
        // ������ѡ�е����У�ѡ�к���not chosen 0����
        for (int i = 0; i < matrixSize; i++) {
            if (!rowChosen[i])
                continue;
            for (int j = 0; j < matrixSize; j++) {
                if (colChosen[j])  // ������ѡ�е���
                    continue;
                if (efficiencyMatrix.at(j).at(i) != -2)
                    continue;
                colChosen[j] = true;
                cout << "choose col " << j << endl;
                goOnLoop = true;
            }
        }
        // ������ѡ�е����У�ѡ�к���chosen 0����
        for (int j = 0; j < matrixSize; j++) {
            if (!colChosen[j])
                continue;
            for (int i = 0; i < matrixSize; i++) {
                if (rowChosen[i])  // ������ѡ�е���
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

// �����ģ����Ӹ���0Ԫ��
inline static void Add0Elmnts() {
    cout << "Step 4" << endl;
    double minNoLine = DBL_MAX;
    for (int j = 0; j < matrixSize; j++) {
        if (colChosen[j])  // Ҫ�����δ��
            continue;
        for (int i = 0; i < matrixSize; i++) {
            if (!rowChosen[i])  // Ҫ������Ѵ�
                continue;
            if (efficiencyMatrix.at(j).at(i) > minNoLine)  // ����ȷ����ʱ������̬��0��������
                continue;
            minNoLine = efficiencyMatrix.at(j).at(i);
        }
    }
    // ��ѡ���м�ȥ��Сֵ
    for (int j = 0; j < matrixSize; j++) {
        if (!colChosen[j])  // Ҫ������Ѵ�
            continue;
        for (int i = 0; i < matrixSize; i++)
            efficiencyMatrix.at(j).at(i) += minNoLine;
    }
    // δѡ���м�ȥ��Сֵ
    for (int i = 0; i < matrixSize; i++) {
        if (!rowChosen[i])  // Ҫ����д�
            continue;
        for (int j = 0; j < matrixSize; j++)
            efficiencyMatrix.at(j).at(i) -= minNoLine;
    }
}

Problem* HungarianMethod(Problem* pblm0) {
    Problem* pblm = new Problem(*pblm0);  // ���

    // �����㣬�������ʼ���淶��
    InitMatrix(pblm);

    // ����һ����ÿ�����в���0Ԫ��
    Generate0Elmnts();

    while (1) {
        // ���0Ԫ��
        Count0Elmnts();

        // �������һ��ȷ����ָ��
        DeterminAssign();

        while (numOfIndivial0 != matrixSize) {  // �õ�n������0Ԫ�������
            cout << "numOfIndivial0 is " << numOfIndivial0 << endl;
            // ������������̽��ָ��
            ExploreAssign();

            OutputMatrix2d(efficiencyMatrix);
            cout << endl;
            // �õ�n������0Ԫ�������
            if (numOfIndivial0 == matrixSize)
                break;
            // �����������뻮��
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
        // ���Ĳ�������0Ԫ��
        Add0Elmnts();
    }
    cout << "��Ϊ";
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

    // �õ���
    //OutputMatrix2d(efficiencyMatrix);

    return pblm;  // ���û�к��ʵĽ⣬�õ���Ӧ���ǳ�ʼ���⣬resultΪNO_SOLUTION
}

// n����(n�����Ʒ���)��n����

// 0 �� 1 �� 2.1 �� 2.2 (�� 3 �� 4) �� ���
// 3 �� 2.2
// 4 �� 2.1