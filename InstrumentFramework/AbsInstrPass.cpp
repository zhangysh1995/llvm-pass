// ===== LLVM Headers =====
#include "llvm/Pass.h"
#include "llvm/IR/Function.h"

// ===== LLVM Utility Headers =====
#include "llvm/Support/raw_ostream.h"
#include <llvm/Support/FileSystem.h>
#include <llvm/IR/Verifier.h>
#include "llvm/Bitcode/ReaderWriter.h"

// ===== User Headers =====
#include "AbsInstrPass.h"


using namespace llvm;


// ===== Definitions =====
bool AbsInstrPass::runOnModule(Module &M) {
  return false;
}

bool AbsInstrPass::verify(Module &M) {
  return verifyModule(M);
}

bool AbsInstrPass::writeModule(Module &M, StringRef filename) {
  std::error_code EC;
  raw_fd_ostream OS(filename, EC, sys::fs::F_None);
  WriteBitcodeToFile(&M, OS);
  return false;
}

