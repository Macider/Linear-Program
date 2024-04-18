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
    // cout << "以下为单纯形法" << endl;
    // Problem* pblm0 = SimplexMethod(pblm);
    // pblm0->OutputResult();
    // cout << endl;

    // cout << "以下为对偶单纯形法" << endl;
    // Problem* pblm1 = DualSimplexMethod(pblm);
    // pblm1->OutputResult();
    // cout << endl;

    // cout << "以下为分支定界法" << endl;
    // Problem* pblm2 = BranchBoundMethod(pblm);
    // pblm2->OutputResult();
    // cout << endl;

    // cout << "以下为割平面法" << endl;
    // Problem* pblm3 = CutPlaneMethod(pblm);
    // pblm3->OutputResult();
    // cout << endl;

    // cout << "以下为隐枚举法" << endl;
    // Problem* pblm4 = ImplicitEnumerationMethod(pblm);
    // pblm4->OutputResult();
    // cout << endl;

    cout << "以下为匈牙利法" << endl;
    Problem* pblm5 = HungarianMethod(pblm);

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
