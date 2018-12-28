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
#include "llvm/IR/Verifier.h"

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

    raw_ostream &os();
    cg = &getAnalysis<CallGraphWrapperPass>().getCallGraph();
    module = &M;
    readFunctions();

    // add global value
//    num = genRandomNum(); // optional, random number generator
//    addGlobalValue(num); // pass a global value

    addGlobalValue(2, "l");
    addGlobalValue(7, "r");

    GlobalVariable *l = module->getGlobalVariable("l");
    GlobalVariable *r = module->getGlobalVariable("r");

    Value *vl = (ConstantInt*) l->getInitializer();
    Value *vr = (ConstantInt*) r->getInitializer();

    verifyModule(*module, &outs());
    DEBUG("initialized global variable")

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

        // test
        assert(func->getEntryBlock().getTerminator() != nullptr);

        // if required, instrument this function
        auto& i = *inst_begin(func);
        IRBuilder<> Builder(&i);
        DEBUG("Constructed IRBuilder")

        //  before statements in this function
        Builder.SetInsertPoint(&i);
        CallInst* v = Builder.CreateCall2(getTargetInst(&Builder), vl, vr); // insert checks

        assert(func->getEntryBlock().getTerminator() != nullptr);
        DEBUG("Inserted new function")
    }

    DEBUG("End instrumentation")

    // output new module
    std::error_code EC;
    raw_fd_ostream OS("module.bc", EC, sys::fs::F_None);
    WriteBitcodeToFile(&M, OS);

    verifyModule(*module, &outs());
    return true;
}

int cmpr(int l, int r, int i) {
    if (i < l || i > r) {
        std::exit(0);
        return i;
    }
    else
        return i;
}

// ----- instrument required checks
Value * InstrumentFunction::getTargetInst(IRBuilder<> *builder) {

    // ----- construct function call -----

    // singleton
    if (v == nullptr) {
        /*
         * customized values
         */
        // todo: get integer from value symbol table
        addGlobalValue(5, "t");
        GlobalVariable *t = module->getGlobalVariable("t");
        Value *vt = (ConstantInt*) t->getInitializer();

        // insert a new function to program
        Constant *c = module->getOrInsertFunction("cmpr",
                FunctionType::getVoidTy(*context),
                Type::getInt64Ty(*context),
                Type::getInt64Ty(*context), NULL);

        f = &cast<Function>(*c);

        // define arguments
        Function::arg_iterator args = f->arg_begin();
        Value* l = args++;
        l->setName("l"); // lower  bound
        Value* r = args++;
        r->setName("r"); // upper bound
        DEBUG("Inserted function prototype")

        BasicBlock *pb =  BasicBlock::Create(*context, "entry", f);
        BasicBlock *blk =  BasicBlock::Create(*context, "cond_true", f);
        BasicBlock *nblk =  BasicBlock::Create(*context, "cond_false", f);

        IRBuilder<> Builder(pb);
        DEBUG("Created new empty basicblock")

        // i < l
        Value *lower = Builder.CreateICmpSLT(vt, l);
        // i > r
        Value *upper = Builder.CreateICmpSGT(vt, r);
        // l - i || i - r (l > i or i > r)
        Value *both = Builder.CreateOr(lower, upper);
        // if (l > i || i > r), exit(0)
        BranchInst *br = Builder.CreateCondBr(both, blk, nblk);

        DEBUG("Created compare condition")

        // cond_true: call exit(0)
        IRBuilder<> blkBuilder(blk);

        // could have runtime problem
        Value *one = ConstantInt::get(Type::getInt32Ty(*context), 0, true);
        Value *blkv = module->getOrInsertFunction("exit",
                Type::getVoidTy(getGlobalContext()),
                Type::getInt32Ty(getGlobalContext()), NULL);

        blkBuilder.CreateCall(blkv, one);
        DEBUG("Created call to exit 0")

        // every basic block must have one terminator
        blkBuilder.CreateRetVoid();

        IRBuilder<> nblkBuilder(nblk);
        nblkBuilder.CreateRetVoid(); // cond_false: do nothing and return
    }

    // f should be called
    return f;
}

void InstrumentFunction::addGlobalValue(int i, string name) {
    // ----- pass generated number to program -----

    Type *i64_type = IntegerType::getInt64Ty(*context);
    Constant* const_ = ConstantInt::get(*context, APInt(64, 0, true));

    // create constant int
    GlobalVariable* gvar_int = new GlobalVariable(
            /*Type=*/i64_type,
            /*isConstant=*/false,
            /*Linkage=*/GlobalValue::CommonLinkage,
            /*Initializer=*/0, // has initializer, specified below
            /*Name=*/name);

    // Constant Definitions
    gvar_int->setAlignment(4);

    // Global Variable Definitions
    gvar_int->setInitializer(const_);

    auto list = &module->getGlobalList();
    list->push_back(gvar_int);

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
    module = &M;
    context = &module->getContext();
    return true;
}

bool InstrumentFunction::doFinalization(Module &M) {
    return true;
}

InstrumentFunction::~InstrumentFunction() {
    functions = nullptr;
    free(this->functions);
}
