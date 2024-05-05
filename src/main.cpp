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
    cout << "Pblm ����" << endl;
    pblm->OutputPblm();

    // Problem* dualPblm = pblm->Dualize();
    // cout << "dualPblm ����" << endl;
    // dualPblm->OutputPblm();

    clock_t clock0 = clock();
    cout << "����Ϊ�����η�" << endl;
    Problem* pblm0 = SimplexMethod(pblm);
    pblm0->OutputResult();
    cout << endl;
    clock_t clock1 = clock();
    double timeUsed1 = ((double)(clock1 - clock0)) / CLOCKS_PER_SEC;

    cout << "�����η���Ҫ��ʱ��Ϊ" << timeUsed1 << endl;

    cout << "����Ϊ��ż�����η�" << endl;
    Problem* pblm1 = DualSimplexMethod(pblm);
    pblm1->OutputResult();
    cout << endl;
    clock_t clock2 = clock();
    double timeUsed2 = ((double)(clock2 - clock1)) / CLOCKS_PER_SEC;

    cout << "�����η�ʹ����" << timeUsed1 << "��ʱ�䣬��ż�����η�ʹ����" << timeUsed2 << "��ʱ��"
         << "������ǰ�ߵ�" << timeUsed2 / timeUsed1 << "��" << endl;

    cout << "�����ż�����η�" << endl;
    Problem* pblm1_ = SimplexMethod(pblm1);
    pblm1_->OutputResult();

    // cout << "����Ϊ��֧���編" << endl;
    // Problem* pblm2 = BranchBoundMethod(pblm);
    // pblm2->OutputResult();
    // cout << endl;

    // cout << "����Ϊ��ƽ�淨" << endl;
    // Problem* pblm3 = CutPlaneMethod(pblm);
    // pblm3->OutputResult();
    // cout << endl;

    // cout << "����Ϊ��ö�ٷ�" << endl;
    // Problem* pblm4 = ImplicitEnumerationMethod(pblm);
    // pblm4->OutputResult();
    // cout << endl;

    // cout << "����Ϊ��������" << endl;
    // Problem* pblm5 = HungarianMethod(pblm);
    // pblm5->OutputResult();

    return 0;
}

// int main(){
//     Widget widget(800, 800);
//     widget.init();
//  	//setaspectratio(0.9, 0.9); //��ʾ�����˿���û����
//  	//Resize(NULL,1000,1000);
//     widget.run();
//     widget.close();
//     return 0;
// }
