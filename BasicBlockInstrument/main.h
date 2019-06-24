#include "llvm/Pass.h"
#include "llvm/IR/BasicBlock.h"


class BasicBlockStats: public BasicBlockPass{
public:
    static int count;
    static char ID;

        BasicBlockStats() : BasicBlockPass(ID) {};
        bool runOnBasicBlock(llvm::BasicBlock &B) override;
        bool doFinalization(llvm::BasicBlock &B) override;
};

// ----- register pass -----
char BasicBlockStats::ID = 0;
int BasicBlockStats::count = 0;
static RegisterPass<BasicBlockStats> X("bbstats", "Show statistics about basic blocks",
                                          false /* Only looks at CFG */,
                                          false /* Analysis Pass */);
