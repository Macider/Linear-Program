/*
 * 0-1�滮����������
 */

#pragma once

#include "./problem.h"
#include "./simplex_method.h"
using namespace std;

typedef vector<double> vCol;
typedef vector<vCol> Matrix2d;
// Ŀǰ���д洢
void OutputMatrix2d(Matrix2d m) {
    for (vCol col : m) {
        for (double d : col) {
            cout << d << " ";
        }
        cout << endl;
    }
}

Problem* HungarianMethod(Problem* pblm0) {
    Problem* pblm = new Problem(*pblm0);  // ���
    int Xsize = pblm->X.size();
    int Bsize = pblm->B.size();
    int matrixSize = sqrt(Xsize);
    assert(Bsize == 2 * matrixSize);
    // ��Ҫ��֤C��������

    // �����㣬��������ʽ�淶��

    Matrix2d efficiencyMatrix;
    // ����ΪC������ʽ(matrixSize*matrixSize)
    efficiencyMatrix.resize(matrixSize);
    for (int j = 0; j < matrixSize; j++) {
        efficiencyMatrix.at(j).resize(matrixSize);
        for (int i = 0; i < matrixSize; i++)
            efficiencyMatrix.at(j).at(i) = pblm->C.at(j * matrixSize + i);
    }

    // ����һ������C����
    for (int i = 0; i < matrixSize; i++) {
        double minInCol = DBL_MAX;
        for (int j = 0; j < matrixSize; j++)  // ��ÿ���ҳ���Сֵ
            minInCol = min(efficiencyMatrix.at(j).at(i), minInCol);
        if (equals(0, minInCol))
            continue;
        for (int j = 0; j < matrixSize; j++)
            efficiencyMatrix.at(j).at(i) -= minInCol;  // ÿ�в���0Ԫ��
    }
    for (int j = 0; j < matrixSize; j++) {
        double minInRow = DBL_MAX;
        for (int i = 0; i < matrixSize; i++)  // ��ÿ���ҳ���Сֵ
            minInRow = min(efficiencyMatrix.at(j).at(i), minInRow);
        if (equals(0, minInRow))
            continue;
        for (int i = 0; i < matrixSize; i++)
            efficiencyMatrix.at(j).at(i) -= minInRow;  // ÿ�в���0Ԫ��
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

    // ���������ָ��
    // 0 means not checked, -1 means chosen, -2 means not chosen
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
                for (int j_ = 0; j_ < matrixSize;j_++) {
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
        if(!goOnLoop)
            break;
    }

    if(numOfIndivial0 < matrixSize){
        // ��ָ��
    }

    // �õ���

    OutputMatrix2d(efficiencyMatrix);

    return pblm;  // ���û�к��ʵĽ⣬�õ���Ӧ���ǳ�ʼ���⣬resultΪNO_SOLUTION
}

// n����(n�����Ʒ���)��n����