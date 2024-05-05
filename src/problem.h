/*
 * ����������ݽṹ
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

// ���ݽṹ��
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
    MaxOrMin maxOrMin = Max;  // �Ż�Ŀ��
    vX X;                     // ���߱�����Ϣ
    vP P;                     // ����ϵ�� P[i_n][i_m]
    vValue C;                 // Ŀ�꺯��ϵ��
    vB B;                     // Լ�������ұ��� //B>=0
    double offset = 0;        // Ŀ�꺯������ֵ��ע������һ���ܱ�ʾ�����
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

// ������
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
    // cout << "equals(d, floor(d))�Ľ��Ϊ" << equals(d, floor(d)) << endl;
    // cout << "equals(d, 1 + floor(d))�Ľ��Ϊ" << equals(d, 1 + floor(d)) << endl;
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
    string namePattern = "[a-zA-Z_](\\w)*";  // C++������ʽ
    string rangePattern = "(\\+)|(-)|(\\?)|((>|<)=(\\s)*((\\+)|(-))?((\\d)+(\\.(\\d)+)?))";
    // (\\+)|(-)|(\\?)��������+-����?�е�һ����(>|<)=(\\s)*Ϊ(��)�Ⱥż�����Ŀո�((\\+)|(-))?((\\d)+(\\.(\\d)+)?)��һ�����԰������ŵ���
    string constraintPattern = "((>|<)?=(\\s)*((\\+)|(-))?((\\d)+(\\.(\\d)+)?))";
    string factorPattern = "(\\d)+(\\.(\\d)+)?";  // һ��������/�������ԣ������ȵ�������
    regex var_name(namePattern);                  // ������
    regex var_range(rangePattern);                // ������Χ
    regex constraint(constraintPattern);          // Լ���ұ���
    regex factor(factorPattern);

    // �������
    {
        cout << "Step1:�����������ȡֵ��Χ" << endl;
        cout << "����x1>=0,x2<=3,x3+,x4-,x5?;" << endl;
        cout << "��\",\"�ָ�����\";\"��β" << endl;
        cout << ">=0����дΪ+, <=0����дΪ-, ��Լ����?��ʾ" << endl;

        // ����
        string result;
        getline(cin, result, ';');
        replace(result.begin(), result.end(), '\n', ' ');  // ���������ո�

        // ƥ�������ȡֵ��Χ
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

    // ���������������ݽṹ
    pblm->C.resize(pblm->X.size());
    pblm->P.resize(pblm->X.size());

    // ����Ŀ�꺯��
    {
        cout << "Step2:������Ŀ�꺯��" << endl;
        cout << "����max 2*x1-x2+4x3;" << endl;
        cout << "��\"max\"��\"min\"��ͷ����\";\"��β" << endl;

        // ����
        string result;
        getline(cin, result, ';');
        replace(result.begin(), result.end(), '\n', ' ');  // ���������ո�
        result.erase(0, result.find_first_not_of(' '));    // ȥ��ǰ���ո�

        // �ж����/��С��
        string first4 = result.substr(0, 4);
        result.erase(0, 4);
        transform(first4.begin(), first4.end(), first4.begin(), ::tolower);  // תСд
        if (first4 == "max ")
            pblm->maxOrMin = Max;
        else if (first4 == "min ")
            pblm->maxOrMin = Min;
        else
            cout << "minmax Error!" << endl;

        // ����Ŀ�꺯���ı�������
        result.erase(remove(result.begin(), result.end(), '*'), result.end());  // ɾ��*
        result.erase(remove(result.begin(), result.end(), ' '), result.end());  // ɾ���ո�
        sregex_iterator name_pos(result.cbegin(), result.cend(), var_name);
        for (sregex_iterator end; name_pos != end; ++name_pos) {
            // �ٶ��ǵ�i�ű���

            // Ѱ�Ҳ���¼ϵ��C[i]
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
                    c = stod(prefix.substr(last_not_num));                       // ������last_not_num
                else
                    cout << "Ci Error!" << endl;
            }

            // �ҵ���Ӧ�ı������޸�C[i]����ȷ��i
            string name = name_pos->str();
            vX::iterator itx;
            for (itx = pblm->X.begin(); itx != pblm->X.end(); ++itx)
                if ((*itx).name == name)
                    break;
            if (itx == pblm->X.end())
                cout << "var name is "<< name <<", not found, error!" << endl;
            int index = distance(pblm->X.begin(), itx);
            pblm->C.at(index) += c;  // ���ǵ������� x1+x2+x1�����
        }

        // ����Ŀ�꺯���п��ܴ��ڵĳ�����
        // �ص�Ϊǰ��Ϊ+����-����null������Ϊ+����-����null
        sregex_iterator factor_pos(result.cbegin(), result.cend(), factor);
        for (sregex_iterator end; factor_pos != end; ++factor_pos) {
            // Ѱ�Ҳ���¼������ofst
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

    // ����Լ������
    {
        cout << "Step3:������Լ������" << endl;
        cout << "����x1+x2-3x3<=9,\nx1-2x2+x3=7;" << endl;
        cout << "Լ������\",\"���н��зָ�����\";\"��β" << endl;

        // ��������
        string result, line;
        getline(cin, result, ';');
        replace(result.begin(), result.end(), '\n', ',');  // ������������

        // ����Լ���������������ݽṹ
        int m = 0;
        stringstream s(result);
        while (getline(s, line, ','))
            if (!line.empty())
                m++;
        pblm->B.resize(m);
        for (vP::iterator itp = pblm->P.begin(); itp != pblm->P.end(); ++itp)
            (*itp).resize(m);

        // ��������
        stringstream ss(result);
        int line_num = 0;
        while (getline(ss, line, ',')) {
            if (line.empty())  // �����������ŵ��»��й������
                continue;
            // cout << "line is " << line << endl;
            line.erase(remove(line.begin(), line.end(), '*'), line.end());  // ɾ��*
            line.erase(remove(line.begin(), line.end(), ' '), line.end());  // ɾ���ո�

            // ʶ��Լ�������ұ���
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

            // ʶ��Լ�����������
            sregex_iterator name_pos(line.cbegin(), line.cend(), var_name);
            for (sregex_iterator end; name_pos != end; ++name_pos) {
                // �ٶ��ǵ�i�ű�������j�ŷ���
                // Ѱ�Ҳ���¼ϵ��P[i][j]
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
                        p = stod(prefix.substr(last_not_num));                       // ������last_not_num
                    else
                        cout << "Pij Error!" << endl;
                }

                // �ҵ���Ӧ�ı������޸�P[i][j]����ȷ��i
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
    OutputVar();         // �����������Χ
    OutputTarget();      // ���Ŀ�꺯��
    OutputConstraint();  // ���Լ������
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
    cout << "��" << X.size() << "������" << endl;
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
    for (vB::const_iterator itb = B.cbegin(); itb != B.cend(); ++itb) {  // ��Լ������
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
    cout << "��" << B.size() << "��Լ������" << endl;
}

Problem* Problem::Dualize() {
    int m = B.size(), n = X.size();
    Problem* dual = new Problem();

    // P ת��
    dual->P.resize(m);
    for (vP::const_iterator itp_out = P.cbegin(); itp_out != P.cend(); ++itp_out) {
        vValue::const_iterator itp_in = (*itp_out).cbegin();
        for (int index = 0; itp_in != (*itp_out).cend(); ++itp_in, ++index)
            dual->P.at(index).push_back(*itp_in);
    }

    // Լ�������ұ�����ֵ����Ŀ�꺯��ϵ��
    // Լ���������Ⱥž�������ȡֵ��Χ(Max�෴��Min��ͬ)
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

    // Ŀ�꺯��ϵ������Լ�������ұ�����ֵ
    // ����ȡֵ��Χ����Լ���������Ⱥ�(Max��ͬ��Min�෴)
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

    // ����ȡֵ��Χ����Լ�������ұ�����ֵ
    dual->offset = offset;
    for (vX::const_iterator itx = X.cbegin(); itx != X.cend(); ++itx) {
        if (equals(0.0, (*itx).rhs.second))
            continue;
        int index = distance(X.cbegin(), itx);
        vValue::iterator itc_ = dual->C.begin();
        for (int i = 0; itc_ != dual->C.end(); ++itc_, ++i)
            (*itc_) -= (*itx).rhs.second * P.at(index).at(i);
        dual->offset += (*itx).rhs.second * C.at(index);  // ��ż������Ҫȡ��
    }

    // Ŀ�꺯���仯
    if (maxOrMin == Max)
        dual->maxOrMin = Min;
    else
        dual->maxOrMin = Max;
    return dual;
}

Problem* Problem::Standardlize(WorkMode mode) {
    // �ֱ�����Ⱥź�bֵ��Χ��Ĭ��ǰ��Ϊȡ�ȣ�����Ϊ�Ǹ�
    // ����һ����׼��
    cout << "��׼����" << endl;
    Problem* pblm = new Problem(*this);

    // ��Ϊ��Ŀ�꺯�����
    pblm->ChangeMinMax();
    // if (maxOrMin == Min) {
    //     for (vValue::iterator itc = pblm->C.begin(); itc != pblm->C.end(); ++itc)
    //         *itc = -*itc;
    //     pblm->maxOrMin = Max;
    //     pblm->offset = -pblm->offset;
    // }

    // ��������ȡֵ��Χ
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
    //         cout << index << "�ű������ڵ���" << bound << "��Ҫ����" << endl;
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
    //         cout << index << "�ű���С�ڵ���" << bound << "��Ҫ����" << endl;
    //         (*itx).name = "__minus_" + (*itx).name + "_plus_" + bound_str + "__";  // x'=-x+a,x=-x'+a
    //         pblm->offset += bound * pblm->C.at(index);
    //         pblm->C.at(index) = -pblm->C.at(index);  // Ciȡ��
    //         vValue::iterator itp_in = pblm->P.at(index).begin();
    //         vB::iterator itb = pblm->B.begin();
    //         for (; itp_in != pblm->P.at(index).end() && itb != pblm->B.end(); ++itp_in, ++itb) {
    //             (*itb).second -= bound * (*itp_in);
    //             (*itp_in) = -(*itp_in);  // Piȫ��ȡ��
    //         }
    //         (*itx).rhs = make_pair(LARGE_EQUAL, 0);
    //     }
    //     // x unlimited,x = x_1-x_2
    //     else if ((*itx).rhs.first == UNLIMITED) {
    //         cout << index << "�ű���ȡֵ��Χ�����ƣ���Ҫ����" << endl;
    //         string name = (*itx).name;
    //         (*itx).rhs = make_pair(LARGE_EQUAL, 0);
    //         (*itx).name = "__" + name + "_part1__";
    //         tRightSide rhs(LARGE_EQUAL, 0);
    //         tVar var("__" + name + "_part2__", rhs, 0.0);
    //         itx = pblm->X.emplace(itx + 1, var);                               // ����������ʧЧ����Ҫ���»�ȡ
    //         pblm->C.emplace(pblm->C.begin() + index + 1, -pblm->C.at(index));  // Ci' = -Ci
    //         pblm->P.emplace(pblm->P.begin() + index + 1, pblm->P.at(index));   // Pi' = Pi
    //         vValue::iterator itp_in = pblm->P.at(index + 1).begin();
    //         for (; itp_in != pblm->P.at(index + 1).end(); ++itp_in)
    //             (*itp_in) = -(*itp_in);  // Pi' = -Pi' = -Pi
    //     } else
    //         cout << "Error" << (*itx).rhs.first << endl;
    // }

    // �޸�Լ���������Ⱥţ��޸��Ҳ�ֵ
    // �ò�������ú������
    for (vB::iterator itb = pblm->B.begin(); itb != pblm->B.end(); ++itb) {
        int index = distance(pblm->B.begin(), itb);
        // cout << "index is " << index << endl;
        // Ĭ�ϱ���B�ķǸ���
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
        // ����ô�����
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


        /*  �ڶ�ż�����η�ʱ������任��ͨ���������Լ�������ӻ�����
        if ((*itb).first == EQUAL) {
            if (mode == SIMPLEX)
                continue;
            else if (mode == DUAL_SIMPLEX) {
                // ���ȼ���Ƿ��Ѵ����ɳ�/ʣ�����
                int n = pblm->X.size();
                int m = pblm->B.size();
                int already = -1;
                for (int i_n = 0; i_n < n; i_n++) {
                    if (pblm->C.at(i_n))  // Ci!=0˵�������ɳ�/ʣ�����
                        continue;
                    if (equals(pblm->P.at(i_n).at(index), 0))  // ��Ԫ��==0˵�������ɳ�/ʣ�����
                        continue;
                    for (int i_m = 0; i_m < m; i_m++) {
                        if (pblm->P.at(i_n).at(i_m) && i_m != index)  // ��������Ԫ��!=0˵�������ɳ�/ʣ�����
                            break;
                        if (i_m + 1 == m)
                            already = i_n;
                    }
                }
                // cout << "already is " << already << endl;
                // �Ѿ�����ĳ��������Ϊ�ɳڱ���
                if (already != -1) {
                    // ��Լ���淶��ΪsigmaX + x' = b
                    double eleChosen = pblm->P.at(already).at(index);
                    pblm->B.at(index).second /= eleChosen;
                    for (int i_n = 0; i_n < n; i_n++)
                        pblm->P.at(i_n).at(index) /= eleChosen;
                }
                // �ֶ�����ɳڱ���
                else {
                    // sigmaX = b -> sigmaX + x' = b && -sigmaX + x'' = -b
                    // ��Լ��ȡ�������ɳڱ���1
                    tRightSide rhs(LARGE_EQUAL, 0);
                    pblm->B.push_back(pblm->B.at(index));
                    itb = pblm->B.begin() + index;  // push֮�����������ʧЧ����Ҫ���»�ȡ
                    for (int i_n = 0; i_n < n; i_n++)
                        pblm->P.at(i_n).push_back(-pblm->P.at(i_n).at(index));
                    int index1 = pblm->B.size() - 1;
                    tVar var1("__flabby_" + to_string(index1) + "__", rhs, 0.0);
                    pblm->X.push_back(var1);
                    pblm->C.push_back(0.0);
                    pblm->P.push_back(vector<double>(pblm->B.size(), 0.0));
                    pblm->P.back().at(index1) = +1;
                    // pblm->OutputPblm();
                    // ԭԼ���������ɳڱ���2
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

        // ��������ɳڱ������ʡ�ʱ�������⴦��
        // ���Ⱥ�Ϊ��ʱ
        if ((*itb).first == LARGE_EQUAL) {
            // һ�����������ʣ�����remain variable
            if (mode == SIMPLEX) {
                tRightSide rhs(LARGE_EQUAL, 0);
                tVar var("__remain_" + to_string(index) + "__", rhs, 0.0);
                pblm->X.push_back(var);
                pblm->C.push_back(0.0);
                pblm->P.push_back(vector<double>(pblm->B.size(), 0.0));
                pblm->P.back().at(index) = -1;
                (*itb).first = EQUAL;
            }
            // ��ż��׼��ʱ������ȡ���������ɳڱ���
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
        // ���Ⱥ�Ϊ��ʱ�������ɳڱ���flabby variable
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
    // DUAL_SIMPLEXģʽ�£�
    // =b -> =-b,
    // <=b -> +x=b,
    // <=-b -> +x=-b,
    // >=b -> +x=-b,
    // <=b -> +x=b;

    return pblm;
}

// �����Ż�����
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

// ����Լ�������ұ����ֵ
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
        // ����B�ķǸ��Ի������
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

// ����Լ���������Ⱥ�
void Problem::ChangeBRange(Range range) {
    int m = B.size();        // ��һ������n��仯
    if (range == UNLIMITED)  // �����޸�
        return;
    else if (range == EQUAL)
        for (vB::iterator itb = B.begin(); itb != B.end(); ++itb) {
            int index = distance(B.begin(), itb);
            // ���Ⱥ�Ϊ��ʱ������ʣ�����remain variable
            if ((*itb).first == LARGE_EQUAL) {
                // һ�����������ʣ�����remain variable
                tRightSide rhs(LARGE_EQUAL, 0);
                tVar var("__remain_" + to_string(index) + "__", rhs, 0.0);
                X.push_back(var);
                C.push_back(0.0);
                P.push_back(vector<double>(m, 0.0));
                P.back().at(index) = -1;
                (*itb).first = EQUAL;

            }
            // ���Ⱥ�Ϊ��ʱ�������ɳڱ���flabby variable
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

// ���ı���ȡֵ��Χ
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
        if ((*itx).rhs.first == EQUAL)  // �Ѿ�ָ��ȡֵ�����Һ��ԣ�����
            continue;
        // x>=a,x'=x-a,x=x'+a
        else if ((*itx).rhs.first == LARGE_EQUAL) {
            if (equals(bound, 0))
                continue;
            cout << index << "�ű������ڵ���" << bound << "��Ҫ����" << endl;
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
            cout << index << "�ű���С�ڵ���" << bound << "��Ҫ����" << endl;
            (*itx).name = "__minus_" + (*itx).name + "_plus_" + bound_str + "__";  // x'=-x+a,x=-x'+a
            offset += bound * C.at(index);
            C.at(index) = -C.at(index);  // Ciȡ��
            vValue::iterator itp_in = P.at(index).begin();
            vB::iterator itb = B.begin();
            for (; itp_in != P.at(index).end() && itb != B.end(); ++itp_in, ++itb) {
                (*itb).second -= bound * (*itp_in);
                (*itp_in) = -(*itp_in);  // Piȫ��ȡ��
            }
            (*itx).rhs = make_pair(LARGE_EQUAL, 0);
        }
        // x unlimited,x = x_1-x_2
        else if ((*itx).rhs.first == UNLIMITED) {
            cout << index << "�ű���ȡֵ��Χ�����ƣ���Ҫ����" << endl;
            string name = (*itx).name;
            (*itx).rhs = make_pair(LARGE_EQUAL, 0);
            (*itx).name = "__" + name + "_part1__";
            tRightSide rhs(LARGE_EQUAL, 0);
            tVar var("__" + name + "_part2__", rhs, 0.0);
            itx = X.emplace(itx + 1, var);                   // ����������ʧЧ����Ҫ���»�ȡ
            C.emplace(C.begin() + index + 1, -C.at(index));  // Ci' = -Ci
            P.emplace(P.begin() + index + 1, P.at(index));   // Pi' = Pi
            vValue::iterator itp_in = P.at(index + 1).begin();
            for (; itp_in != P.at(index + 1).end(); ++itp_in)
                (*itp_in) = -(*itp_in);  // Pi' = -Pi' = -Pi
        } else
            cout << "Error" << (*itx).rhs.first << endl;
    }
}

// ��������Ƿ��ѱ�׼����Ӧ�ô������ȽϺ���
bool Problem::IsStandard() {
    // Ŀ�꺯��ȡ���
    if (maxOrMin != Max)
        return 0;

    // ���б���ȡֵ��Ϊ�Ǹ�
    for (vX::const_iterator itx = X.cbegin(); itx != X.cend(); ++itx)
        if ((*itx).rhs.first != LARGE_EQUAL || !equals((*itx).rhs.second, 0))
            return 0;

    // ����Լ����Ϊ����ĳ�Ǹ�ֵ
    for (vB::const_iterator itb = B.cbegin(); itb != B.cend(); ++itb)
        if ((*itb).first != EQUAL || (*itb).second < 0)
            return 0;

    // cout << "Problem is Standard!" << endl;
    return 1;
}

// ���������Xȡֵ�õ����
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

// ���������Xȡֵ������
void Problem::OutputResult() {
    if (result == NO_SOLUTION) {
        cout << "�������޿��н�" << endl;
        return;
    } else if (result == UNBOUNDED) {
        cout << "���������н����Ž�" << endl;
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
        if (itx + 1 != X.cend())  // �˴��ж�ѭ���Ƿ����
            cout << ", ";
    }
    cout << ")" << endl;
}

// ��⵱ǰ��Xȡֵ�Ƿ��������Ҫ��
bool Problem::TestConstraint() {
    for (vB::const_iterator itb = B.cbegin(); itb != B.cend(); ++itb) {  // ��Լ������
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
        cout << "ԭԼ��Ϊ";
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

// ���0.999!=1����
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
