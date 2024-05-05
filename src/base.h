/*
 * 定义基础数据结构
 */
#pragma once
#include "./problem.h"

#define NOTBASIC -2
#define UNCHECK -1
#define LACKBASE -1

class Base {  // 可以考虑合并到Problem类中，并且增加删除行的操作
   public:
    vector<int> baseVarOfConstraint;  // m member in [0,n)
    vector<int> constraintOfBaseVar;  // n member in [0,m) //互为逆运算
    // 用这样的方法存储，在删除行的时候会很麻烦，删除了第一行则所有的都得变
    int enterBaseVar;  // 将要入基的变量
    int leaveBaseVar;  // 将要出基的变量
    Base(int n, int m) {
        this->baseVarOfConstraint.assign(m, LACKBASE);
        this->constraintOfBaseVar.assign(n, UNCHECK);
        // fill_n(this->baseVarOfConstraint, m, -1);  // -1 = uncheck,
        // fill_n(this->constraintOfBaseVar, n, -1);  // -1 = uncheck, -2 = nonbasic
        enterBaseVar = leaveBaseVar = -1;
    }
};

Base* GetBase(Problem*, WorkMode = SIMPLEX);     // 找到一组基
Base* newGetBase(Problem*, WorkMode = SIMPLEX);  // 找到一组基
void PivotBase(Problem*, Base*, int, int);       // 将指定行列化为1
extern Problem* SimplexMethod(Problem*);

Base* GetBase(Problem* pblm, WorkMode mode) {
    // 找到一组基，其在P中的子阵为单位阵
    // 相比于普通算法，额外考虑了B不能小于0
    // 若允许B小于0，可以不用寻找minTheta，随便找一个变量作为新的基变量即可
    cout << "GetBase!" << endl;
    int m = pblm->B.size(), n = pblm->X.size();
    Base* base = new Base(n, m);
    pblm->result = UNKNOWN;
    for (int i_n = 0; i_n < n; i_n++)
        pblm->X.at(i_n).value = 0;
    int baseLack = m;  // 差了多少个基变量

    // 第一步，扫描一遍确定基变量
    cout << "寻找基变量Step1" << endl;
    for (int i_m = 0; i_m < m; i_m++) {
        for (int i_n = 0; i_n < n; i_n++) {                 // 遍历所有变量找到基变量
            if (base->constraintOfBaseVar[i_n] != UNCHECK)  // 已作为基变量/不会作为基变量
                continue;
            if (equals(pblm->P.at(i_n).at(i_m), 0))  // eleChosen是0则不可能成为基变量
                continue;
            if (!equals(pblm->P.at(i_n).at(i_m), 1)) {  // 该变量不会作为任何约束条件的基变量
                base->constraintOfBaseVar[i_n] = NOTBASIC;
                continue;
            }
            for (int j_m = 0; j_m < m; j_m++)
                if (j_m != i_m && pblm->P.at(i_n).at(j_m) != 0) {  // 检查该列其他Pij不为0
                    base->constraintOfBaseVar[i_n] = NOTBASIC;     // 该变量不会作为任何约束条件的基变量
                    break;
                }
            if (base->constraintOfBaseVar[i_n] == UNCHECK) {  // 找到基变量，这个if似乎是确定的，可以改成assert
                base->constraintOfBaseVar[i_n] = i_m;
                base->baseVarOfConstraint[i_m] = i_n;
                baseLack--;
                // cout << "增加变量" << i_n << "作为约束" << i_m << "的基变量";
                // cout << "，还缺少" << baseLack << "个基变量" << endl;
                pblm->X.at(i_n).value = pblm->B.at(i_m).second;  // 修改X的取值
                // cout << pblm->X.at(i_n).name << "新值为" << pblm->X.at(i_n).value << endl;
                break;
            }
        }
    }
    for (int i_n = 0; i_n < n; i_n++)
        if (base->constraintOfBaseVar[i_n] == UNCHECK)
            base->constraintOfBaseVar[i_n] = NOTBASIC;
    if (!baseLack)
        return base;
    cout << "baseLack is " << baseLack << endl;

    // 下为改进方法，可以直接适用于单纯形法
    // 第二步，选中b/a最小的非基变量进行转轴成为基变量
    cout << "寻找基变量Step2" << endl;
    for (int i_n = 0; i_n < n; i_n++) {  // 考虑i_n号变量是否可以作为某约束的基变量
        if (!baseLack)
            break;
        // cout << "i_n is " << i_n << endl;
        if (base->constraintOfBaseVar[i_n] != NOTBASIC)
            continue;
        int colChosen = i_n;
        // 确认该变量是否可以作为该约束的基变量
        int rowChosen = -1;
        if (mode == SIMPLEX) {
            double minTheta = DBL_MAX;
            for (int i_m = 0; i_m < m; i_m++) {
                // cout << "   i_m is " << i_m << endl;
                // if (base->baseVarOfConstraint[i_m] != LACKBASE)
                //     continue;
                // 以上行为是错误的，需要找到b/ai最小的行，以防止将某个b变成负数，因此不能跳过

                if (equals(pblm->P.at(i_n).at(i_m), 0))
                    continue;
                if (smallThan(pblm->P.at(i_n).at(i_m), 0) && !equals(pblm->B.at(i_m).second, 0))  // other - ax = 0这种情况是符合要求的
                    continue;
                double theta = pblm->B.at(i_m).second / pblm->P.at(i_n).at(i_m);
                if (equals(theta, 0) && (rowChosen == -1 || base->constraintOfBaseVar[rowChosen] != NOTBASIC)) {  // 有0就可以直接跳出了
                    rowChosen = i_m;
                    break;
                }
                if (theta < minTheta) {
                    minTheta = theta;
                    rowChosen = i_m;
                } else if (equals(theta, minTheta))  // 只有在这种情况下才相等替换
                    if (rowChosen == -1 || base->constraintOfBaseVar[rowChosen] != NOTBASIC)
                        rowChosen = i_m;
                // cout << "       theta is " << theta << ", minTheta is " << minTheta << ", rowChosen is " << rowChosen << endl;
            }
            if (rowChosen == -1 || base->baseVarOfConstraint[rowChosen] != LACKBASE)
                continue;
        } else if (mode == DUAL_SIMPLEX) {
            for (int i_m = 0; i_m < m; i_m++) {
                if (base->baseVarOfConstraint[i_m] != LACKBASE)
                    continue;
                if (equals(pblm->P.at(i_n).at(i_m), 0))
                    continue;
                rowChosen = i_m;
                break;
            }
            if (rowChosen == -1)
                continue;
        }
        // cout << "row Chosen is" << rowChosen << endl;

        // 以下操作实质为转轴
        PivotBase(pblm, base, rowChosen, colChosen);

        base->baseVarOfConstraint[rowChosen] = colChosen;
        base->constraintOfBaseVar[colChosen] = rowChosen;
        baseLack--;
        // cout << "增加变量" << colChosen << "作为约束" << rowChosen << "的基变量";
        // cout << "，还缺少" << baseLack << "个基变量" << endl;
    }
    if (!baseLack)
        return base;
    cout << "baseLack is " << baseLack << endl;
    // 考虑到对偶单纯形法经过了特殊初始化，理论上在上面就能找到足够的基变量

    if (mode == DUAL_SIMPLEX)
        cout << "!!!!!!!!!!!!!!!!!" << endl;

    // 第三步，产生0元素以新增任意a!=0的非基变量作为基变量
    cout << "寻找基变量Step3" << endl;
    int deleteConstraint = 0;
    for (int i_m = 0; i_m < m; i_m++) {  // 依然有约束没找到对应的基变量
        // cout << "i_m is" << i_m << endl;
        // if (!baseLack)
        //     break;
        int firstNoBaseRow = -1;

        if (base->baseVarOfConstraint[i_m] != LACKBASE)  // 该约束已有基变量
            continue;
        firstNoBaseRow = i_m;
        // cout << "firstNoBaseRow is" << firstNoBaseRow << endl;
        // 这条方程是 sigma x = 0，可以直接试着在里面找基变量
        if (equals(pblm->B.at(firstNoBaseRow).second, 0)) {
            int rowChosen = firstNoBaseRow;
            int colChosen = -1;
            for (int i_n = 0; i_n < n; i_n++) {
                if (base->constraintOfBaseVar[i_n] != NOTBASIC)  // 这个变量已经被用作基变量了
                    continue;
                if (equals(pblm->P.at(i_n).at(rowChosen), 0))  // 1e-10 x1 + 1e-10 x2 = 0怎么处理
                    continue;
                colChosen = i_n;
                break;
            }
            if (colChosen == -1) {  // 跳过所有变量，意味着当前方程为base+0x=0，或0x=0
                // 当前方程无意义

                cout << "当前删除的约束为";
                int index = i_m;
                int Psz = pblm->P.size();
                vB::iterator itb = pblm->B.begin() + index;
                for (int i = 0; i < Psz; ++i) {
                    if (!pblm->P.at(i).at(index))
                        continue;
                    if (i)
                        cout << " ";
                    if (pblm->P.at(i).at(index) > 0 && i)
                        cout << "+";
                    cout << pblm->P.at(i).at(index) << "*" << pblm->X.at(i).name;
                }
                if ((*itb).first == EQUAL)
                    cout << " = " << (*itb).second << endl;
                else if ((*itb).first == LARGE_EQUAL)
                    cout << " >= " << (*itb).second << endl;
                else if ((*itb).first == SMALL_EQUAL)
                    cout << " <= " << (*itb).second << endl;

                deleteConstraint++;
                pblm->B.erase(pblm->B.begin() + i_m);
                for (int i_n = 0; i_n < n; i_n++)
                    pblm->P.at(i_n).erase(pblm->P.at(i_n).begin() + i_m);
                for (int j_m = i_m + 1; j_m < m; j_m++) {
                    if (base->baseVarOfConstraint[j_m] == LACKBASE)
                        continue;
                    int baseVar4jm = base->baseVarOfConstraint[j_m];
                    base->constraintOfBaseVar[baseVar4jm]--;
                }
                base->baseVarOfConstraint.erase(base->baseVarOfConstraint.begin() + i_m);
                m--;
                baseLack--;
                i_m--;
                continue;
            }
            assert(!equals(pblm->P.at(colChosen).at(rowChosen), 0) && "line 176");
            PivotBase(pblm, base, rowChosen, colChosen);
            base->baseVarOfConstraint[rowChosen] = colChosen;
            base->constraintOfBaseVar[colChosen] = rowChosen;
            baseLack--;
            // cout << "增加变量" << colChosen << "作为约束" << rowChosen << "的基变量";
            // cout << "，还缺少" << baseLack << "个基变量" << endl;
            continue;
        }

        int secondNoBaseRow = -1;  // 不一定存在
        for (int j_m = 0; j_m < m; j_m++) {
            if (j_m == firstNoBaseRow)
                continue;
            if (base->baseVarOfConstraint[j_m] != LACKBASE)
                continue;
            secondNoBaseRow = j_m;
            break;
        }
        // cout << "secondNoBaseRow is" << secondNoBaseRow << endl;
        if (secondNoBaseRow == -1) {  // 只剩一条没有基变量的约束了，可以转step4?
            cout << "只剩一条没有基变量的约束" << endl;
            break;
        }
        // 这条方程是 sigma x = 0，可以直接试着在里面找基变量
        if (equals(pblm->B.at(secondNoBaseRow).second, 0)) {
            int rowChosen = secondNoBaseRow;
            int colChosen = -1;
            for (int i_n = 0; i_n < n; i_n++) {
                if (base->constraintOfBaseVar[i_n] != NOTBASIC)  // 这个变量已经被用作基变量了
                    continue;
                if (equals(pblm->P.at(i_n).at(rowChosen), 0))  // 1e-10 x1 + 1e-10 x2 = 0怎么处理
                    continue;
                colChosen = i_n;
                break;
            }
            if (colChosen == -1) {  // 跳过所有变量，意味着当前方程为base+0x=0，或0x=0
                // 当前方程无意义
                cout << "当前删除的约束为";
                int index = i_m;
                int Psz = pblm->P.size();
                vB::iterator itb = pblm->B.begin() + index;
                for (int i = 0; i < Psz; ++i) {
                    if (!pblm->P.at(i).at(index))
                        continue;
                    if (i)
                        cout << " ";
                    if (pblm->P.at(i).at(index) > 0 && i)
                        cout << "+";
                    cout << pblm->P.at(i).at(index) << "*" << pblm->X.at(i).name;
                }
                if ((*itb).first == EQUAL)
                    cout << " = " << (*itb).second << endl;
                else if ((*itb).first == LARGE_EQUAL)
                    cout << " >= " << (*itb).second << endl;
                else if ((*itb).first == SMALL_EQUAL)
                    cout << " <= " << (*itb).second << endl;

                deleteConstraint++;
                pblm->B.erase(pblm->B.begin() + i_m);
                for (int i_n = 0; i_n < n; i_n++)
                    pblm->P.at(i_n).erase(pblm->P.at(i_n).begin() + i_m);
                for (int j_m = i_m + 1; j_m < m; j_m++) {
                    if (base->baseVarOfConstraint[j_m] == LACKBASE)
                        continue;
                    int baseVar4jm = base->baseVarOfConstraint[j_m];
                    base->constraintOfBaseVar[baseVar4jm]--;
                }
                base->baseVarOfConstraint.erase(base->baseVarOfConstraint.begin() + i_m);
                m--;
                baseLack--;
                i_m--;
                continue;
            }
            assert(!equals(pblm->P.at(colChosen).at(rowChosen), 0) && "line 176");
            PivotBase(pblm, base, rowChosen, colChosen);
            base->baseVarOfConstraint[rowChosen] = colChosen;
            base->constraintOfBaseVar[colChosen] = rowChosen;
            baseLack--;
            // cout << "增加变量" << colChosen << "作为约束" << rowChosen << "的基变量";
            // cout << "，还缺少" << baseLack << "个基变量" << endl;
            continue;
        }

        // fNBR += Multiplier*sNBR，fNBR的B值归零
        // cout << "将通过行变换使右边项变为0" << endl;
        double Multiplier = -pblm->B.at(firstNoBaseRow).second / pblm->B.at(secondNoBaseRow).second;
        pblm->B.at(firstNoBaseRow).second += Multiplier * pblm->B.at(secondNoBaseRow).second;
        // if (!equals(pblm->B.at(firstNoBaseRow).second, 0)) {
        //     cout << "firstNoBaseRow is " << pblm->B.at(firstNoBaseRow).second << endl;
        //     cout << "Multiplier is " << Multiplier << endl;
        // }
        for (int i_n = 0; i_n < n; i_n++)
            pblm->P.at(i_n).at(firstNoBaseRow) += Multiplier * pblm->P.at(i_n).at(secondNoBaseRow);
        // 该循环至此为止产生了一条sigmaAX = 0，接下来就是重复前文的寻找基变量方法
        int rowChosen = firstNoBaseRow;
        int colChosen = -1;
        for (int i_n = 0; i_n < n; i_n++) {
            if (base->constraintOfBaseVar[i_n] != NOTBASIC)
                continue;
            if (equals(pblm->P.at(i_n).at(rowChosen), 0))  // 跳过所有变量，意味着两条方程完全相同
                continue;
            colChosen = i_n;
            break;
        }
        // 当前约束中没有任何一个可以作为基变量的变量
        if (colChosen == -1) {
            cout << "当前删除的约束为";
            int index = i_m;
            int Psz = pblm->P.size();
            vB::iterator itb = pblm->B.begin() + index;
            for (int i = 0; i < Psz; ++i) {
                if (!pblm->P.at(i).at(index))
                    continue;
                if (i)
                    cout << " ";
                if (pblm->P.at(i).at(index) > 0 && i)
                    cout << "+";
                cout << pblm->P.at(i).at(index) << "*" << pblm->X.at(i).name;
            }
            if ((*itb).first == EQUAL)
                cout << " = " << (*itb).second << endl;
            else if ((*itb).first == LARGE_EQUAL)
                cout << " >= " << (*itb).second << endl;
            else if ((*itb).first == SMALL_EQUAL)
                cout << " <= " << (*itb).second << endl;

            deleteConstraint++;
            cout << "存在实质上重复的约束" << endl;
            // 当前方程变成sigma 0x = 0了
            pblm->B.erase(pblm->B.begin() + i_m);
            for (int i_n = 0; i_n < n; i_n++)
                pblm->P.at(i_n).erase(pblm->P.at(i_n).begin() + i_m);
            base->baseVarOfConstraint.erase(base->baseVarOfConstraint.begin() + i_m);
            m--;
            baseLack--;
            i_m--;
            continue;
        }

        assert(mode != DUAL_SIMPLEX);  // 理论上对偶单纯形法执行第三步只可能是为了删除冗余约束
        assert(!equals(pblm->P.at(colChosen).at(rowChosen), 0) && "line 231");
        PivotBase(pblm, base, rowChosen, colChosen);
        base->baseVarOfConstraint[rowChosen] = colChosen;
        base->constraintOfBaseVar[colChosen] = rowChosen;
        baseLack--;
        // cout << "增加变量" << colChosen << "作为约束" << rowChosen << "的基变量";
        // cout << "，还缺少" << baseLack << "个基变量" << endl;
    }
    if (deleteConstraint)
        cout << "删除了" << deleteConstraint << "个冗余的约束条件，当前剩余" << pblm->B.size() << "个约束条件，m is " << m << endl;
    if (!baseLack)
        return base;
    cout << "baseLack is " << baseLack << endl;

    // 第四步，使用两阶段法寻找剩余基变量
    cout << "寻找基变量Step4" << endl;
    if (baseLack) {
        Problem* auxiliaryPblm = new Problem(*pblm);
        for (vValue::iterator itc_ = auxiliaryPblm->C.begin(); itc_ != auxiliaryPblm->C.end(); ++itc_)
            (*itc_) = 0;
        for (int i_m = 0; i_m < m; i_m++) {
            if (base->baseVarOfConstraint[i_m] != LACKBASE)  // 该约束已找到对应的基变量
                continue;
            tRightSide rhs(LARGE_EQUAL, 0);
            tVar var("__auxiliary_" + to_string(i_m) + "__", rhs, 0.0);
            auxiliaryPblm->X.push_back(var);
            auxiliaryPblm->C.push_back(-1);
            auxiliaryPblm->P.push_back(vector<double>(m, 0.0));
            auxiliaryPblm->P.back().at(i_m) = 1;
        }
        cout << "开始解辅助问题" << endl;
        auxiliaryPblm = SimplexMethod(auxiliaryPblm);
        if (auxiliaryPblm->result != NO_SOLUTION) {
            vP::iterator itp = pblm->P.begin();
            vP::const_iterator itp_ = auxiliaryPblm->P.cbegin();
            for (; itp != pblm->P.end() && itp_ != auxiliaryPblm->P.cend(); ++itp, ++itp_)
                (*itp) = (*itp_);
            vB::iterator itb = pblm->B.begin();
            vB::const_iterator itb_ = auxiliaryPblm->B.cbegin();
            for (; itb != pblm->B.end() && itb_ != auxiliaryPblm->B.cend(); ++itb, ++itb_)
                (*itb) = (*itb_);
            vX::iterator itx = pblm->X.begin();
            vX::const_iterator itx_ = auxiliaryPblm->X.cbegin();
            for (; itx != pblm->X.end() && itx_ != auxiliaryPblm->X.cend(); ++itx, ++itx_)
                (*itx) = (*itx_);
        }
        // cout << "原问题可转化为" << endl;
        // pblm->OutputTarget();
        // pblm->OutputConstraint();
        // 若此时的X不符合方程约束，则说明该问题无解
        // auxiliaryPblm->OutputResult();
        // if (!pblm->TestConstraint()) {
        //     cout << "No solution" << endl;
        //     pblm->result = NO_SOLUTION;
        //     return base;
        // }
        base = GetBase(pblm);
        delete auxiliaryPblm;
        pblm->result = UNKNOWN;
        return base;
    }

    return base;
}

/*
Base* newGetBase(Problem* pblm) {
    // 找到一组基，其在P中的子阵为单位阵
    cout << "GetBase!" << endl;
    int m = pblm->B.size(), n = pblm->X.size();
    Base* base = new Base(n, m);
    for (int i_n = 0; i_n < n; i_n++)
        pblm->X.at(i_n).value = 0;

    int baseLack = m;

    for (int i_m = 0; i_m < m; i_m++) {
        bool flag_tmp = equals(pblm->B.at(i_m).second, 0);  // 应对-x1-x2=0的情况，过于丑陋
        for (int i_n = 0; i_n < n; i_n++) {                 // 遍历所有变量找到基变量
            if (base->constraintOfBaseVar[i_n] >= 0)        // 已作为基变量/不会作为基变量
                continue;
            if (pblm->P.at(i_n).at(i_m) == 0)
                continue;
            if (pblm->P.at(i_n).at(i_m) != 1 && !flag_tmp) {  // 该变量不会作为任何约束条件的基变量
                base->constraintOfBaseVar[i_n] = m;
                continue;
            }
            for (int j_m = i_m + 1; j_m < m; j_m++)
                if (pblm->P.at(i_n).at(j_m) != 0) {      // 检查该列其他Pij不为0
                    base->constraintOfBaseVar[i_n] = m;  // 该变量不会作为任何约束条件的基变量
                    break;
                }
            if (base->constraintOfBaseVar[i_n] == -1) {
                base->constraintOfBaseVar[i_n] = i_m;
                base->baseVarOfConstraint[i_m] = i_n;
                baseLack--;
                cout << "增加变量" << i_n << "作为约束" << i_m << "的基变量";
                cout << "，还缺少" << baseLack << "个基变量" << endl;
                if (flag_tmp) {
                    double Pij = pblm->P.at(i_n).at(i_m);
                    for (int j_n = 0; j_n < n; j_n++)
                        pblm->P.at(j_n).at(i_m) /= Pij;
                }
                pblm->X.at(i_n).value = pblm->B.at(i_m).second;  // 修改X的取值
                break;
            }
        }
    }
    for (int i_n = 0; i_n < n; i_n++)
        if (base->constraintOfBaseVar[i_n] == -1)
            base->constraintOfBaseVar[i_n] = m;

    // 事实上辅助问题只在XCP末尾引入了少量元素，可以考虑直接覆写原pblm，只需将C另行储存！！！
    Problem* auxiliaryPblm = new Problem(*pblm);
    for (vValue::iterator itc_ = auxiliaryPblm->C.begin(); itc_ != auxiliaryPblm->C.end(); ++itc_)
        (*itc_) = 0;
    for (int i_m = 0; i_m < m; i_m++) {
        if (base->baseVarOfConstraint[i_m] != -1)  // 该约束已找到对应的基变量
            continue;
        tRightSide rhs(LARGE_EQUAL, 0);
        tVar var("__auxiliary_" + to_string(i_m) + "__", rhs, 0.0);
        auxiliaryPblm->X.push_back(var);
        auxiliaryPblm->C.push_back(-1);
        auxiliaryPblm->P.push_back(vector<double>(auxiliaryPblm->B.size(), 0.0));
        auxiliaryPblm->P.back().at(i_m) = 1;
    }
    if (baseLack) {
        // cout << "辅助问题如下所示" << endl;
        // auxiliaryPblm->OutputPblm();
        cout << "开始解辅助问题" << endl;
        auxiliaryPblm = SimplexMethod(auxiliaryPblm);
        // cout << "辅助问题最终型为" << endl;
        // auxiliaryPblm->OutputPblm();
        // auxiliaryPblm->OutputResult();
        if (auxiliaryPblm->result != NO_SOLUTION) {
            vP::iterator itp = pblm->P.begin();
            vP::const_iterator itp_ = auxiliaryPblm->P.cbegin();
            for (; itp != pblm->P.end() && itp_ != auxiliaryPblm->P.cend(); ++itp, ++itp_)
                (*itp) = (*itp_);
            vB::iterator itb = pblm->B.begin();
            vB::const_iterator itb_ = auxiliaryPblm->B.cbegin();
            for (; itb != pblm->B.end() && itb_ != auxiliaryPblm->B.cend(); ++itb, ++itb_)
                (*itb) = (*itb_);
            vX::iterator itx = pblm->X.begin();
            vX::const_iterator itx_ = auxiliaryPblm->X.cbegin();
            for (; itx != pblm->X.end() && itx_ != auxiliaryPblm->X.cend(); ++itx, ++itx_)
                (*itx) = (*itx_);
        }
        // cout << "原问题可转化为" << endl;
        // pblm->OutputTarget();
        // pblm->OutputConstraint();
        // 若此时的X不符合方程约束，则说明该问题无解
        if (!pblm->TestConstraint()) {
            cout << "No solution" << endl;
            pblm->result = NO_SOLUTION;
            return base;
        }
        base = newGetBase(pblm);
    }
    // cout << "Base has been Gotten!" << endl;
    delete auxiliaryPblm;
    pblm->result = UNKNOWN;
    // pblm->OutputTarget();
    return base;
}
*/

Base* newGetBase(Problem* pblm, WorkMode mode) {
    // 找到一组基，其在P中的子阵为单位阵
    // 相比于普通算法，额外考虑了B不能小于0
    // 若允许B小于0，可以不用寻找minTheta，随便找一个变量作为新的基变量即可
    cout << "newGetBase!" << endl;
    int m = pblm->B.size(), n = pblm->X.size();
    Base* base = new Base(n, m);
    pblm->result = UNKNOWN;
    for (int i_n = 0; i_n < n; i_n++)
        pblm->X.at(i_n).value = 0;
    int baseLack = m;  // 差了多少个基变量

    // 第一步，扫描一遍确定基变量
    cout << "寻找基变量Step1" << endl;
    for (int i_m = 0; i_m < m; i_m++) {
        bool bIsZero = equals(pblm->B.at(i_m).second, 0);   // 应对-x1-x2=0的情况，过于丑陋 // 考虑到有第二步，所以这个实际上可以去掉
        for (int i_n = 0; i_n < n; i_n++) {                 // 遍历所有变量找到基变量
            if (base->constraintOfBaseVar[i_n] != UNCHECK)  // 已作为基变量/不会作为基变量
                continue;
            if (equals(pblm->P.at(i_n).at(i_m), 0))  // eleChosen是0则不可能成为基变量
                continue;
            if (!equals(pblm->P.at(i_n).at(i_m), 1) && !bIsZero) {  // 该变量不会作为任何约束条件的基变量
                base->constraintOfBaseVar[i_n] = NOTBASIC;
                continue;
            }
            for (int j_m = 0; j_m < m; j_m++)
                if (j_m != i_m && !equals(pblm->P.at(i_n).at(j_m), 0)) {  // 检查该列其他Pij不为0
                    base->constraintOfBaseVar[i_n] = NOTBASIC;            // 该变量不会作为任何约束条件的基变量
                    break;
                }
            if (base->constraintOfBaseVar[i_n] == LACKBASE) {  // 找到基变量
                base->constraintOfBaseVar[i_n] = i_m;
                base->baseVarOfConstraint[i_m] = i_n;
                baseLack--;

                if (bIsZero) {
                    double Pij = pblm->P.at(i_n).at(i_m);
                    for (int j_n = 0; j_n < n; j_n++)
                        pblm->P.at(j_n).at(i_m) /= Pij;
                }
                // cout << "增加变量" << i_n << "作为约束" << i_m << "的基变量";
                // cout << "，还缺少" << baseLack << "个基变量" << endl;
                pblm->X.at(i_n).value = pblm->B.at(i_m).second;  // 修改X的取值
                // cout << pblm->X.at(i_n).name << "新值为" << pblm->X.at(i_n).value << endl;
                break;
            }
        }
    }
    for (int i_n = 0; i_n < n; i_n++)
        if (base->constraintOfBaseVar[i_n] == UNCHECK)
            base->constraintOfBaseVar[i_n] = NOTBASIC;
    if (!baseLack)
        return base;
    cout << "baseLack is " << baseLack << endl;

    if (mode == OTHER) {
        // 只有在已经使用过一次两阶段法之后才会进入的分支
        // 用于删除仍未找到基变量的约束条件，这些约束条件可以被其他约束条件线性表出
        cout << "当前约束方程组具有线性相关性，其秩为" << m - baseLack << "，多余了" << baseLack << "个约束条件" << endl;
        for (int i_m = 0; i_m < m; i_m++) {
            if (base->baseVarOfConstraint[i_m] != LACKBASE)
                continue;
            pblm->B.erase(pblm->B.begin() + i_m);
            for (int i_n = 0; i_n < n; i_n++)
                pblm->P.at(i_n).erase(pblm->P.at(i_n).begin() + i_m);
            // 对于后面的每一行，都需要调整基中的记录
            for (int j_m = i_m + 1; j_m < m; j_m++) {
                if (base->baseVarOfConstraint[j_m] == LACKBASE)
                    continue;
                int baseVar4jm = base->baseVarOfConstraint[j_m];
                base->constraintOfBaseVar[baseVar4jm]--;
            }
            base->baseVarOfConstraint.erase(base->baseVarOfConstraint.begin() + i_m);
            m--;
            baseLack--;
            i_m--;
        }
        if (!baseLack)
            return base;
        cout << "baseLack is " << baseLack << endl;
    }

    // 第四步，使用两阶段法寻找剩余基变量
    cout << "寻找基变量Step4" << endl;
    if (baseLack) {
        Problem* auxiliaryPblm = new Problem(*pblm);
        for (vValue::iterator itc_ = auxiliaryPblm->C.begin(); itc_ != auxiliaryPblm->C.end(); ++itc_)
            (*itc_) = 0;
        for (int i_m = 0; i_m < m; i_m++) {
            if (base->baseVarOfConstraint[i_m] != LACKBASE)  // 该约束已找到对应的基变量
                continue;
            tRightSide rhs(LARGE_EQUAL, 0);
            tVar var("__auxiliary_" + to_string(i_m) + "__", rhs, 0.0);
            auxiliaryPblm->X.push_back(var);
            auxiliaryPblm->C.push_back(-1);
            auxiliaryPblm->P.push_back(vector<double>(m, 0.0));
            auxiliaryPblm->P.back().at(i_m) = 1;
        }
        cout << "开始解辅助问题" << endl;
        auxiliaryPblm = SimplexMethod(auxiliaryPblm);
        if (auxiliaryPblm->result != NO_SOLUTION) {
            vP::iterator itp = pblm->P.begin();
            vP::const_iterator itp_ = auxiliaryPblm->P.cbegin();
            for (; itp != pblm->P.end() && itp_ != auxiliaryPblm->P.cend(); ++itp, ++itp_)
                (*itp) = (*itp_);
            vB::iterator itb = pblm->B.begin();
            vB::const_iterator itb_ = auxiliaryPblm->B.cbegin();
            for (; itb != pblm->B.end() && itb_ != auxiliaryPblm->B.cend(); ++itb, ++itb_)
                (*itb) = (*itb_);
            vX::iterator itx = pblm->X.begin();
            vX::const_iterator itx_ = auxiliaryPblm->X.cbegin();
            for (; itx != pblm->X.end() && itx_ != auxiliaryPblm->X.cend(); ++itx, ++itx_)
                (*itx) = (*itx_);
        }
        // cout << "原问题可转化为" << endl;
        // pblm->OutputTarget();
        // pblm->OutputConstraint();
        // 若此时的X不符合方程约束，则说明该问题无解
        // cout << "辅助问题最终形式为" << endl;
        // auxiliaryPblm->OutputConstraint();
        // auxiliaryPblm->OutputResult();
        // if (!pblm->TestConstraint()) {
        //     cout << "No solution" << endl;
        //     pblm->result = NO_SOLUTION;
        //     return base;
        // }

        base = newGetBase(pblm, OTHER);
        delete auxiliaryPblm;
        pblm->result = UNKNOWN;
        return base;
    }

    return base;
}

void PivotBase(Problem* pblm, Base* base, int rowChosen, int colChosen) {
    int m = pblm->B.size();
    int n = pblm->X.size();
    // rowChosen *= 1 / a，产生1元素
    double eleChosen = pblm->P.at(colChosen).at(rowChosen);
    assert(!equals(eleChosen, 0));

    pblm->B.at(rowChosen).second /= eleChosen;
    pblm->X.at(colChosen).value = pblm->B.at(rowChosen).second;
    // cout << pblm->X.at(colChosen).name << "新值为" << pblm->X.at(colChosen).value << endl;
    if (base->baseVarOfConstraint[rowChosen] != LACKBASE)
        pblm->X.at(base->baseVarOfConstraint[rowChosen]).value = 0;  // 原本如有基变量，则归零
    for (int j_n = 0; j_n < n; j_n++)
        pblm->P.at(j_n).at(rowChosen) /= eleChosen;
    // 遍历其他行，i_m += rowChosen * Multiplier，该列其他元素变为0
    for (int i_m = 0; i_m < m; i_m++) {
        if (i_m == rowChosen)
            continue;
        double Multiplier = -pblm->P.at(colChosen).at(i_m);
        pblm->B.at(i_m).second += Multiplier * pblm->B.at(rowChosen).second;
        for (int j_n = 0; j_n < n; j_n++)
            pblm->P.at(j_n).at(i_m) += Multiplier * pblm->P.at(j_n).at(rowChosen);
        if (base->baseVarOfConstraint[i_m] == LACKBASE)
            continue;
        pblm->X.at(base->baseVarOfConstraint[i_m]).value += Multiplier * pblm->B.at(rowChosen).second;
        // if (!equals(Multiplier * pblm->B.at(rowChosen).second, 0))
        //     cout << pblm->X.at(base->baseVarOfConstraint[i_m]).name << "增加了" << Multiplier * pblm->B.at(rowChosen).second << "增加至" << pblm->X.at(base->baseVarOfConstraint[i_m]).value << endl;
    }
}
