#include "clang/Interpreter/CppInterOp.h"
#include "Export.h"
#include <cstring>

static char* StdStringToCString(std::string&& s) {
  char dummy[sizeof(std::string)] = { 0 };
  char *cstr = s.data();
  char* start = ((char*)&s);

  // Steal the string data when not its not using small string optimization
  if ((cstr - start) > (intptr_t)sizeof(std::string) || (cstr - start) < 0) {
    *((std::string*)&dummy) = std::move(s);
  }
  else {
    cstr = s.get_allocator().allocate(s.length() + 1);
    memcpy(cstr, s.c_str(), s.length() + 1);
  }
  return cstr;
}

extern "C" {
#define VectorAPI(name, type) \
  EXPORT void name##_Init(void* v) { \
    new (reinterpret_cast<std::vector<type>*>(v)) std::vector<type>(); \
  } \
\
  EXPORT void name##_Clear(void* v) { \
    ((std::vector<type>*)v)->clear(); \
  } \
\
  EXPORT void name##_Free(void* v) { \
    ((std::vector<type>*)v)->~vector(); \
  } \

VectorAPI(VoidVec, void*);
VectorAPI(StringVec, std::string);
VectorAPI(TemplateArgVector, Cpp::TemplateArgInfo);

  EXPORT const char* StringVec_Get(void* v, int index) {
    return (*((std::vector<std::string>*)v))[index].c_str();
  }

  EXPORT void* VoidVec_Get(void* v, int index) {
    return (*((std::vector<void*>*)v))[index];
  }
///\returns the version string information of the library.
  EXPORT const char* GetVersion() {
    return StdStringToCString(Cpp::GetVersion());
  }

std::string::allocator_type cstralloctor;

  EXPORT void FreeString(char *s) {
    cstralloctor.deallocate(s, strlen(s));
  }
/// Enables or disables the debugging printouts on stderr.
/// Debugging output can be enabled also by the environment variable
/// CPPINTEROP_EXTRA_INTERPRETER_ARGS. For example,
/// CPPINTEROP_EXTRA_INTERPRETER_ARGS="-mllvm -debug-only=jitcall" to produce
/// only debug output for jitcall events.
EXPORT void EnableDebugOutput(bool value) {
  return Cpp::EnableDebugOutput(value);
}

///\returns true if the debugging printouts on stderr are enabled.
bool IsDebugOutputEnabled() {
  return Cpp::IsDebugOutputEnabled();
}

/// Checks if the given class represents an aggregate type).
///\returns true if \c scope is an array or a C++ tag (as per C++
///[dcl.init.aggr]) \returns true if the scope supports aggregate
/// initialization.
EXPORT bool IsAggregate(TCppScope_t scope) {
  return Cpp::IsAggregate(scope);
}

/// Checks if the scope is a namespace or not.
EXPORT bool IsNamespace(TCppScope_t scope) {
  return Cpp::IsNamespace(scope);
}

/// Checks if the scope is a class or not.
EXPORT bool IsClass(TCppScope_t scope) {
  return Cpp::IsClass(scope);
}

  // See TClingClassInfo::IsLoaded
/// Checks if the class definition is present, or not. Performs a
/// template instantiation if necessary.
EXPORT bool IsComplete(TCppScope_t scope) {
  return Cpp::IsComplete(scope);
}

EXPORT size_t SizeOf(TCppScope_t scope) {
  return Cpp::SizeOf(scope);
}

/// Checks if it is a "built-in" or a "complex" type.
EXPORT bool IsBuiltin(TCppType_t type) {
  return Cpp::IsBuiltin(type);
}

/// Checks if it is a templated class.
EXPORT bool IsTemplate(TCppScope_t handle) {
  return Cpp::IsTemplate(handle);
}

/// Checks if it is a class template specialization class.
EXPORT bool IsTemplateSpecialization(TCppScope_t handle) {
  return Cpp::IsTemplateSpecialization(handle);
}

/// Checks if \c handle introduces a typedef name via \c typedef or \c using.
EXPORT bool IsTypedefed(TCppScope_t handle) {
  return Cpp::IsTypedefed(handle);
}

EXPORT bool IsAbstract(TCppType_t klass) {
  return Cpp::IsAbstract(klass);
}

/// Checks if it is an enum name (EnumDecl represents an enum name).
EXPORT bool IsEnumScope(TCppScope_t handle) {
  return Cpp::IsEnumScope(handle);
}

/// Checks if it is an enum's value (EnumConstantDecl represents
/// each enum constant that is defined).
EXPORT bool IsEnumConstant(TCppScope_t handle) {
  return Cpp::IsEnumConstant(handle);
}

/// Checks if the passed value is an enum type or not.
EXPORT bool IsEnumType(TCppType_t type) {
  return Cpp::IsEnumType(type);
}

/// Extracts enum declarations from a specified scope and stores them in
/// vector
EXPORT int GetEnums(TCppScope_t scope, StringVector* Result) {

  new (Result)std::vector<std::string>();
  Cpp::GetEnums(scope, *Result);
  return Result->size();
}

/// We assume that smart pointer types define both operator* and
/// operator->.
EXPORT bool IsSmartPtrType(TCppType_t type) {
  return Cpp::IsSmartPtrType(type);
}

/// For the given "class", get the integer type that the enum
/// represents, so that you can store it properly in your specific
/// language.
EXPORT TCppType_t GetIntegerTypeFromEnumScope(TCppScope_t handle) {
  return Cpp::GetIntegerTypeFromEnumScope(handle);
}

/// For the given "type", this function gets the integer type that the enum
/// represents, so that you can store it properly in your specific
/// language.
EXPORT TCppType_t GetIntegerTypeFromEnumType(TCppType_t handle) {
  return Cpp::GetIntegerTypeFromEnumType(handle);
}

/// Gets a list of all the enum constants for an enum.
EXPORT int GetEnumConstants(TCppScope_t scope, VoidVector* list) {
  new (list)std::vector<TCppScope_t>();
  *list = Cpp::GetEnumConstants(scope);
  return list->size();
}

/// Gets the enum name when an enum constant is passed.
EXPORT TCppType_t GetEnumConstantType(TCppScope_t scope) {
  return Cpp::GetEnumConstantType(scope);
}

/// Gets the index value (0,1,2, etcetera) of the enum constant
/// that was passed into this function.
EXPORT TCppIndex_t GetEnumConstantValue(TCppScope_t scope) {
  return Cpp::GetEnumConstantValue(scope);
}

/// Gets the size of the "type" that is passed in to this function.
EXPORT size_t GetSizeOfType(TCppType_t type) {
  return Cpp::GetSizeOfType(type);
}

/// Checks if the passed value is a variable.
EXPORT bool IsVariable(TCppScope_t scope) {
  return Cpp::IsVariable(scope);
}

/// Gets the name of any named decl (a class,
/// namespace, variable, or a function).
EXPORT char* GetName(TCppScope_t klass) {
  return StdStringToCString(Cpp::GetName(klass));
}

/// This is similar to GetName() function, but besides
/// the name, it also gets the template arguments.
EXPORT char* GetCompleteName(TCppType_t klass) {
  return StdStringToCString(Cpp::GetCompleteName(klass));
}

/// Gets the "qualified" name (including the namespace) of any
/// named decl (a class, namespace, variable, or a function).
EXPORT char* GetQualifiedName(TCppType_t klass) {
  return StdStringToCString(Cpp::GetQualifiedName(klass));
}

/// This is similar to GetQualifiedName() function, but besides
/// the "qualified" name (including the namespace), it also
/// gets the template arguments.
EXPORT char* GetQualifiedCompleteName(TCppType_t klass) {
  return StdStringToCString(Cpp::GetQualifiedCompleteName(klass));
}

/// Gets the list of namespaces utilized in the supplied scope.
EXPORT int GetUsingNamespaces(TCppScope_t scope, VoidVector* list) {
  new (list)std::vector<TCppScope_t>();
  *list = Cpp::GetUsingNamespaces(scope);
  return list->size();
}

/// Gets the global scope of the whole C++  instance.
EXPORT void* GetGlobalScope() {
  return Cpp::GetGlobalScope();
}

/// Strips the typedef and returns the underlying class, and if the
/// underlying decl is not a class it returns the input unchanged.
EXPORT TCppScope_t GetUnderlyingScope(TCppScope_t scope) {
  return Cpp::GetUnderlyingScope(scope);
}

/// Gets the namespace or class (by stripping typedefs) for the name 
/// passed as a parameter, and if the parent is not passed, 
/// then global scope will be assumed.
EXPORT TCppScope_t GetScope(const char* name, TCppScope_t parent) {
  return Cpp::GetScope(name, parent);
}

/// When the namespace is known, then the parent doesn't need
/// to be specified. This will probably be phased-out in
/// future versions of the interop library.
EXPORT TCppScope_t GetScopeFromCompleteName(const char* name) {
  return Cpp::GetScopeFromCompleteName(name);
}

/// This function performs a lookup within the specified parent,
/// a specific named entity (functions, enums, etcetera).
EXPORT TCppScope_t GetNamed(const char* name, TCppScope_t parent)
{
  return Cpp::GetNamed(name, parent);
}

/// Gets the parent of the scope that is passed as a parameter.
EXPORT TCppScope_t GetParentScope(TCppScope_t scope) {
  return Cpp::GetParentScope(scope);
}

/// Gets the scope of the type that is passed as a parameter.
EXPORT TCppScope_t GetScopeFromType(TCppType_t type) {
  return Cpp::GetScopeFromType(type);
}

/// Gets the number of Base Classes for the Derived Class that
/// is passed as a parameter.
EXPORT TCppIndex_t GetNumBases(TCppType_t klass) {
  return Cpp::GetNumBases(klass);
}

/// Gets a specific Base Class using its index. Typically GetNumBases()
/// is used to get the number of Base Classes, and then that number
/// can be used to iterate through the index value to get each specific
/// base class.
EXPORT TCppScope_t GetBaseClass(TCppType_t klass, TCppIndex_t ibase) {
  return Cpp::GetBaseClass(klass, ibase);
}

/// Checks if the supplied Derived Class is a sub-class of the
/// provided Base Class.
EXPORT bool IsSubclass(TCppScope_t derived, TCppScope_t base) {
  return Cpp::IsSubclass(derived, base);
}

/// Each base has its own offset in a Derived Class. This offset can be
/// used to get to the Base Class fields.
EXPORT int64_t GetBaseClassOffset(TCppScope_t derived, TCppScope_t base) {
  return Cpp::GetBaseClassOffset(derived, base);
}

/// Gets a list of all the Methods that are in the Class that is
/// supplied as a parameter.
EXPORT int GetClassMethods(TCppScope_t klass, VoidVector* list) {
  new (list)std::vector<TCppFunction_t>();
  *list = Cpp::GetClassMethods(klass);
  return list->size();
}

///\returns if a class has a default constructor.
EXPORT bool HasDefaultConstructor(TCppScope_t scope) {
  return Cpp::HasDefaultConstructor(scope);
}

///\returns the default constructor of a class, if any.
EXPORT TCppFunction_t GetDefaultConstructor(TCppScope_t scope) {
  return Cpp::GetDefaultConstructor(scope);
}

///\returns the class destructor, if any.
EXPORT TCppFunction_t GetDestructor(TCppScope_t scope) {
  return Cpp::GetDestructor(scope);
}

/// Looks up all the functions that have the name that is
/// passed as a parameter in this function.
EXPORT int GetFunctionsUsingName(TCppScope_t scope, const char* name, VoidVector* list) {
  new (list)std::vector<TCppFunction_t>();
  *list = Cpp::GetFunctionsUsingName(scope, name);
  return list->size();
}

/// Gets the return type of the provided function.
EXPORT TCppType_t GetFunctionReturnType(TCppFunction_t func) {
  return Cpp::GetFunctionReturnType(func);
}

/// Gets the number of Arguments for the provided function.
EXPORT TCppIndex_t GetFunctionNumArgs(TCppFunction_t func) {
  return Cpp::GetFunctionNumArgs(func);
}

/// Gets the number of Required Arguments for the provided function.
EXPORT TCppIndex_t GetFunctionRequiredArgs(TCppConstFunction_t func) {
  return Cpp::GetFunctionRequiredArgs(func);
}

/// For each Argument of a function, you can get the Argument Type
/// by providing the Argument Index, based on the number of arguments
/// from the GetFunctionNumArgs() function.
EXPORT TCppType_t GetFunctionArgType(TCppFunction_t func, TCppIndex_t iarg) {
  return Cpp::GetFunctionArgType(func, iarg);
}

///\returns a stringified version of a given function signature in the form:
/// void N::f(int i, double d, long l = 0, char ch = 'a').
EXPORT char* GetFunctionSignature(TCppFunction_t func) {
  return StdStringToCString(Cpp::GetFunctionSignature(func));
}

///\returns if a function was marked as \c =delete.
EXPORT bool IsFunctionDeleted(TCppConstFunction_t function) {
  return Cpp::IsFunctionDeleted(function);
}

EXPORT bool IsTemplatedFunction(TCppFunction_t func) {
  return Cpp::IsTemplatedFunction(func);
}

/// This function performs a lookup to check if there is a
/// templated function of that type.
EXPORT bool ExistsFunctionTemplate(const char* name, TCppScope_t* parent) {
  return Cpp::ExistsFunctionTemplate(name, parent);
}

/// Checks if the provided parameter is a method.
EXPORT bool IsMethod(TCppConstFunction_t method) {
  return Cpp::IsMethod(method);
}

/// Checks if the provided parameter is a 'Public' method.
EXPORT bool IsPublicMethod(TCppFunction_t method) {
  return Cpp::IsPublicMethod(method);
}

/// Checks if the provided parameter is a 'Protected' method.
EXPORT bool IsProtectedMethod(TCppFunction_t method) {
  return Cpp::IsProtectedMethod(method);
}

/// Checks if the provided parameter is a 'Private' method.
EXPORT bool IsPrivateMethod(TCppFunction_t method) {
  return Cpp::IsPrivateMethod(method);
}

/// Checks if the provided parameter is a Constructor.
EXPORT bool IsConstructor(TCppConstFunction_t method) {
  return Cpp::IsConstructor(method);
}

/// Checks if the provided parameter is a Destructor.
EXPORT bool IsDestructor(TCppConstFunction_t method) {
  return Cpp::IsDestructor(method);
}

/// Checks if the provided parameter is a 'Static' method.
EXPORT bool IsStaticMethod(TCppConstFunction_t method) {
  return Cpp::IsStaticMethod(method);
}

///\returns the address of the function given its potentially mangled name.
EXPORT TCppFuncAddr_t GetFunctionAddressFromName(const char* mangled_name) {
  return Cpp::GetFunctionAddress(mangled_name);
}

///\returns the address of the function given its function declaration.
EXPORT TCppFuncAddr_t GetFunctionAddress(TCppFunction_t method) {
  return Cpp::GetFunctionAddress(method);
}

/// Checks if the provided parameter is a 'Virtual' method.
EXPORT bool IsVirtualMethod(TCppFunction_t method) {
  return Cpp::IsVirtualMethod(method);
}

/// Gets all the Fields/Data Members of a Class. For now, it
/// only gets non-static data members but in a future update,
/// it may support getting static data members as well.
EXPORT int GetDatamembers(TCppScope_t scope, std::vector<TCppScope_t>* list) {
  new (list)std::vector<TCppScope_t>();
  *list = Cpp::GetDatamembers(scope);
  return list->size();
}

/// This is a Lookup function to be used specifically for data members.
EXPORT void* LookupDatamember(const char* name, TCppScope_t parent) {
  return Cpp::LookupDatamember(name, parent);
}

/// Gets the type of the variable that is passed as a parameter.
EXPORT TCppType_t GetVariableType(TCppScope_t var) {
  return Cpp::GetVariableType(var);
}

/// Gets the address of the variable, you can use it to get the
/// value stored in the variable.
EXPORT intptr_t GetVariableOffset(TCppScope_t var) {
  return Cpp::GetVariableOffset(var);
}

/// Checks if the provided variable is a 'Public' variable.
EXPORT bool IsPublicVariable(TCppScope_t var) {
  return Cpp::IsPublicVariable(var);
}

/// Checks if the provided variable is a 'Protected' variable.
EXPORT bool IsProtectedVariable(TCppScope_t var) {
  return Cpp::IsProtectedVariable(var);
}

/// Checks if the provided variable is a 'Private' variable.
EXPORT bool IsPrivateVariable(TCppScope_t var) {
  return Cpp::IsPrivateVariable(var);
}

/// Checks if the provided variable is a 'Static' variable.
EXPORT bool IsStaticVariable(TCppScope_t var) {
  return Cpp::IsStaticVariable(var);
}

/// Checks if the provided variable is a 'Constant' variable.
EXPORT bool IsConstVariable(TCppScope_t var) {
  return Cpp::IsConstVariable(var);
}

/// Checks if the provided parameter is a Record (struct).
EXPORT bool IsRecordType(TCppType_t type) {
  return Cpp::IsRecordType(type);
}

/// Checks if the provided parameter is a Plain Old Data Type (POD).
EXPORT bool IsPODType(TCppType_t type) {
  return Cpp::IsPODType(type);
}

/// Gets the pure, Underlying Type (as opposed to the Using Type).
EXPORT TCppType_t GetUnderlyingType(TCppType_t type) {
  return Cpp::GetUnderlyingType(type);
}

/// Gets the Type (passed as a parameter) as a String value.
EXPORT char* GetTypeAsString(TCppType_t type) {
  return StdStringToCString(Cpp::GetTypeAsString(type));
}

/// Gets the Canonical Type string from the std string. A canonical type
/// is the type with any typedef names, syntactic sugars or modifiers stripped
/// out of it.
EXPORT TCppType_t GetCanonicalType(TCppType_t type) {
  return Cpp::GetCanonicalType(type);
}

/// Used to either get the built-in type of the provided string, or
/// use the name to lookup the actual type.
EXPORT TCppType_t GetType(const char* type) {
  return Cpp::GetType(type);
}

///\returns the complex of the provided type.
EXPORT TCppType_t GetComplexType(TCppType_t element_type) {
  return Cpp::GetComplexType(element_type);
}

/// This will convert a class into its type, so for example, you can
/// use it to declare variables in it.
EXPORT TCppType_t GetTypeFromScope(TCppScope_t klass) {
  return Cpp::GetTypeFromScope(klass);
}

/// Checks if a C++ type derives from another.
EXPORT bool IsTypeDerivedFrom(TCppType_t derived, TCppType_t base) {
  return Cpp::IsTypeDerivedFrom(derived, base);
}

/// Creates a trampoline function by using the interpreter and returns a
/// uniform interface to call it from compiled code.
EXPORT int MakeFunctionCallable(TCppConstFunction_t func, Cpp::JitCall* pcall) {
  auto call = Cpp::MakeFunctionCallable(func);
  memcpy(pcall, &call, sizeof(call));
  return call.getKind();
}

/// Checks if a function declared is of const type or not.
EXPORT bool IsConstMethod(TCppFunction_t method) {
  return Cpp::IsConstMethod(method);
}

///\returns the default argument value as string.
EXPORT char* GetFunctionArgDefault(TCppFunction_t func, TCppIndex_t param_index) {
  return StdStringToCString(Cpp::GetFunctionArgDefault(func, param_index));
}

///\returns the argument name of function as string.
EXPORT char* GetFunctionArgName(TCppFunction_t func, TCppIndex_t param_index) {
  return StdStringToCString(Cpp::GetFunctionArgName(func, param_index));
}

/// Creates an instance of the interpreter we need for the various interop
/// services.
///\param[in] Args - the list of arguments for interpreter constructor.
///\param[in] CPPINTEROP_EXTRA_INTERPRETER_ARGS - an env variable, if defined,
///           adds additional arguments to the interpreter.
EXPORT TInterp_t CreateInterpreterC(const char** Args, int ArgSize, const char* GpuArgs, int GpuArgsSize) {
  std::vector<const char*> args;

  for (int i = 0; i < ArgSize; i++) {
    args.push_back(Args[i]);
  }

  return Cpp::CreateInterpreter(args);
}

/// Checks which Interpreter backend was CppInterOp library built with (Cling,
/// Clang-REPL, etcetera). In practice, the selected interpreter should not
/// matter, since the library will function in the same way.
///\returns the current interpreter instance, if any.
EXPORT TInterp_t GetInterpreter() {
  return Cpp::GetInterpreter();
}

/// Adds a Search Path for the Interpreter to get the libraries.
EXPORT void AddSearchPath(const char *dir, bool isUser = true, bool prepend = false) {
  return Cpp::AddSearchPath(dir, isUser, prepend);
}

/// Returns the resource-dir path (for headers).
EXPORT const char* GetResourceDir() {
  return Cpp::GetResourceDir();
}

/// Secondary search path for headers, if not found using the
/// GetResourceDir() function.
EXPORT void AddIncludePath(const char *dir) {
  return Cpp::AddIncludePath(dir);
}

/// Only Declares a code snippet in \c code and does not execute it.
///\returns 0 on success
EXPORT int Declare(const char* code, bool silent) {
  return Cpp::Declare(code, silent);
}

/// Declares and executes a code snippet in \c code.
///\returns 0 on success
EXPORT int Process(const char *sourcecode) {
  return Cpp::Process(sourcecode);
}

/// Declares, executes and returns the execution result as a intptr_t.
///\returns the expression results as a intptr_t.
EXPORT intptr_t Evaluate(const char *sourcecode, bool *HadError) {
  return Cpp::Evaluate(sourcecode, HadError);
}

/// Looks up the library if access is enabled.
///\returns the path to the library.
EXPORT char* LookupLibrary(const char *lib_name) {
  std::string libpath = Cpp::LookupLibrary(lib_name);
  return StdStringToCString(std::move(libpath));
}

/// Finds \c lib_stem considering the list of search paths and loads it by
/// calling dlopen.
/// \returns true on success.
EXPORT bool LoadLibraryInPaths(const char* lib_stem, bool lookup) {
  return Cpp::LoadLibrary(lib_stem, lookup);
}

/// Finds \c lib_stem considering the list of search paths and unloads it by
/// calling dlclose.
/// function.
EXPORT void UnloadLibrary(const char* lib_stem) {
  return Cpp::UnloadLibrary(lib_stem);
}

/// Scans all libraries on the library search path for a given potentially
/// mangled symbol name.
///\returns the path to the first library that contains the symbol definition.
EXPORT char* SearchLibrariesForSymbol(const char* mangled_name, bool search_system) {
  return StdStringToCString(Cpp::SearchLibrariesForSymbol(mangled_name, search_system));
}

/// Inserts or replaces a symbol in the JIT with the one provided. This is
/// useful for providing our own implementations of facilities such as printf.
///
///\param[in] linker_mangled_name - the name of the symbol to be inserted or
///           replaced.
///\param[in] address - the new address of the symbol.
///
///\returns true on failure.
EXPORT bool InsertOrReplaceJitSymbol(const char* linker_mangled_name, uint64_t address) {
  return Cpp::InsertOrReplaceJitSymbol(linker_mangled_name, address);
}

/// Tries to load provided objects in a string format (prettyprint).
EXPORT char* ObjToString(const char *type, void *obj) {
  return StdStringToCString(Cpp::ObjToString(type, obj));
}

EXPORT void* InstantiateClassTemplate(TCppScope_t tmpl, TemplateArgInfo* template_args, size_t template_args_size) {
  return Cpp::InstantiateClassTemplate(tmpl, reinterpret_cast<Cpp::TemplateArgInfo*>(template_args), template_args_size);
}

/// Returns the class template instantiation arguments of \c templ_instance.
EXPORT int GetClassTemplateInstantiationArgs(TCppScope_t templ_instance, TemplateArgInfo* arglist, int count) {
  std::vector<Cpp::TemplateArgInfo> argvec;
  Cpp::GetClassTemplateInstantiationArgs(templ_instance, argvec);
  int ArgCount = (int)argvec.size();
  if (count > 0)
  {
    count = count > ArgCount ? ArgCount : count;
    memcpy(arglist, argvec.data(), sizeof(Cpp::TemplateArgInfo)*count);
  }
  return ArgCount;
}

/// Instantiates a function template from a given string representation. This
/// function also does overload resolution.
///\returns the instantiated function template declaration.
EXPORT TCppFunction_t InstantiateTemplateFunctionFromString(const char* function_template) {
  return Cpp::InstantiateTemplateFunctionFromString(function_template);
}

EXPORT size_t GetAllCppNames(TCppScope_t scope, StringVector* list) {
  new (list)std::vector<std::string>();
  *list = Cpp::GetAllCppNames(scope);
  return list->size();
}

EXPORT void DumpScope(TCppScope_t scope) {
  return Cpp::DumpScope(scope);
}

/// Gets the size/dimensions of a multi-dimension array.
EXPORT int GetDimensions(TCppType_t type, std::vector<long int>* list) {
  new (list)std::vector<long int>();
  *list = Cpp::GetDimensions(type);
  return list->size();
}

/// Allocates memory for a given class.
EXPORT TCppObject_t Allocate(TCppScope_t scope) {
  return Cpp::Allocate(scope);
}

/// Deallocates memory for a given class.
EXPORT void Deallocate(TCppScope_t scope, TCppObject_t address) {
  return Cpp::Deallocate(scope, address);
}

/// Creates an object of class \c scope and calls its default constructor. If
/// \c arena is set it uses placement new.
EXPORT TCppObject_t Construct(TCppScope_t scope, void* arena) {
  return Cpp::Construct(scope, arena);
}

/// Calls the destructor of object of type \c type. When withFree is true it
/// calls operator delete/free.
EXPORT void Destruct(TCppObject_t This, TCppScope_t type, bool withFree) {
  return Cpp::Destruct(This, type, withFree);
}

EXPORT void BeginStdStreamCapture(CaptureStreamKind fd_kind) {
  Cpp::BeginStdStreamCapture((Cpp::CaptureStreamKind)fd_kind);
}

/// Ends recording the standard stream and returns the result as a string.
EXPORT char* EndStdStreamCapture() {
  return StdStringToCString(Cpp::EndStdStreamCapture());
}

EXPORT const char* GetManagledName(TCppFunction_t method) {
  std::string name;
  return Cpp::GetManagledName(name) ? StdStringToCString(name) : "";
}

}