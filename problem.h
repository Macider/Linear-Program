#pragma once
#include <float.h>
#include <algorithm>
#include <cassert>
#include <iostream>
#include <regex>
#include <string>
#include <vector>
using namespace std;

// 数据结构区
enum MaxOrMin {
    Max,
    Min,
};
enum Range {
    UNLIMITED,
    LARGE_EQUAL,
    SMALL_EQUAL,
    EQUAL,
};

typedef pair<Range, double> tRightSide;
typedef vector<tRightSide> vRange;
typedef vector<double> vValue;
typedef vector<vector<double>> vP;

struct tVar {
    string name;
    tRightSide rhs;
    tVar(string name, tRightSide rhs) {
        this->name = name;
        this->rhs = rhs;
    }
};

typedef vector<tVar> vX;

class Problem {
   public:
    MaxOrMin maxOrmin;  // 优化目标
    vX X;               // 决策变量信息
    // vRange Xrange;      // 变量限制范围，可以考虑与X合并
    vP P;               // 工艺系数 P[i_n][i_m]
    vValue C;           // 目标函数系数
    vRange B;           // 约束条件右边项 //B>=0
    double offset = 0;  // 目标函数修正值
    Problem() {}
    Problem(const Problem& other)
        : X(other.X), P(other.P), C(other.C), B(other.B) {
        this->maxOrmin = other.maxOrmin;
        this->offset = other.offset;
    }
    Problem* Dualize();
    Problem* Standandlize();
    bool IsStandard();
    void OutputPblm();

    void OutputVar();
    void OutputTarget();
    void OutputConstraint();
};

// 函数区
inline bool equals(double d1, double d2) {
    return (d1 - d2 < DBL_EPSILON) && (d2 - d1 < DBL_EPSILON);
}

Problem* InputPblm() {
    Problem* pblm = new Problem();
    string namePattern = "[a-zA-Z_](\\w)*";  // C++变量格式
    string rangePattern = "(\\+)|(-)|(\\?)|((>|<)=(\\s)*((\\+)|(-))?(\\d)+)";
    string constraintPattern = "((>|<)?=(\\s)*((\\+)|(-))?(\\d)+)";
    regex var_name(namePattern);          // 变量名
    regex var_range(rangePattern);        // 变量范围     // 暂时只支持整数
    regex constraint(constraintPattern);  // 约束

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

            tVar var(name, rhs);
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
            pblm->maxOrmin = Max;
        else if (first4 == "min ")
            pblm->maxOrmin = Min;
        else
            cout << "minmax Error!" << endl;

        // 分析目标函数
        result.erase(remove(result.begin(), result.end(), '*'), result.end());  // 删除*
        result.erase(remove(result.begin(), result.end(), ' '), result.end());  // 删除空格
        sregex_iterator name_pos(result.cbegin(), result.cend(), var_name);
        for (sregex_iterator end; name_pos != end; ++name_pos) {
            // 假定是第i号变量

            // 寻找并记录系数C[i]
            double c;  // c[i]
            string prefix = name_pos->prefix().str();
            size_t last_not_num = prefix.find_last_not_of("0123456789");  // 暂时只支持整数
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
            } else
                c = stod(prefix.substr(last_not_num));  // x1- 3name>=2;

            // 找到对应的变量并修改C[i]，即确定i
            string name = name_pos->str();
            vX::iterator it;
            for (it = pblm->X.begin(); it != pblm->X.end(); it++)
                if ((*it).name == name)
                    break;
            if (it == pblm->X.end())
                cout << "var not found,error!" << endl;
            int index = distance(pblm->X.begin(), it);
            pblm->C.at(index) = c;
        }
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
                double p;  // P[i][j]
                string prefix = name_pos->prefix().str();
                size_t last_not_num = prefix.find_last_not_of("0123456789");  // 暂时只支持整数
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
                } else
                    p = stod(prefix.substr(last_not_num));  // x1- 3name>=2;

                // 找到对应的变量并修改P[i][j]，即确定i
                string name = name_pos->str();
                vX::iterator itx;
                for (itx = pblm->X.begin(); itx != pblm->X.end(); itx++)
                    if ((*itx).name == name)
                        break;
                if (itx == pblm->X.end())
                    cout << "var not found,error!" << endl;
                int index = distance(pblm->X.begin(), itx);
                // cout << "index is " << index << ", line_num is " << line_num << endl;
                pblm->P.at(index).at(line_num) = p;
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
    if (maxOrmin == Max)
        cout << "max " << flush;
    else if (maxOrmin == Min)
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
    for (vRange::const_iterator itb = B.cbegin(); itb != B.cend(); ++itb) {  // 按约束遍历
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
    vRange::const_iterator itb = B.cbegin();
    for (int index = 0; itb != B.cend(); ++itb, ++index) {
        Range range;
        if ((*itb).first == EQUAL)
            range = UNLIMITED;
        else if ((*itb).first == SMALL_EQUAL) {
            if (maxOrmin == Max)
                range = LARGE_EQUAL;
            else if (maxOrmin == Min)
                range = SMALL_EQUAL;
        } else if ((*itb).first == LARGE_EQUAL) {
            if (maxOrmin == Max)
                range = SMALL_EQUAL;
            else if (maxOrmin == Min)
                range = LARGE_EQUAL;
        } else
            cout << "Range Error!" << endl;
        tRightSide rhs(range, 0);
        tVar var("line" + to_string(index), rhs);
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
            if (maxOrmin == Max)
                range = SMALL_EQUAL;
            else if (maxOrmin == Min)
                range = LARGE_EQUAL;
        } else if ((*itx).rhs.first == LARGE_EQUAL) {
            if (maxOrmin == Max)
                range = LARGE_EQUAL;
            else if (maxOrmin == Min)
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
    if (maxOrmin == Max)
        dual->maxOrmin = Min;
    else
        dual->maxOrmin = Max;
    return dual;
}

Problem* Problem::Standandlize() {
    // 生成一个标准型
    Problem* pblm = new Problem(*this);

    // 变为求目标函数最大化
    if (maxOrmin == Min) {
        for (vValue::iterator itc = pblm->C.begin(); itc != pblm->C.end(); itc++)
            *itc = -*itc;
        pblm->maxOrmin = Max;
        pblm->offset = -pblm->offset;
    }

    // 修正变量取值范围
    for (vX::iterator itx = pblm->X.begin(); itx != pblm->X.end(); ++itx) {
        double bound = (*itx).rhs.second;
        string bound_str = to_string(bound);
        if (equals(bound, floor(bound)))  // bound = 3.0001
            bound_str = to_string(int(bound));
        else if (equals(bound, floor(bound) + 1))  // bound = 2.9999
            bound_str = to_string(int(bound) + 1);
        regex_replace(bound_str, std::regex("\\."), "dot");
        int index = distance(pblm->X.begin(), itx);
        if ((*itx).rhs.first == EQUAL)
            continue;

        // x>=a,x'=x-a,x=x'+a
        else if ((*itx).rhs.first == LARGE_EQUAL) {
            if (equals(bound, 0))
                continue;
            (*itx).name = "__" + (*itx).name + "_minus_" + bound_str + "__";  // x'=x-a,x=x'+a
            pblm->offset += bound * pblm->C.at(index);
            vValue::iterator itp_in = pblm->P.at(index).begin();
            vRange::iterator itb = pblm->B.begin();
            for (; itp_in != pblm->P.at(index).end() && itb != pblm->B.end(); ++itp_in, ++itb)
                (*itb).second -= bound * (*itp_in);
            (*itx).rhs = make_pair(LARGE_EQUAL, 0);
        }

        // x<=a,x'=-x+a,x=-x'+a
        else if ((*itx).rhs.first == SMALL_EQUAL) {
            (*itx).name = "__minus_" + (*itx).name + "_plus_" + bound_str + "__";  // x'=-x+a,x=-x'+a
            pblm->offset += bound * pblm->C.at(index);
            pblm->C.at(index) = -pblm->C.at(index);  // Ci取负
            vValue::iterator itp_in = pblm->P.at(index).begin();
            vRange::iterator itb = pblm->B.begin();
            for (; itp_in != pblm->P.at(index).end() && itb != pblm->B.end(); ++itp_in, ++itb) {
                (*itb).second -= bound * (*itp_in);
                (*itp_in) = -(*itp_in);  // Pi全部取负
            }
            (*itx).rhs = make_pair(LARGE_EQUAL, 0);
        }

        // x unlimited,x = x_1-x_2
        else if ((*itx).rhs.first == UNLIMITED) {
            string name = (*itx).name;
            (*itx).rhs = make_pair(LARGE_EQUAL, 0);
            (*itx).name = "__" + name + "_part1__";
            tRightSide rhs(LARGE_EQUAL, 0);
            tVar var("__" + name + "_part2__", rhs);
            itx = pblm->X.emplace(itx + 1, var);                               // 迭代器可能失效，需要重新获取
            pblm->C.emplace(pblm->C.begin() + index + 1, -pblm->C.at(index));  // Ci' = -Ci
            pblm->P.emplace(pblm->P.begin() + index + 1, pblm->P.at(index));   // Pi' = Pi
            vValue::iterator itp_in = pblm->P.at(index + 1).begin();
            for (; itp_in != pblm->P.at(index + 1).end(); ++itp_in)
                (*itp_in) = -(*itp_in);  // Pi' = -Pi' = -Pi
        } else
            cout << "Error" << (*itx).rhs.first << endl;
    }

    // 保证约束条件均为等于某非负值
    for (vRange::iterator itb = pblm->B.begin(); itb != pblm->B.end(); itb++) {
        int index = distance(pblm->B.begin(), itb);

        // 保持B的非负性
        if ((*itb).second < 0) {
            (*itb).second = -(*itb).second;
            if ((*itb).first == LARGE_EQUAL)
                (*itb).second = SMALL_EQUAL;
            else if ((*itb).second == SMALL_EQUAL)
                (*itb).first = LARGE_EQUAL;
            int Psz = pblm->P.size();
            for (int i = 0; i < Psz; ++i)
                pblm->P.at(i).at(index) = -pblm->P.at(i).at(index);
        }

        // 引入剩余变量remain variable
        if ((*itb).first == LARGE_EQUAL) {
            tRightSide rhs(LARGE_EQUAL, 0);
            tVar var("__remain_" + to_string(index) + "__", rhs);
            pblm->X.push_back(var);
            pblm->C.push_back(0.0);
            pblm->P.push_back(vector(pblm->B.size(), 0.0));
            pblm->P.back().at(index) = -1;
            (*itb).first = EQUAL;
        }
        // 引入松弛变量flabby variable
        else if ((*itb).first == SMALL_EQUAL) {
            tRightSide rhs(LARGE_EQUAL, 0);
            tVar var("__flabby_" + to_string(index) + "__", rhs);
            pblm->X.push_back(var);
            pblm->C.push_back(0.0);
            pblm->P.push_back(vector(pblm->B.size(), 0.0));
            pblm->P.back().at(index) = 1;
            (*itb).first = EQUAL;
        }
    }

    return pblm;
}

bool Problem::IsStandard() {
    // 检验对象是否已标准化

    // 目标函数取最大化
    if (maxOrmin != Max)
        return 0;

    // 所有变量取值都为非负
    for (vX::const_iterator itx = X.cbegin(); itx != X.cend(); ++itx)
        if ((*itx).rhs.first == LARGE_EQUAL || !equals((*itx).rhs.second, 0))
            return 0;

    // 所有约束都为等于某非负值
    for (vRange::const_iterator itb = B.cbegin(); itb != B.cend(); ++itb)
        if ((*itb).first != EQUAL || (*itb).second < 0)
            return 0;

    cout << "Problem is Standard!" << endl;
    return 1;
}
