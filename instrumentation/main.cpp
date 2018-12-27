#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Pass.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/InstIterator.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/TypeBuilder.h"
#include "llvm/Analysis/CallGraph.h"
#include "llvm/Bitcode/ReaderWriter.h"

#include "main.h"

#include <system_error>
#include <vector>
#include <ctime>
#include <string>

using std::vector;
using std::string;
using std::error_code;
using std::exit;

// ----- transformation -----
bool InstrumentFunction::runOnModule(Module &M) {
    /*
     * Assumption: only works on single module program (one .bc file)
     */

    v = nullptr;
    cg = &getAnalysis<CallGraphWrapperPass>().getCallGraph();
    module = &M;
    readFunctions();

    Type *i64_type = IntegerType::getInt64Ty(getGlobalContext());

    // add global value
//    num = genRandomNum(); // optional
//    addGlobalValue(num); // pass a value want to compare

    addGlobalValue(2, "l");
    addGlobalValue(7, "r");

    GlobalVariable *l = module->getGlobalVariable("l");
    GlobalVariable *r = module->getGlobalVariable("r");

    Value *vl = (ConstantInt*) l->getInitializer();
    Value *vr = (ConstantInt*) r->getInitializer();


    // iterate functions
    for (auto& cgn: *cg) {
        auto *func = const_cast<Function *>(cgn.first);
        // funcations cannot be null
        if (func == nullptr) continue;
        // we do not want to change llvm functions
        if (func->isIntrinsic()) continue;

        // is this function required?
        auto result = std::find(functions->begin(), functions->end(), func->getName().str());
        if (result == functions->end()) continue;

        // if required, instrument this function
        auto i = inst_begin(func);
        IRBuilder<> Builder(i->clone());
        DEBUG("Constructed IRBuilder")

        // fixme
        CallInst* v = Builder.CreateCall2(getTargetInst(&Builder), vl, vr); // insert checks
        Function *f = &cast<Function>(*v);
//        DEBUG("Inserted new function")

    }

    // output new module
    std::error_code EC;
    raw_fd_ostream OS("module.bc", EC, sys::fs::F_None);
    WriteBitcodeToFile(&M, OS);

    return true;
}

void cmpr(int l, int r, int i) {
    if (i < l || i > r)
        std::exit(-1);
}

// ----- instrument required checks
Value * InstrumentFunction::getTargetInst(IRBuilder<> *builder) {

    // ----- construct function call -----

    Type *i64_type = IntegerType::getInt64Ty(getGlobalContext());

    // singleton
    if (v == nullptr) {

        /*
         * customized values
         */
        addGlobalValue(5, "t");

        GlobalVariable *t = module->getGlobalVariable("t");

        Value *vt = (ConstantInt*) t->getInitializer();

        // insert a new function to program
        Value *v = module->getOrInsertFunction("cmpr", i64_type, i64_type, NULL);

        f = &cast<Function>(*v);

        // todo: get integer from value symbol table

        // define arguments
        Function::arg_iterator args = f->arg_begin();
        Value* l = args++;
        l->setName("l"); // lower  bound

        Value* r = args++;
        r->setName("r"); // upper bound

        BasicBlock *pb = f->begin();
        IRBuilder<> Builder(pb);

        // call "exit(0)"
        BasicBlock *blk =  BasicBlock::Create(getGlobalContext(), "stop", f);
        IRBuilder<> blkBuilder(blk);
        Value *one = ConstantInt::get(Type::getInt32Ty(module->getContext()),0);
        Value *blkv = module->getOrInsertFunction("exit", i64_type, NULL);
        blkBuilder.CreateCall(blkv, one);

        // empty
        BasicBlock *nblk =  BasicBlock::Create(getGlobalContext(), "nothing", f);

        // l - i
        Value *lower = Builder.CreateSub(l, t);
        // i - r
        Value *upper = Builder.CreateSub(t, r);
        // l - i || i - r (l > i or i > r)
        Value *both = Builder.CreateOr(lower, upper);
        // if (l > i || i > r), exit(0)
        BranchInst *br = Builder.CreateCondBr(both, blk, nblk);
    }

    // f should be called
    return f;
}

void InstrumentFunction::addGlobalValue(int i, string name) {
    // ----- pass generated number to program -----

    Type *i64_type = IntegerType::getInt64Ty(getGlobalContext());

    // create constant int
    GlobalVariable* gvar_int = new GlobalVariable(
            /*Type=*/i64_type,
            /*isConstant=*/true,
            /*Linkage=*/GlobalValue::CommonLinkage,
            /*Initializer=*/0, // has initializer, specified below
            /*Name=*/name);

    // Constant Definitions
    gvar_int->setAlignment(4);
    Constant* const_ = ConstantInt::get(i64_type, i/*value*/, true);
    // Global Variable Definitions
    gvar_int->setInitializer(const_);


    // todo: should work, need testing
}

int InstrumentFunction::genRandomNum() {
    // use current time for random seed
    srand(std::time(0));
    return rand()%100 + 1;
}

void InstrumentFunction::getAnalysisUsage(AnalysisUsage &AU) const {
    AU.addRequired<CallGraphWrapperPass>();
    AU.addPreserved<CallGraphWrapperPass>();
}

// ----- import required functions -----
void InstrumentFunction::readFunctions() {
    functions = new vector<string>();
    functions->push_back(string("main"));
    functions->push_back(string("test"));
}

bool InstrumentFunction::doInitialization(Module &M) {
    return true;
}

bool InstrumentFunction::doFinalization(Module &) {
    return true;
}

InstrumentFunction::~InstrumentFunction() {
    functions = nullptr;
    free(this->functions);
}
