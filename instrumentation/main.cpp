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


// ----- transformation -----
bool InstrumentFunction::runOnModule(Module &M) {
    /*
     * Assumption: only works on single module program (one .bc file)
     */

    v = nullptr;
    cg = &getAnalysis<CallGraphWrapperPass>().getCallGraph();
    module = &M;
    readFunctions();

    // add global value
    num = genRandomNum();
    addGlobalValue(num);

    // iterate functions
    for (auto& cgn: *cg) {
        auto *func = const_cast<Function *>(cgn.first);
        if (func == nullptr) continue;
        if (func->isIntrinsic()) continue;

        auto result = std::find(functions->begin(), functions->end(), func->getName().str());
        if (result == functions->end()) continue;

        // instrument functions
        auto i = inst_begin(func);
        IRBuilder<> Builder(getGlobalContext());
        DEBUG("Constructed IRBuilder")

        Builder.CreateCall(getTargetInst());
        DEBUG("Inserted new function")

    }

    // output new module
//    std::error_code EC;
//    raw_fd_ostream OS("module.bc", EC, sys::fs::F_None);
//    WriteBitcodeToFile(&M, OS);

    return true;
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


void InstrumentFunction::readFunctions() {
    functions = new vector<string>();
    functions->push_back(string("main"));
    functions->push_back(string("test"));
}

bool InstrumentFunction::doInitialization(Module &M) {
    readFunctions();
    return true;
}

bool InstrumentFunction::doFinalization(Module &) {

    return true;
}

InstrumentFunction::~InstrumentFunction() {
    functions = nullptr;
    free(this->functions);
}

Value * InstrumentFunction::getTargetInst() {

    // ----- construct function call -----

    if (v == nullptr) {
        Value *v = module->getOrInsertFunction("cmpr", IntegerType::getInt32Ty(getGlobalContext()), NULL);

        f = &cast<Function>(*v);

        Function::arg_iterator args = f->arg_begin();
        Value* i = args++;
        i->setName("i");
    }

    return f;
}

void InstrumentFunction::addGlobalValue(int i) {
    // ----- pass generated number to program -----

    Type *i64_type = IntegerType::getInt64Ty(getGlobalContext());

    // create constant int
    GlobalVariable* gvar_ptr_abc = new GlobalVariable(
            /*Type=*/i64_type,
            /*isConstant=*/true,
            /*Linkage=*/GlobalValue::CommonLinkage,
            /*Initializer=*/0, // has initializer, specified below
            /*Name=*/"num");
    gvar_ptr_abc->setAlignment(4);
    Constant* const_ptr_2 = ConstantInt::get(i64_type, i/*value*/, true);
}



