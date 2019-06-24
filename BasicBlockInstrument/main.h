#include "llvm/Pass.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Module.h"

using namespace llvm;

class BasicBlockStats: public BasicBlockPass{
public:
    static int count;
    static char ID;

        BasicBlockStats() : BasicBlockPass(ID) {};
        bool runOnBasicBlock(BasicBlock &B) override;
        bool doFinalization(Module &M) override;
};

// ----- register pass -----
char BasicBlockStats::ID = 0;
int BasicBlockStats::count = 0;

static RegisterPass<BasicBlockStats> X("bbstats", "Show statistics about basic blocks",
                                          false /* Only looks at CFG */,
                                          false /* Analysis Pass */);
