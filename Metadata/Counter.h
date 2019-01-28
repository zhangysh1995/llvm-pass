//
// Created by zhangysh1995 on 1/21/19.
//

#ifndef PROJECT_COUNTER_H
#define PROJECT_COUNTER_H

#include "llvm/IR/Function.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/PassManager.h"

using namespace llvm;

class Counter: public FunctionPass{
public:
    static char ID;
    static int num ;

    Counter(): FunctionPass(ID) {}
    bool runOnFunction(Function &F) override;
    bool doFinalization(Module &M) override;
    void getAnalysisUsage(AnalysisUsage& AU) const override;

};

#endif //PROJECT_COUNTER_H
