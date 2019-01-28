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
    CallTraceInstrPass(): AbsInstrPass(ID) {};

    bool runOnModule(Module& M) override;
    void getAnalysisUsage(AnalysisUsage& AU) const override;

    // variadic function
    void doInstrument(Module &M, IRBuilder<>* builder,
            StringRef name,...);

private:
    LLVMContext* context;

};

#endif //PROJECT_CALLTRACEINSTRPASS_H
