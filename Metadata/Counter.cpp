//
// Created by zhangysh1995 on 1/21/19.
//

#include "Counter.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/InstIterator.h"

#include "llvm/IR/CFG.h"


// ===== initialize pass =====
char Counter::ID = 0;
int Counter::num = 0;
static RegisterPass<Counter>X("counter", "count call site", false, false);

// ===== pass =====
bool Counter::runOnFunction(Function &F) {

    for (inst_iterator i = inst_begin(F); i != inst_end(F); i++) {
        if (i->getOpcode() == Instruction::Call || i->getOpcode() == Instruction::Invoke) {
            num ++;
        }
    }


    return false;
}

bool Counter::doFinalization(Module &M) {
   outs() << "We have " << num << " call sites \n";
}

bool Counter::getAnalysisUsage(AnalysisUsage &AU) const {
//    AU.addRequired();
}
