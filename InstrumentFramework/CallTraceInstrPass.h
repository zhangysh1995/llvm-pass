//
// Created by zhangysh1995 on 1/25/19.
//

#ifndef PROJECT_CALLTRACEINSTRPASS_H
#define PROJECT_CALLTRACEINSTRPASS_H

#include "AbsInstrPass.h"
#include "llvm/IR/IRBuilder.h"

class CallTraceInstrPass: public AbsInstrPass {
public:
    static char ID;
    CallTraceInstrPass(): AbsInstrPass(ID), csNum(0), fpNum(0), funcCG(0) {};

    bool runOnModule(Module& M) override;
    void getAnalysisUsage(AnalysisUsage& AU) const override;

    bool doFinalization(Module& M) override;


    // variadic function
    void doInstrument(Module &M, IRBuilder<>* builder,
            StringRef name,...);

private:
    LLVMContext* context;
    int funcCG;
    int csNum;
    int fpNum;
};

#endif //PROJECT_CALLTRACEINSTRPASS_H
