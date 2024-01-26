#define CPP_INTERP_IMPORT
#include "../lib/Interpreter/Export.h"
#include <string.h>
#include <stdio.h>

void __CRTDECL operator delete(void* const block, size_t const) noexcept;
//const type_info tinfof;

int main() {
  EnableDebugOutput(true);
  const char* args[] = {
    "-v",
    "-target x86_64-pc-windows-msvc",
    "-fms-runtime-lib=dll",
    "-fms-extensions",
    "-Wunused-command-line-argument",
    "-fno-rtti",
     "-###",
  };
  CreateInterpreterC(args,2, "", 0);

  Process(R"==(#include <vcruntime_typeinfo.h>)==");
  InsertOrReplaceJitSymbol("??_7type_info@@6B@", 1);
  Process(R"==(
#include <vcruntime_typeinfo.h>
#include <vector>
//std::vector<int> testvec;
typedef std::vector<void*> VoidVec;

class testclass{
  int testmethod(){ return 42; }
};
void vector_reserve(VoidVec& v, int size)
{
  v.reserve(size);
}

void __CRTDECL operator delete(void* const block, size_t const) noexcept
{
    operator delete(block);
}
    )==");
  auto newstate = SearchLibrariesForSymbol("luaL_newstate", true);

  auto testvec = GetType("VoidVec");
  auto testclass = GetNamed("testclass", NULL);
  auto testtype = GetCanonicalType(testvec);// GetVariableType(testvec);
  auto stdns = GetScope("std", NULL);
  auto vector = GetScope("vector", stdns);
  char* vectypestr = GetTypeAsString(testtype);
  auto vecsize = GetSizeOfType(testvec);

  TemplateArgInfo tempargs[8] = { 0 };
  int targcount;// = GetClassTemplateInstantiationArgs(testtype, tempargs, 8);
  // targcount = GetClassTemplateInstantiationArgs(testvec, tempargs, 8);
  tempargs[0].m_Type = GetType("uint64_t");
  auto intvec = InstantiateClassTemplate(vector, tempargs, 1);
  char vec[32] = { 0 };
  Construct(intvec, vec);
  void* resultlist[32] = { 0 };
  VoidVec_Init(vec);


  vecsize = GetSizeOfType(testvec);
  int methodcount = GetClassMethods(intvec, (VoidVector*)&vec);
  for (int i = 0; i < methodcount; i++)
  {
    void* method = VoidVec_Get(vec, i);
    const char * sig = GetFunctionSignature(method);
    //printf("%s\n", sig);
  }
  auto testmethod = GetNamed("testmethod", testclass);
  auto reserver = GetNamed("reserve", intvec);
  int overloads = GetFunctionsUsingName(intvec, "reserve", (VoidVector*)&resultlist);
  void* func = VoidVec_Get(resultlist, 0);
  //char* sig = GetFunctionSignature(func);
  JitCall jcall;

  auto addr = MakeFunctionCallable(func, &jcall);

  auto faddr = GetFunctionAddress(func);
  addr = MakeFunctionCallable(func, &jcall);
  return 0;
}