//
// Created by zhangysh1995 on 1/25/19.
//

#include <llvm/IR/IRBuilder.h>
#include "llvm/IR/LegacyPassManager.h"
#include <llvm/Transforms/IPO/PassManagerBuilder.h>
#include "llvm/Pass.h"
#include "llvm/Analysis/CallGraph.h"
#include "llvm/IR/InstIterator.h"
#include "llvm/IR/CallSite.h"
#include "CallTraceInstrPass.h"
#include <stdarg.h>
#include <string.h>

bool CallTraceInstrPass::runOnModule(Module& M) {
    context=&getGlobalContext();
    // only care about functions called
    CallGraph* cg = &getAnalysis<CallGraphWrapperPass>().getCallGraph();

    for(auto& node : *cg)
    {
        if(node.first == nullptr)
            continue;
        if(node.first->isIntrinsic())
            continue;

        auto func = const_cast<Function*>(node.first);
        for(inst_iterator i = inst_begin(func);
            i != inst_end(func); i++)
        {
            assert(i->getParent()->getTerminator() != nullptr);

            // check for call site
            if(i->getOpcode() == Instruction::Call ||
                    i->getOpcode() == Instruction::Invoke)
            {
                StringRef funcName;

                // get called function
                CallSite cs(cast<Value>(&*i));
                if(Function* calledFunc = cs.getCalledFunction())
                    funcName = calledFunc->getName();
                else
                    continue; // function pointer

                // insert logging
                IRBuilder<> builder(*context);
                builder.SetInsertPoint(&*i);

                doInstrument(M, &builder, funcName);
            }
        }
    }

    verify(M);
//    writeModule(M, "test.bc");
//    writeModule(M, name);
}

void CallTraceInstrPass::doInstrument(Module &M,
        IRBuilder<>* builder, StringRef funcName, ...) {

    // variadic args
    va_list ap;
    va_start(ap, funcName);


    // update name to print

    Value* result = M.getGlobalVariable("funcName");

    if(result == nullptr) {
        result = builder->CreateGlobalStringPtr(funcName, "funcName");
    }


    Value* blkv = M.getOrInsertFunction("printf",
            FunctionType::get(IntegerType::getInt32Ty(*context),
            PointerType::get(Type::getInt8Ty(*context), 0),true /* this is var arg func type*/));

    builder->CreateCall(blkv, result);

}

void CallTraceInstrPass::getAnalysisUsage(AnalysisUsage &AU) const {
    AU.addRequired<CallGraphWrapperPass>();
    AU.setPreservesAll();
}


// ===== Register Pass =====
char CallTraceInstrPass::ID = 0;
static RegisterPass<CallTraceInstrPass> X("calltrace", "instrument every call site", false, false);

static void registerCallTraceInstrPass(const PassManagerBuilder &,
                                 legacy::PassManagerBase &PM) {
    PM.add(new CallTraceInstrPass);
}
static RegisterStandardPasses
        RegisterMyPass(PassManagerBuilder::EP_EnabledOnOptLevel0,
                       registerCallTraceInstrPass);
