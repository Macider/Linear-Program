#include <math.h>
#include <iostream>
#include "./problem.h"
#include "simplex.h"

using namespace std;

int main() {
    // StandardForm* stdFm = InputStdFm();
    // stdFm->OutputStdFm();
    // SimplexMethod(stdFm);
    Problem* pblm = InputPblm();
    // pblm->OutputPblm();
    Problem* stdfm = pblm->Standandlize();
    // stdfm->OutputPblm();
    SimplexMethod(stdfm);

    cout << "以下为对偶问题" << endl;
    Problem* dual = pblm->Dualize();
    dual->OutputPblm();
    Problem* stdDual = dual->Standandlize();
    stdDual->OutputPblm();
    SimplexMethod(stdDual);
    // Problem* DDual = dual->Dualize();
    // DDual->OutputPblm();
    return 0;
}