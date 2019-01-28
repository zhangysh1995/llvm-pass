//
// Created by zhangysh1995 on 1/25/19.
//

#ifndef PROJECT_ABSINSTR_H
#define PROJECT_ABSINSTR_H

#include <llvm/IR/IRBuilder.h>
#include "llvm/IR/Module.h"

using namespace llvm;


// ===== Declarations =====
class AbsInstrPass: public ModulePass {

public:
    explicit AbsInstrPass(char ID): ModulePass(ID) {};

    bool runOnModule(Module &M) override;

protected:
    bool verify(Module &M);
    bool writeModule(Module &M, StringRef filename);
};


#endif //PROJECT_ABSINSTR_H
