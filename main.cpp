#include <iostream>
//#include "codegen.h"
#include "node.h"
//#include "llvm/Target/Targetmachine.h"
//#include "llvm-3.4/lib/Target/X86/MCTargetDesc/X86MCTargetDesc.h"
//#include "llvm-3.4/lib/Target/X86/InstPrinter/X86ATTInstPrinter.h"
//#include "llvm-3.4/lib/Target/X86/InstPrinter/X86IntelInstPrinter.h"
//#include "X86MCAsmInfo.h"
//#include "llvm/ADT/Triple.h"
//#include "llvm/MC/MCCodeGenInfo.h"
//#include "llvm/MC/MCInstrAnalysis.h"
//#include "llvm/MC/MCInstrInfo.h"
//#include "llvm/MC/MCRegisterInfo.h"
//#include "llvm/MC/MCStreamer.h"
//#include "llvm/MC/MCSubtargetInfo.h"
//#include "llvm/MC/MachineLocation.h"
//#include "llvm/Support/ErrorHandling.h"
//#include "llvm/Support/Host.h"
//#include "llvm/Support/TargetRegistry.h"
//#include "X86GenRegisterInfo.inc"
//#include "X86GenInstrInfo.inc"
//#include "X86GenSubtargetInfo.inc"


using namespace std;

extern int yyparse();
extern NBlock* programBlock;

//void createCoreFunctions(CodeGenContext& context);

int main(int argc, char **argv)
{
	yyparse();
	std::cout << programBlock << endl;
    // see http://comments.gmane.org/gmane.comp.compilers.llvm.devel/33877
    //InitializeNativeTarget();
	//CodeGenContext context;
	//createCoreFunctions(context);
	//context.generateCode(*programBlock);
	//context.runCode();
	
	return 0;
}

