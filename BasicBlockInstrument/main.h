#define DEBUG(X) errs() << X << "\n";

#include "llvm/Pass.h"
#include "llvm/IR/BasicBlock.h"

class BasicBlockStats : public BasicBlockPass {

    static int count = 0;
    static char ID;

    public:
        BasicBlockStats() : BasicBlockPass(ID) {};
        bool runOnBasicBlock(BasicBloc &B) override;
        bool doFinalization(Module &) override;
}

// ----- register pass -----
char InstrumentFunction::ID = 0;
static RegisterPass<BasicBlockStats> X("bbstats", "Show statistics about basic blocks",
                                          true /* Only looks at CFG */,
                                          false /* Analysis Pass */);