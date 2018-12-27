//
// Created by ZHANG Yushan on 2018/12/26.
// Description: instrument given functions, compare random number
// and decide whether stop or not
//

#ifndef LLVM_LEARN_MAIN_H
#define LLVM_LEARN_MAIN_H

#define DEBUG(X) errs() << X << "\n";

#include "llvm/Pass.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/Analysis/CallGraph.h"

#include <vector>
#include <string>


using namespace llvm;

class InstrumentFunction : public ModulePass {
public:
    static char ID;

    InstrumentFunction() : ModulePass(ID), v(nullptr),
        cg(nullptr), module(nullptr), functions(nullptr),
        num(0), f(nullptr), context(nullptr){ }
    ~InstrumentFunction() final;
    bool doInitialization(Module &M) override;
    bool runOnModule(Module &M) override;
    void getAnalysisUsage(AnalysisUsage & AU) const override;

    bool doFinalization(Module &) override;

private:
    Value *v;
    CallGraph *cg;
    Module* module;
    std::vector<std::string> *functions;
    Function *f;
    LLVMContext* context;

    int num;
    int genRandomNum();
    void readFunctions();
    void addGlobalValue(int i, std::string name);
    Value *getTargetInst(IRBuilder<> *builder);
};

// ----- register pass -----
char InstrumentFunction::ID = 0;
static RegisterPass<InstrumentFunction> X("instrument", "Instrument Pass",
                                          false /* Only looks at CFG */,
                                          false /* Analysis Pass */);


#endif //LLVM_LEARN_MAIN_H
