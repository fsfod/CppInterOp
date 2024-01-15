#define CPP_INTERP_IMPORT
#include <string.h>
#include "../lib/Interpreter/Export.h"
#include <stdio.h>

int main() {
  EnableDebugOutput(true);
  const char* args[] = {
    "-target x86_64-pc-win32-msvc",
  };
  CreateInterpreterC(args, 1, "", 0);

  Declare(R"==(
#include <vector>
typedef std::vector<void*> VoidVec;
std::vector<int> testvec;

class testclass{
  int testmethod(){ return 42; }
};
    )==", false);
  auto testvec = GetNamed("testvec", NULL);
  auto testclass = GetNamed("testclass", NULL);
  auto testtype = GetVariableType(testvec);
  auto stdns = GetScope("std", NULL);
  auto vector = GetScope("vector", stdns);
  auto vecsize = GetSizeOfType(testtype);
  char* vectypestr = GetTypeAsString(testtype);
  char vec[32] = { 0 };
  Construct(vector, vec);
  void* resultlist[32] = { 0 };
  VoidVec_Init(vec);

  TemplateArgInfo tempargs[8] = { 0 };
  int targcount;// = GetClassTemplateInstantiationArgs(testtype, tempargs, 8);
 // targcount = GetClassTemplateInstantiationArgs(testvec, tempargs, 8);
  tempargs[0].m_Type = GetType("uint64_t");
  auto intvec = InstantiateClassTemplate(vector, tempargs, 1);
  int methodcount = GetClassMethods(intvec, (VoidVector*)&vec);
  for (int i = 0; i < methodcount; i++)
  {
    void* method = VoidVec_Get(vec, i);
    const char * sig = GetFunctionSignature(method);
    printf("%s\n", sig);
  }


  auto testmethod = GetNamed("testmethod", testclass);
  auto reserver = GetNamed("reserve", intvec);
  int overloads = GetFunctionsUsingName(intvec, "reserve", (VoidVector*)&resultlist);
  void* func = VoidVec_Get(resultlist, 0);
  char* sig = GetFunctionSignature(func);
  return 0;
}