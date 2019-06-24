#include "llvm/Support/raw_ostream.h"

#include "main.h"

using namespace llvm;

bool BasicBlockStats::runOnBasicBlock(BasicBlock &B){
    count += 1;
}

bool BasicBlockStats::doFinalization(BasicBlock &B){
    errs() << "============ " << " Basic Block Statistics" << " ============";
    errs() << "We have " << BasicBlockStats::count << " basic blocks";
}
