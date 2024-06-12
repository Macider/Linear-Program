#include <time.h>
#include "branch_bound_method.h"
#include "cut_plane_method.h"
#include "dual_simplex_method.h"
#include "hungarian_method.h"
#include "implicit_enumeration_method.h"
#include "problem.h"
#include "simplex_method.h"
// #include "button_textbox.h"

using namespace std;

int main() {
    Problem* pblm = InputPblm();
    // cout << "Pblm 如下" << endl;
    // pblm->OutputPblm();

    // Problem* dualPblm = pblm->Dualize();
    // cout << "dualPblm 如下" << endl;
    // dualPblm->OutputPblm();

    clock_t clock0 = clock();
    cout << "以下为单纯形法" << endl;
    Problem* pblm0 = SimplexMethod(pblm);
    pblm0->OutputResult();
    cout << endl;
    clock_t clock1 = clock();
    double timeUsed0 = ((double)(clock1 - clock0)) / CLOCKS_PER_SEC;
    // cout << "单纯形法需要的时间为" << timeUsed0 << endl;

    cout << "以下为对偶单纯形法" << endl;
    // Problem* pblm1 = DualSimplexMethod(pblm);
    // pblm1->OutputResult();
    // cout << endl;
    clock_t clock2 = clock();
    double timeUsed1 = ((double)(clock2 - clock1)) / CLOCKS_PER_SEC;
    // cout << "对偶单纯形法需要的时间为" << timeUsed1 << endl;

    // cout << "单纯形法使用了" << timeUsed0 << "的时间，对偶单纯形法使用了" << timeUsed1 << "的时间"
    //      << "后者是前者的" << timeUsed1 / timeUsed0 << "倍" << endl;

    // cout << "验算对偶单纯形法" << endl;
    // Problem* pblm1_ = SimplexMethod(pblm1);
    // pblm1_->OutputResult();

    cout << "以下为分支定界法" << endl;
    // Problem* pblm2 = BranchBoundMethod(pblm);
    // pblm2->OutputResult();
    // cout << endl;
    clock_t clock3 = clock();
    double timeUsed2 = ((double)(clock3 - clock2)) / CLOCKS_PER_SEC;  
    cout << "分支定界法需要的时间为" << timeUsed2 << endl;

    cout << "以下为割平面法" << endl;
    // Problem* pblm3 = CutPlaneMethod(pblm);
    // pblm3->OutputResult();
    // cout << endl;
    clock_t clock4 = clock();
    double timeUsed3 = ((double)(clock4 - clock3)) / CLOCKS_PER_SEC;
    cout << "割平面法需要的时间为" << timeUsed3 << endl;

    cout << "以下为隐枚举法" << endl;
    // Problem* pblm4 = ImplicitEnumerationMethod(pblm);
    // pblm4->OutputResult();
    // cout << endl;
    clock_t clock5 = clock();
    double timeUsed4 = ((double)(clock5 - clock4)) / CLOCKS_PER_SEC;
    cout << "隐枚举法需要的时间为" << timeUsed4 << endl;

    cout << "以下为匈牙利法" << endl;
    // Problem* pblm5 = HungarianMethod(pblm);
    // pblm5->OutputResult();
    // cout << endl;
    clock_t clock6 = clock();
    double timeUsed5 = ((double)(clock6 - clock5)) / CLOCKS_PER_SEC;
    cout << "隐枚举法需要的时间为" << timeUsed5 << endl;

    return 0;
}

// int main(){
//     Widget widget(800, 800);
//     widget.init();
//  	//setaspectratio(0.9, 0.9); //显示缩放了控制没缩放
//  	//Resize(NULL,1000,1000);
//     widget.run();
//     widget.close();
//     return 0;
// }
