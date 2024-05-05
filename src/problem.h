/*
 * 定义基础数据结构
 */
#pragma once
#include <float.h>
#include <algorithm>
#include <cassert>
#include <cmath>
#include <iostream>
#include <regex>
#include <sstream>
#include <string>
#include <vector>
using namespace std;

// 数据结构区
enum MaxOrMin {
    Max,
    Min,
};
enum ResultType {
    NO_SOLUTION = -1,
    UNKNOWN = 0,
    ONE_SOLUTION,
    MAYBE_MANY,
    MANY_SOLUTION,
    UNBOUNDED,
};
enum Range {
    UNLIMITED,
    LARGE_EQUAL,
    SMALL_EQUAL,
    EQUAL,
};
enum WorkMode {
    SIMPLEX,
    DUAL_SIMPLEX,
    OTHER,
};

typedef pair<Range, double> tRightSide;
typedef vector<tRightSide> vB;
typedef vector<double> vValue;
typedef vector<vValue> vP;
typedef pair<double, vValue> tResult;

struct tVar {
    string name;
    tRightSide rhs;
    double value;
    tVar(string name, tRightSide rhs, double value) {
        this->name = name;
        this->rhs = rhs;
        this->value = value;
    }
};

typedef vector<tVar> vX;

class Problem {
   public:
    MaxOrMin maxOrMin = Max;  // 优化目标
    vX X;                     // 决策变量信息
    vP P;                     // 工艺系数 P[i_n][i_m]
    vValue C;                 // 目标函数系数
    vB B;                     // 约束条件右边项 //B>=0
    double offset = 0;        // 目标函数修正值，注意它不一定能表示最后结果
    ResultType result = UNKNOWN;
    Problem() {}
    Problem(const Problem& other)
        : X(other.X), P(other.P), C(other.C), B(other.B) {
        this->maxOrMin = other.maxOrMin;
        this->offset = other.offset;
    }
    Problem* Dualize();
    bool IsStandard();
    Problem* Standardlize(WorkMode = SIMPLEX);
    void ChangeMinMax(MaxOrMin = Max);
    void ChangeBValue(Range = LARGE_EQUAL);
    void ChangeBRange(Range = EQUAL);
    void ChangeXRange(tRightSide = make_pair(LARGE_EQUAL, 0.0));

    bool TestConstraint();
    void SimplifyDouble();

    tResult* GetResult();
    void OutputResult();

    void OutputPblm();
    void OutputVar();
    void OutputTarget();
    void OutputConstraint();
};

// 函数区
inline bool equals(double d1, double d2) {
    return (d1 - d2 < 1e-6) && (d2 - d1 < 1e-6);
}
inline bool largeEquals(double d1, double d2) {
    return (d1 > d2) || equals(d1, d2);
}
inline bool largeThan(double d1, double d2) {
    return (d1 > d2) && !equals(d1, d2);
}
inline bool smallEquals(double d1, double d2) {
    return (d1 < d2) || equals(d1, d2);
}
inline bool smallThan(double d1, double d2) {
    return (d1 < d2) && !equals(d1, d2);
}
inline bool IsInt(double d) {
    // d always in [floor(d), floor(d) + 1];
    // cout << "d is " << d << ", floor d is " << floor(d) << endl;
    // cout << "equals(d, floor(d))的结果为" << equals(d, floor(d)) << endl;
    // cout << "equals(d, 1 + floor(d))的结果为" << equals(d, 1 + floor(d)) << endl;
    return equals(d, floor(d)) || equals(d, 1 + floor(d));
}
inline int SimplifyToInt(double d) {
    assert(IsInt(d));
    if (equals(d, floor(d)))
        return int(floor(d));
    else if (equals(d, 1 + floor(d)))
        return int(1 + floor(d));
    else
        cout << "SimplifyToInt Error" << endl;
    return 0;
}
inline bool IsIntResult(tResult* rst) {
    for (vValue::const_iterator itv = rst->second.cbegin(); itv != rst->second.cend(); ++itv)
        if (!IsInt(*itv)) {
            // cout << (*itv) << "is not int!!!" << endl;
            return false;
        }
    return true;
}

Problem* InputPblm() {
    Problem* pblm = new Problem();
    string namePattern = "[a-zA-Z_](\\w)*";  // C++变量格式
    string rangePattern = "(\\+)|(-)|(\\?)|((>|<)=(\\s)*((\\+)|(-))?((\\d)+(\\.(\\d)+)?))";
    // (\\+)|(-)|(\\?)表达可以是+-或者?中的一个；(>|<)=(\\s)*为(不)等号及后面的空格；((\\+)|(-))?((\\d)+(\\.(\\d)+)?)是一个可以包含符号的数
    string constraintPattern = "((>|<)?=(\\s)*((\\+)|(-))?((\\d)+(\\.(\\d)+)?))";
    string factorPattern = "(\\d)+(\\.(\\d)+)?";  // 一个数，整/浮都可以，不含先导正负号
    regex var_name(namePattern);                  // 变量名
    regex var_range(rangePattern);                // 变量范围
    regex constraint(constraintPattern);          // 约束右边项
    regex factor(factorPattern);

    // 输入变量
    {
        cout << "Step1:请输入变量及取值范围" << endl;
        cout << "例如x1>=0,x2<=3,x3+,x4-,x5?;" << endl;
        cout << "以\",\"分隔，以\";\"结尾" << endl;
        cout << ">=0可缩写为+, <=0可缩写为-, 无约束以?表示" << endl;

        // 输入
        string result;
        getline(cin, result, ';');
        replace(result.begin(), result.end(), '\n', ' ');  // 换行视作空格

        // 匹配变量及取值范围
        sregex_iterator name_pos(result.cbegin(), result.cend(), var_name);
        sregex_iterator range_pos(result.cbegin(), result.cend(), var_range);
        for (sregex_iterator end; name_pos != end && range_pos != end; ++name_pos, ++range_pos) {
            string name = name_pos->str();
            string str = range_pos->str();
            tRightSide rhs;
            if (str == "\\")
                rhs = make_pair(UNLIMITED, 0);
            else if (str == "+")
                rhs = make_pair(LARGE_EQUAL, 0);
            else if (str == "-")
                rhs = make_pair(SMALL_EQUAL, 0);
            else if (str.substr(0, 2) == ">=")
                rhs = make_pair(LARGE_EQUAL, stod(str.substr(2)));
            else if (str.substr(0, 2) == "<=")
                rhs = make_pair(SMALL_EQUAL, stod(str.substr(2)));

            tVar var(name, rhs, 0.0);
            pblm->X.push_back(var);
        }
    }

    // 根据列数调整数据结构
    pblm->C.resize(pblm->X.size());
    pblm->P.resize(pblm->X.size());

    // 输入目标函数
    {
        cout << "Step2:请输入目标函数" << endl;
        cout << "例如max 2*x1-x2+4x3;" << endl;
        cout << "以\"max\"或\"min\"开头，以\";\"结尾" << endl;

        // 输入
        string result;
        getline(cin, result, ';');
        replace(result.begin(), result.end(), '\n', ' ');  // 换行视作空格
        result.erase(0, result.find_first_not_of(' '));    // 去除前导空格

        // 判断最大化/最小化
        string first4 = result.substr(0, 4);
        result.erase(0, 4);
        transform(first4.begin(), first4.end(), first4.begin(), ::tolower);  // 转小写
        if (first4 == "max ")
            pblm->maxOrMin = Max;
        else if (first4 == "min ")
            pblm->maxOrMin = Min;
        else
            cout << "minmax Error!" << endl;

        // 分析目标函数的变量部分
        result.erase(remove(result.begin(), result.end(), '*'), result.end());  // 删除*
        result.erase(remove(result.begin(), result.end(), ' '), result.end());  // 删除空格
        sregex_iterator name_pos(result.cbegin(), result.cend(), var_name);
        for (sregex_iterator end; name_pos != end; ++name_pos) {
            // 假定是第i号变量

            // 寻找并记录系数C[i]
            double c;  // c[i]
            string prefix = name_pos->prefix().str();
            size_t last_not_num = prefix.find_last_not_of(".0123456789");
            if (prefix.size() == 0)
                c = 1;  // name + x2 <= 3
            else if (last_not_num == string::npos)
                c = stod(prefix);  // 2name + x2 <=3
            else if (last_not_num == prefix.size() - 1) {
                if (prefix[last_not_num] == '+')  // x1 + name<=4;
                    c = 1.0;
                else if (prefix[last_not_num] == '-')  // x1 - name<=4;
                    c = -1.0;
                else
                    cout << "Ci Error!" << endl;
            } else {
                if (prefix[last_not_num] == '+' || prefix[last_not_num] == '-')  // x1+ 3name>=2;
                    c = stod(prefix.substr(last_not_num));                       // 包括了last_not_num
                else
                    cout << "Ci Error!" << endl;
            }

            // 找到对应的变量并修改C[i]，即确定i
            string name = name_pos->str();
            vX::iterator itx;
            for (itx = pblm->X.begin(); itx != pblm->X.end(); ++itx)
                if ((*itx).name == name)
                    break;
            if (itx == pblm->X.end())
                cout << "var name is "<< name <<", not found, error!" << endl;
            int index = distance(pblm->X.begin(), itx);
            pblm->C.at(index) += c;  // 考虑到可能有 x1+x2+x1的情况
        }

        // 分析目标函数中可能存在的常数项
        // 特点为前置为+或者-或者null，后置为+或者-或者null
        sregex_iterator factor_pos(result.cbegin(), result.cend(), factor);
        for (sregex_iterator end; factor_pos != end; ++factor_pos) {
            // 寻找并记录常数项ofst
            double ofst = stod(factor_pos->str());
            string suffix = factor_pos->suffix().str();
            string prefix = factor_pos->prefix().str();
            if (suffix.size() != 0 && suffix[0] != '+' && suffix[0] != '-')
                continue;
            if (prefix.size() == 0 || prefix.back() == '+')
                pblm->offset += ofst;
            else if (prefix.back() == '-')
                pblm->offset += -ofst;
        }
        // pblm->OutputTarget();
    }

    // 输入约束条件
    {
        cout << "Step3:请输入约束条件" << endl;
        cout << "例如x1+x2-3x3<=9,\nx1-2x2+x3=7;" << endl;
        cout << "约束间以\",\"或换行进行分隔，以\";\"结尾" << endl;

        // 整体输入
        string result, line;
        getline(cin, result, ';');
        replace(result.begin(), result.end(), '\n', ',');  // 换行视作逗号

        // 根据约束条件数调整数据结构
        int m = 0;
        stringstream s(result);
        while (getline(s, line, ','))
            if (!line.empty())
                m++;
        pblm->B.resize(m);
        for (vP::iterator itp = pblm->P.begin(); itp != pblm->P.end(); ++itp)
            (*itp).resize(m);

        // 单句输入
        stringstream ss(result);
        int line_num = 0;
        while (getline(ss, line, ',')) {
            if (line.empty())  // 换行视作逗号导致会有过多的行
                continue;
            // cout << "line is " << line << endl;
            line.erase(remove(line.begin(), line.end(), '*'), line.end());  // 删除*
            line.erase(remove(line.begin(), line.end(), ' '), line.end());  // 删除空格

            // 识别约束条件右边项
            smatch constraint_matched;
            regex_search(line, constraint_matched, constraint);
            string str = constraint_matched.str();
            tRightSide rhs;
            if (str[0] == '=')
                rhs = make_pair(EQUAL, stod(str.substr(1)));
            else if (str.substr(0, 2) == ">=")
                rhs = make_pair(LARGE_EQUAL, stod(str.substr(2)));
            else if (str.substr(0, 2) == "<=")
                rhs = make_pair(SMALL_EQUAL, stod(str.substr(2)));
            else
                cout << "constraint Error! constraint is " << str << endl;
            pblm->B.at(line_num) = rhs;

            // 识别约束条件左边项
            sregex_iterator name_pos(line.cbegin(), line.cend(), var_name);
            for (sregex_iterator end; name_pos != end; ++name_pos) {
                // 假定是第i号变量，第j号方程
                // 寻找并记录系数P[i][j]
                double p = 0;  // P[i][j]
                string prefix = name_pos->prefix().str();
                size_t last_not_num = prefix.find_last_not_of(".0123456789");
                if (prefix.size() == 0)
                    p = 1;  // name + x2 <= 3
                else if (last_not_num == string::npos)
                    p = stod(prefix);  // 2name + x2 <=3
                else if (last_not_num == prefix.size() - 1) {
                    if (prefix[last_not_num] == '+')  // x1 + name<=4;
                        p = 1.0;
                    else if (prefix[last_not_num] == '-')  // x1 - name<=4;
                        p = -1.0;
                    else
                        cout << "Pij Error!" << endl;
                } else {
                    if (prefix[last_not_num] == '+' || prefix[last_not_num] == '-')  // x1+ 3name>=2;
                        p = stod(prefix.substr(last_not_num));                       // 包括了last_not_num
                    else
                        cout << "Pij Error!" << endl;
                }

                // 找到对应的变量并修改P[i][j]，即确定i
                string name = name_pos->str();
                vX::iterator itx;
                for (itx = pblm->X.begin(); itx != pblm->X.end(); ++itx)
                    if ((*itx).name == name)
                        break;
                if (itx == pblm->X.end())
                    cout << "var not found,error!" << endl;
                int index = distance(pblm->X.begin(), itx);
                // cout << "index is " << index << ", line_num is " << line_num << endl;
                pblm->P.at(index).at(line_num) += p;
            }
            line_num++;
        }
    }
    return pblm;
}

void Problem::OutputPblm() {
    OutputVar();         // 输出变量及范围
    OutputTarget();      // 输出目标函数
    OutputConstraint();  // 输出约束条件
}

void Problem::OutputVar() {
    for (vX::const_iterator itx = X.cbegin(); itx != X.cend(); ++itx) {
        cout << (*itx).name;
        switch ((*itx).rhs.first) {
            case UNLIMITED:
                cout << " is unlimited" << endl;
                break;
            case LARGE_EQUAL:
                cout << ">=" << (*itx).rhs.second << endl;
                break;
            case SMALL_EQUAL:
                cout << "<=" << (*itx).rhs.second << endl;
                break;
            case EQUAL:
                cout << "=" << (*itx).rhs.second << endl;
                break;
            default:
                cout << "output Error!" << endl;
                break;
        }
    }
    cout << "共" << X.size() << "个变量" << endl;
}

void Problem::OutputTarget() {
    if (maxOrMin == Max)
        cout << "max " << flush;
    else if (maxOrMin == Min)
        cout << "min " << flush;
    vValue::const_iterator itc = C.cbegin();
    vX::const_iterator itx = X.cbegin();
    for (; itc != C.cend() && itx != X.cend(); ++itc, ++itx) {
        if (!*itc)
            continue;
        if (itc != C.cbegin())
            cout << " ";
        if (*itc > 0 && itc != C.cbegin())
            cout << "+";
        cout << *itc << "*" << (*itx).name;
    }
    if (offset) {
        cout << " ";
        if (offset > 0)
            cout << "+";
        cout << offset;
    }
    cout << endl;
}

void Problem::OutputConstraint() {
    for (vB::const_iterator itb = B.cbegin(); itb != B.cend(); ++itb) {  // 按约束遍历
        int index = distance(B.cbegin(), itb);
        int Psz = P.size();
        for (int i = 0; i < Psz; ++i) {
            if (!P.at(i).at(index))
                continue;
            if (i)
                cout << " ";
            if (P.at(i).at(index) > 0 && i)
                cout << "+";
            cout << P.at(i).at(index) << "*" << X.at(i).name;
        }
        switch ((*itb).first) {
            case UNLIMITED:
                cout << "constraint Error!" << endl;
                break;
            case LARGE_EQUAL:
                cout << " >= " << (*itb).second << endl;
                break;
            case SMALL_EQUAL:
                cout << " <= " << (*itb).second << endl;
                break;
            case EQUAL:
                cout << " = " << (*itb).second << endl;
                break;
            default:
                cout << "output Error!" << endl;
                break;
        }
    }
    cout << "共" << B.size() << "个约束条件" << endl;
}

Problem* Problem::Dualize() {
    int m = B.size(), n = X.size();
    Problem* dual = new Problem();

    // P 转置
    dual->P.resize(m);
    for (vP::const_iterator itp_out = P.cbegin(); itp_out != P.cend(); ++itp_out) {
        vValue::const_iterator itp_in = (*itp_out).cbegin();
        for (int index = 0; itp_in != (*itp_out).cend(); ++itp_in, ++index)
            dual->P.at(index).push_back(*itp_in);
    }

    // 约束条件右边项数值决定目标函数系数
    // 约束条件不等号决定变量取值范围(Max相反，Min相同)
    vB::const_iterator itb = B.cbegin();
    for (int index = 0; itb != B.cend(); ++itb, ++index) {
        Range range;
        if ((*itb).first == EQUAL)
            range = UNLIMITED;
        else if ((*itb).first == SMALL_EQUAL) {
            if (maxOrMin == Max)
                range = LARGE_EQUAL;
            else if (maxOrMin == Min)
                range = SMALL_EQUAL;
        } else if ((*itb).first == LARGE_EQUAL) {
            if (maxOrMin == Max)
                range = SMALL_EQUAL;
            else if (maxOrMin == Min)
                range = LARGE_EQUAL;
        } else
            cout << "Range Error!" << endl;
        tRightSide rhs(range, 0);
        tVar var("line" + to_string(index), rhs, 0.0);
        dual->X.push_back(var);
        dual->C.push_back((*itb).second);
    }

    // 目标函数系数决定约束条件右边项数值
    // 变量取值范围决定约束条件不等号(Max相同，Min相反)
    vValue::const_iterator itc = C.cbegin();
    vX::const_iterator itx = X.cbegin();
    for (; itc != C.cend() && itx != X.cend(); ++itc, ++itx) {
        Range range;
        if ((*itx).rhs.first == UNLIMITED)
            range = EQUAL;
        else if ((*itx).rhs.first == SMALL_EQUAL) {
            if (maxOrMin == Max)
                range = SMALL_EQUAL;
            else if (maxOrMin == Min)
                range = LARGE_EQUAL;
        } else if ((*itx).rhs.first == LARGE_EQUAL) {
            if (maxOrMin == Max)
                range = LARGE_EQUAL;
            else if (maxOrMin == Min)
                range = SMALL_EQUAL;
        } else
            cout << "Range Error!" << endl;
        tRightSide rhs(range, *itc);
        dual->B.push_back(rhs);
    }

    // 变量取值范围决定约束条件右边项数值
    dual->offset = offset;
    for (vX::const_iterator itx = X.cbegin(); itx != X.cend(); ++itx) {
        if (equals(0.0, (*itx).rhs.second))
            continue;
        int index = distance(X.cbegin(), itx);
        vValue::iterator itc_ = dual->C.begin();
        for (int i = 0; itc_ != dual->C.end(); ++itc_, ++i)
            (*itc_) -= (*itx).rhs.second * P.at(index).at(i);
        dual->offset += (*itx).rhs.second * C.at(index);  // 对偶问题需要取反
    }

    // 目标函数变化
    if (maxOrMin == Max)
        dual->maxOrMin = Min;
    else
        dual->maxOrMin = Max;
    return dual;
}

Problem* Problem::Standardlize(WorkMode mode) {
    // 分别代表不等号和b值范围，默认前者为取等，后者为非负
    // 生成一个标准型
    cout << "标准化！" << endl;
    Problem* pblm = new Problem(*this);

    // 变为求目标函数最大化
    pblm->ChangeMinMax();
    // if (maxOrMin == Min) {
    //     for (vValue::iterator itc = pblm->C.begin(); itc != pblm->C.end(); ++itc)
    //         *itc = -*itc;
    //     pblm->maxOrMin = Max;
    //     pblm->offset = -pblm->offset;
    // }

    // 修正变量取值范围
    pblm->ChangeXRange();
    // for (vX::iterator itx = pblm->X.begin(); itx != pblm->X.end(); ++itx) {
    //     double bound = (*itx).rhs.second;
    //     string bound_str = to_string(bound);
    //     // if (equals(bound, floor(bound)))  // bound = 3.0001 / -2.9999
    //     //     bound_str = to_string(int(bound));
    //     // else if (equals(bound, floor(bound) + 1))  // bound = 2.9999 / -3.0001
    //     //     bound_str = to_string(int(bound) + 1);
    //     if (IsInt(bound))
    //         bound_str = to_string(SimplifyToInt(bound));
    //     regex_replace(bound_str, std::regex("\\."), "dot");
    //     int index = distance(pblm->X.begin(), itx);
    //     if ((*itx).rhs.first == EQUAL)
    //         continue;
    //     // x>=a,x'=x-a,x=x'+a
    //     else if ((*itx).rhs.first == LARGE_EQUAL) {
    //         if (equals(bound, 0))
    //             continue;
    //         cout << index << "号变量大于等于" << bound << "需要修正" << endl;
    //         (*itx).name = "__" + (*itx).name + "_minus_" + bound_str + "__";  // x'=x-a,x=x'+a
    //         pblm->offset += bound * pblm->C.at(index);
    //         vValue::iterator itp_in = pblm->P.at(index).begin();
    //         vB::iterator itb = pblm->B.begin();
    //         for (; itp_in != pblm->P.at(index).end() && itb != pblm->B.end(); ++itp_in, ++itb)
    //             (*itb).second -= bound * (*itp_in);
    //         (*itx).rhs = make_pair(LARGE_EQUAL, 0);
    //     }
    //     // x<=a,x'=-x+a,x=-x'+a
    //     else if ((*itx).rhs.first == SMALL_EQUAL) {
    //         cout << index << "号变量小于等于" << bound << "需要修正" << endl;
    //         (*itx).name = "__minus_" + (*itx).name + "_plus_" + bound_str + "__";  // x'=-x+a,x=-x'+a
    //         pblm->offset += bound * pblm->C.at(index);
    //         pblm->C.at(index) = -pblm->C.at(index);  // Ci取负
    //         vValue::iterator itp_in = pblm->P.at(index).begin();
    //         vB::iterator itb = pblm->B.begin();
    //         for (; itp_in != pblm->P.at(index).end() && itb != pblm->B.end(); ++itp_in, ++itb) {
    //             (*itb).second -= bound * (*itp_in);
    //             (*itp_in) = -(*itp_in);  // Pi全部取负
    //         }
    //         (*itx).rhs = make_pair(LARGE_EQUAL, 0);
    //     }
    //     // x unlimited,x = x_1-x_2
    //     else if ((*itx).rhs.first == UNLIMITED) {
    //         cout << index << "号变量取值范围无限制，需要修正" << endl;
    //         string name = (*itx).name;
    //         (*itx).rhs = make_pair(LARGE_EQUAL, 0);
    //         (*itx).name = "__" + name + "_part1__";
    //         tRightSide rhs(LARGE_EQUAL, 0);
    //         tVar var("__" + name + "_part2__", rhs, 0.0);
    //         itx = pblm->X.emplace(itx + 1, var);                               // 迭代器可能失效，需要重新获取
    //         pblm->C.emplace(pblm->C.begin() + index + 1, -pblm->C.at(index));  // Ci' = -Ci
    //         pblm->P.emplace(pblm->P.begin() + index + 1, pblm->P.at(index));   // Pi' = Pi
    //         vValue::iterator itp_in = pblm->P.at(index + 1).begin();
    //         for (; itp_in != pblm->P.at(index + 1).end(); ++itp_in)
    //             (*itp_in) = -(*itp_in);  // Pi' = -Pi' = -Pi
    //     } else
    //         cout << "Error" << (*itx).rhs.first << endl;
    // }

    // 修改约束条件不等号，修改右侧值
    // 该部分最好用函数解决
    for (vB::iterator itb = pblm->B.begin(); itb != pblm->B.end(); ++itb) {
        int index = distance(pblm->B.begin(), itb);
        // cout << "index is " << index << endl;
        // 默认保持B的非负性
        if (mode == SIMPLEX) {
            if ((*itb).second < 0) {
                (*itb).second = -(*itb).second;
                if ((*itb).first == LARGE_EQUAL)
                    (*itb).first = SMALL_EQUAL;
                else if ((*itb).first == SMALL_EQUAL)
                    (*itb).first = LARGE_EQUAL;
                int Psz = pblm->P.size();
                for (int i = 0; i < Psz; ++i)
                    pblm->P.at(i).at(index) = -pblm->P.at(i).at(index);
            }
        }
        // 这个用处不大
        // else if (mode == DUAL_SIMPLEX) {
        //     if ((*itb).second > 0) {
        //         (*itb).second = -(*itb).second;
        //         if ((*itb).first == LARGE_EQUAL)
        //             (*itb).first = SMALL_EQUAL;
        //         else if ((*itb).first == SMALL_EQUAL)
        //             (*itb).first = LARGE_EQUAL;
        //         int Psz = pblm->P.size();
        //         for (int i = 0; i < Psz; ++i)
        //             pblm->P.at(i).at(index) = -pblm->P.at(i).at(index);
        //     }
        // }


        /*  在对偶单纯形法时作特殊变换，通过引入更多约束来增加基变量
        if ((*itb).first == EQUAL) {
            if (mode == SIMPLEX)
                continue;
            else if (mode == DUAL_SIMPLEX) {
                // 首先检查是否已存在松弛/剩余变量
                int n = pblm->X.size();
                int m = pblm->B.size();
                int already = -1;
                for (int i_n = 0; i_n < n; i_n++) {
                    if (pblm->C.at(i_n))  // Ci!=0说明不是松弛/剩余变量
                        continue;
                    if (equals(pblm->P.at(i_n).at(index), 0))  // 该元素==0说明不是松弛/剩余变量
                        continue;
                    for (int i_m = 0; i_m < m; i_m++) {
                        if (pblm->P.at(i_n).at(i_m) && i_m != index)  // 该列其他元素!=0说明不是松弛/剩余变量
                            break;
                        if (i_m + 1 == m)
                            already = i_n;
                    }
                }
                // cout << "already is " << already << endl;
                // 已经存在某个变量作为松弛变量
                if (already != -1) {
                    // 该约束规范化为sigmaX + x' = b
                    double eleChosen = pblm->P.at(already).at(index);
                    pblm->B.at(index).second /= eleChosen;
                    for (int i_n = 0; i_n < n; i_n++)
                        pblm->P.at(i_n).at(index) /= eleChosen;
                }
                // 手动获得松弛变量
                else {
                    // sigmaX = b -> sigmaX + x' = b && -sigmaX + x'' = -b
                    // 将约束取反引入松弛变量1
                    tRightSide rhs(LARGE_EQUAL, 0);
                    pblm->B.push_back(pblm->B.at(index));
                    itb = pblm->B.begin() + index;  // push之后迭代器可能失效，需要重新获取
                    for (int i_n = 0; i_n < n; i_n++)
                        pblm->P.at(i_n).push_back(-pblm->P.at(i_n).at(index));
                    int index1 = pblm->B.size() - 1;
                    tVar var1("__flabby_" + to_string(index1) + "__", rhs, 0.0);
                    pblm->X.push_back(var1);
                    pblm->C.push_back(0.0);
                    pblm->P.push_back(vector<double>(pblm->B.size(), 0.0));
                    pblm->P.back().at(index1) = +1;
                    // pblm->OutputPblm();
                    // 原约束中引入松弛变量2
                    tVar var2("__flabby_" + to_string(index) + "__", rhs, 0.0);
                    pblm->X.push_back(var2);
                    pblm->C.push_back(0.0);
                    pblm->P.push_back(vector<double>(pblm->B.size(), 0.0));
                    pblm->P.back().at(index) = +1;
                    // pblm->OutputPblm();
                    // cout << "111" << endl;
                }
            }
        }
        */

        // 优先添加松弛变量，故≥时需做特殊处理
        // 不等号为≥时
        if ((*itb).first == LARGE_EQUAL) {
            // 一般情况，引入剩余变量remain variable
            if (mode == SIMPLEX) {
                tRightSide rhs(LARGE_EQUAL, 0);
                tVar var("__remain_" + to_string(index) + "__", rhs, 0.0);
                pblm->X.push_back(var);
                pblm->C.push_back(0.0);
                pblm->P.push_back(vector<double>(pblm->B.size(), 0.0));
                pblm->P.back().at(index) = -1;
                (*itb).first = EQUAL;
            }
            // 对偶标准化时，两侧取反并引入松弛变量
            else if (mode == DUAL_SIMPLEX) {
                (*itb).second = -(*itb).second;
                for (vP::iterator itp = pblm->P.begin(); itp != pblm->P.end(); ++itp)
                    (*itp).at(index) = -(*itp).at(index);
                tRightSide rhs(LARGE_EQUAL, 0);
                tVar var("__flabby_" + to_string(index) + "__", rhs, 0.0);
                pblm->X.push_back(var);
                pblm->C.push_back(0.0);
                pblm->P.push_back(vector<double>(pblm->B.size(), 0.0));
                pblm->P.back().at(index) = +1;
                (*itb).first = EQUAL;
            }
        }
        // 不等号为≤时，引入松弛变量flabby variable
        else if ((*itb).first == SMALL_EQUAL) {
            tRightSide rhs(LARGE_EQUAL, 0);
            tVar var("__flabby_" + to_string(index) + "__", rhs, 0.0);
            pblm->X.push_back(var);
            pblm->C.push_back(0.0);
            pblm->P.push_back(vector<double>(pblm->B.size(), 0.0));
            pblm->P.back().at(index) = +1;
            (*itb).first = EQUAL;
        }
    }
    // DUAL_SIMPLEX模式下，
    // =b -> =-b,
    // <=b -> +x=b,
    // <=-b -> +x=-b,
    // >=b -> +x=-b,
    // <=b -> +x=b;

    return pblm;
}

// 更改优化方向
void Problem::ChangeMinMax(MaxOrMin drcn) {
    if (maxOrMin == Min && drcn == Max) {
        for (vValue::iterator itc = C.begin(); itc != C.end(); ++itc)
            *itc = -*itc;
        maxOrMin = Max;
        offset = -offset;
    } else if (maxOrMin == Max && drcn == Min) {
        for (vValue::iterator itc = C.begin(); itc != C.end(); ++itc)
            *itc = -*itc;
        maxOrMin = Min;
        offset = -offset;
    }
}

// 更改约束条件右边项的值
void Problem::ChangeBValue(Range range) {
    if (range == UNLIMITED)
        return;
    else if (range == EQUAL) {
        cout << "ChangeBValue Error!" << endl;
        return;
    }
    int m = B.size();
    for (vB::iterator itb = B.begin(); itb != B.end(); ++itb) {
        int index = distance(B.begin(), itb);
        // 保持B的非负性或非正性
        if ((range == LARGE_EQUAL && (*itb).second < 0) || (range == SMALL_EQUAL && (*itb).second > 0)) {
            (*itb).second = -(*itb).second;
            if ((*itb).first == LARGE_EQUAL)
                (*itb).first = SMALL_EQUAL;
            else if ((*itb).first == SMALL_EQUAL)
                (*itb).first = LARGE_EQUAL;
            for (vP::iterator itp = P.begin(); itp != P.end(); ++itp)
                (*itp).at(index) = -(*itp).at(index);
        }
    }
}

// 更改约束条件不等号
void Problem::ChangeBRange(Range range) {
    int m = B.size();        // 这一部分中n会变化
    if (range == UNLIMITED)  // 不做修改
        return;
    else if (range == EQUAL)
        for (vB::iterator itb = B.begin(); itb != B.end(); ++itb) {
            int index = distance(B.begin(), itb);
            // 不等号为≥时，引入剩余变量remain variable
            if ((*itb).first == LARGE_EQUAL) {
                // 一般情况，引入剩余变量remain variable
                tRightSide rhs(LARGE_EQUAL, 0);
                tVar var("__remain_" + to_string(index) + "__", rhs, 0.0);
                X.push_back(var);
                C.push_back(0.0);
                P.push_back(vector<double>(m, 0.0));
                P.back().at(index) = -1;
                (*itb).first = EQUAL;

            }
            // 不等号为≤时，引入松弛变量flabby variable
            else if ((*itb).first == SMALL_EQUAL) {
                tRightSide rhs(LARGE_EQUAL, 0);
                tVar var("__flabby_" + to_string(index) + "__", rhs, 0.0);
                X.push_back(var);
                C.push_back(0.0);
                P.push_back(vector<double>(m, 0.0));
                P.back().at(index) = +1;
                (*itb).first = EQUAL;
            }
        }
    else if (range == LARGE_EQUAL || range == SMALL_EQUAL) {
        cout << "ChangeBRange Error!" << endl;
        return;
    }
}

// 更改变量取值范围
void Problem::ChangeXRange(tRightSide rightHandSide) {
    if (rightHandSide.first != LARGE_EQUAL || rightHandSide.second != 0) {
        cout << "not finish" << endl;
        return;
    }
    for (vX::iterator itx = X.begin(); itx != X.end(); ++itx) {
        double bound = (*itx).rhs.second;
        string bound_str = to_string(bound);
        if (IsInt(bound))
            bound_str = to_string(SimplifyToInt(bound));
        regex_replace(bound_str, std::regex("\\."), "dot");
        int index = distance(X.begin(), itx);
        if ((*itx).rhs.first == EQUAL)  // 已经指定取值的暂且忽略！！！
            continue;
        // x>=a,x'=x-a,x=x'+a
        else if ((*itx).rhs.first == LARGE_EQUAL) {
            if (equals(bound, 0))
                continue;
            cout << index << "号变量大于等于" << bound << "需要修正" << endl;
            (*itx).name = "__" + (*itx).name + "_minus_" + bound_str + "__";  // x'=x-a,x=x'+a
            offset += bound * C.at(index);
            vValue::iterator itp_in = P.at(index).begin();
            vB::iterator itb = B.begin();
            for (; itp_in != P.at(index).end() && itb != B.end(); ++itp_in, ++itb)
                (*itb).second -= bound * (*itp_in);
            (*itx).rhs = make_pair(LARGE_EQUAL, 0);
        }
        // x<=a,x'=-x+a,x=-x'+a
        else if ((*itx).rhs.first == SMALL_EQUAL) {
            cout << index << "号变量小于等于" << bound << "需要修正" << endl;
            (*itx).name = "__minus_" + (*itx).name + "_plus_" + bound_str + "__";  // x'=-x+a,x=-x'+a
            offset += bound * C.at(index);
            C.at(index) = -C.at(index);  // Ci取负
            vValue::iterator itp_in = P.at(index).begin();
            vB::iterator itb = B.begin();
            for (; itp_in != P.at(index).end() && itb != B.end(); ++itp_in, ++itb) {
                (*itb).second -= bound * (*itp_in);
                (*itp_in) = -(*itp_in);  // Pi全部取负
            }
            (*itx).rhs = make_pair(LARGE_EQUAL, 0);
        }
        // x unlimited,x = x_1-x_2
        else if ((*itx).rhs.first == UNLIMITED) {
            cout << index << "号变量取值范围无限制，需要修正" << endl;
            string name = (*itx).name;
            (*itx).rhs = make_pair(LARGE_EQUAL, 0);
            (*itx).name = "__" + name + "_part1__";
            tRightSide rhs(LARGE_EQUAL, 0);
            tVar var("__" + name + "_part2__", rhs, 0.0);
            itx = X.emplace(itx + 1, var);                   // 迭代器可能失效，需要重新获取
            C.emplace(C.begin() + index + 1, -C.at(index));  // Ci' = -Ci
            P.emplace(P.begin() + index + 1, P.at(index));   // Pi' = Pi
            vValue::iterator itp_in = P.at(index + 1).begin();
            for (; itp_in != P.at(index + 1).end(); ++itp_in)
                (*itp_in) = -(*itp_in);  // Pi' = -Pi' = -Pi
        } else
            cout << "Error" << (*itx).rhs.first << endl;
    }
}

// 检验对象是否已标准化，应该带参数比较合适
bool Problem::IsStandard() {
    // 目标函数取最大化
    if (maxOrMin != Max)
        return 0;

    // 所有变量取值都为非负
    for (vX::const_iterator itx = X.cbegin(); itx != X.cend(); ++itx)
        if ((*itx).rhs.first != LARGE_EQUAL || !equals((*itx).rhs.second, 0))
            return 0;

    // 所有约束都为等于某非负值
    for (vB::const_iterator itb = B.cbegin(); itb != B.cend(); ++itb)
        if ((*itb).first != EQUAL || (*itb).second < 0)
            return 0;

    // cout << "Problem is Standard!" << endl;
    return 1;
}

// 根据问题的X取值得到结果
tResult* Problem::GetResult() {
    tResult* rst = new tResult();
    for (vX::const_iterator itx = X.cbegin(); itx != X.cend(); ++itx) {
        int index = distance(X.cbegin(), itx);
        rst->first += (*itx).value * C.at(index);
    }
    rst->first += offset;
    if (IsInt(rst->first))
        rst->first = SimplifyToInt(rst->first);
    rst->second.clear();
    for (vX::const_iterator itx = X.cbegin(); itx != X.cend(); ++itx)
        rst->second.push_back((*itx).value);
    return rst;
}

// 根据问题的X取值输出结果
void Problem::OutputResult() {
    if (result == NO_SOLUTION) {
        cout << "该问题无可行解" << endl;
        return;
    } else if (result == UNBOUNDED) {
        cout << "该问题无有界最优解" << endl;
        return;
    }
    double target_value = 0;
    for (vX::const_iterator itx = X.cbegin(); itx != X.cend(); ++itx) {
        int index = distance(X.cbegin(), itx);
        target_value += (*itx).value * C.at(index);
    }
    target_value += offset;
    if (IsInt(target_value))
        target_value = SimplifyToInt(target_value);
    cout << "target value is " << target_value;
    cout << ", get at (";
    for (vX::const_iterator itx = X.cbegin(); itx != X.cend(); ++itx) {
        cout << (*itx).name << "=" << (*itx).value;
        if (itx + 1 != X.cend())  // 此处判断循环是否结束
            cout << ", ";
    }
    cout << ")" << endl;
}

// 检测当前的X取值是否符合问题要求
bool Problem::TestConstraint() {
    for (vB::const_iterator itb = B.cbegin(); itb != B.cend(); ++itb) {  // 按约束遍历
        double sigma = 0;
        int index = distance(B.cbegin(), itb);
        int Psz = P.size();
        for (int i = 0; i < Psz; ++i)
            sigma += P.at(i).at(index) * X.at(i).value;
        bool isError = false;
        isError |= ((*itb).first == EQUAL && !equals(sigma, (*itb).second));
        isError |= ((*itb).first == LARGE_EQUAL && sigma < (*itb).second && !equals(sigma, (*itb).second));
        isError |= ((*itb).first == SMALL_EQUAL && sigma > (*itb).second && !equals(sigma, (*itb).second));
        if (!isError)
            continue;
        // this->OutputResult();
        cout << "sigma is " << sigma << ", rhs is " << (*itb).second << endl;
        cout << "原约束为";
        for (int i = 0; i < Psz; ++i) {
            if (!P.at(i).at(index))
                continue;
            if (i)
                cout << " ";
            if (P.at(i).at(index) > 0 && i)
                cout << "+";
            cout << P.at(i).at(index) << "*" << X.at(i).name;
        }
        if ((*itb).first == EQUAL)
            cout << " = " << (*itb).second << endl;
        else if ((*itb).first == LARGE_EQUAL)
            cout << " >= " << (*itb).second << endl;
        else if ((*itb).first == SMALL_EQUAL)
            cout << " <= " << (*itb).second << endl;
        
        return false;
    }
    return true;
}

// 解决0.999!=1所用
void Problem::SimplifyDouble() {
    for (vX::iterator itx = X.begin(); itx != X.end(); ++itx)
        if (IsInt((*itx).value))
            (*itx).value = SimplifyToInt((*itx).value);
    for (vValue::iterator itc = C.begin(); itc != C.cend(); ++itc)
        if (IsInt(*itc))
            (*itc) = SimplifyToInt((*itc));
    if (IsInt(offset))
        offset = SimplifyToInt(offset);
    for (vB::iterator itb = B.begin(); itb != B.end(); ++itb) {
        if (IsInt((*itb).second))
            (*itb).second = SimplifyToInt((*itb).second);
        int index = distance(B.begin(), itb);
        for (vP::iterator itp = P.begin(); itp != P.end(); ++itp)
            if (IsInt((*itp).at(index)))
                (*itp).at(index) = SimplifyToInt((*itp).at(index));
    }
}
