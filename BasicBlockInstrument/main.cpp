#include "llvm/Support/raw_ostream.h"

#include "main.h"


using std::cout;
bool runOnBasicBlock(BasicBloc &B) override {
    count += 1;
}

bool doFinalization(Module &) override {
    errs() << "============ " << " Basic Block Statistics" << " ============";
    errs() << "We have " << count << " basic blocks";
}