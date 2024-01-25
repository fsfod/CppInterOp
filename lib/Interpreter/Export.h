#pragma once

typedef size_t TCppIndex_t;
typedef void *TCppScope_t, *TCppType_t, *TCppFunction_t, *TCppConstFunction_t, *TCppFuncAddr_t, *TInterp_t, *TCppObject_t;

#ifdef CPP_INTERP_IMPORT
#include <stdint.h>
#include <stddef.h>
#ifdef _MSC_VER
#define EXPORT_API extern "C" __declspec(dllexport)
#else
#define EXPORT_API extern "C"
#endif
typedef struct VoidVector;
typedef struct StringVector;
typedef struct JitCall {
  char data[24];
}JitCall;
struct TemplateArgInfo;

typedef struct TemplateArgInfo {
  TCppType_t m_Type;
  const char* m_IntegralValue;
} TemplateArgInfo;

#else

#ifdef _MSC_VER
#define EXPORT __declspec(dllexport)
#define EXPORT_API extern "C" __declspec(dllexport)
#else
#define EXPORT
#define EXPORT_API extern "C" 
#endif

#include "clang/Interpreter/CppInterOp.h"

typedef std::vector<std::string> StringVector;
typedef std::vector<void*> VoidVector;
typedef std::vector<Cpp::TemplateArgInfo> TemplateArgVector;
using Cpp::TemplateArgInfo;
using Cpp::JitCall;
#endif



#define DefVectorAPI(name) \
  EXPORT_API void name##_Init(void* v); \
  EXPORT_API void name##_Clear(void* v); \
  EXPORT_API void name##_Free(void* v); \

DefVectorAPI(VoidVec)
DefVectorAPI(StringVec)

EXPORT_API void* VoidVec_Get(void* v, int index);
EXPORT_API const char* StringVec_Get(void* v, int index);
EXPORT_API void FreeString(char *s);

EXPORT_API void DumpScope(TCppScope_t scope);
EXPORT_API const char* GetVersion();
EXPORT_API void EnableDebugOutput(bool value);
EXPORT_API bool IsDebugOutputEnabled();
EXPORT_API bool IsAggregate(TCppScope_t scope);
EXPORT_API bool IsNamespace(TCppScope_t scope);
EXPORT_API bool IsClass(TCppScope_t scope);
EXPORT_API bool IsComplete(TCppScope_t scope);
EXPORT_API size_t SizeOf(TCppScope_t scope);
EXPORT_API bool IsBuiltin(TCppType_t type);
EXPORT_API bool IsTemplate(TCppScope_t handle);
EXPORT_API bool IsTemplateSpecialization(TCppScope_t handle);
EXPORT_API bool IsTypedefed(TCppScope_t handle);
EXPORT_API bool IsAbstract(TCppType_t klass);
EXPORT_API bool IsEnumScope(TCppScope_t handle);
EXPORT_API bool IsEnumConstant(TCppScope_t handle);
EXPORT_API bool IsEnumType(TCppType_t type);
EXPORT_API int GetEnums(TCppScope_t scope, StringVector* Result);
EXPORT_API bool IsSmartPtrType(TCppType_t type);
EXPORT_API TCppType_t GetIntegerTypeFromEnumScope(TCppScope_t handle);
EXPORT_API TCppType_t GetIntegerTypeFromEnumType(TCppType_t handle);
EXPORT_API int GetEnumConstants(TCppScope_t scope, VoidVector* list);
EXPORT_API TCppType_t GetEnumConstantType(TCppScope_t scope);
EXPORT_API TCppIndex_t GetEnumConstantValue(TCppScope_t scope);
EXPORT_API size_t GetSizeOfType(TCppType_t type);
EXPORT_API bool IsVariable(TCppScope_t scope);


/// Gets the name of any named decl (a class,
/// namespace, variable, or a function).
EXPORT_API char* GetName(TCppScope_t klass);

/// This is similar to GetName() function, but besides
/// the name, it also gets the template arguments.
EXPORT_API char* GetCompleteName(TCppType_t klass);

/// Gets the "qualified" name (including the namespace) of any
/// named decl (a class, namespace, variable, or a function).
EXPORT_API char* GetQualifiedName(TCppType_t klass);

/// This is similar to GetQualifiedName() function, but besides
/// the "qualified" name (including the namespace), it also
/// gets the template arguments.
EXPORT_API char* GetQualifiedCompleteName(TCppType_t klass);

/// Gets the list of namespaces utilized in the supplied scope.
EXPORT_API int GetUsingNamespaces(TCppScope_t scope, VoidVector* list);

/// Gets the global scope of the whole C++  instance.
EXPORT_API void* GetGlobalScope();

/// Strips the typedef and returns the underlying class, and if the
/// underlying decl is not a class it returns the input unchanged.
EXPORT_API TCppScope_t GetUnderlyingScope(TCppScope_t scope);

/// Gets the namespace or class (by stripping typedefs) for the name 
/// passed as a parameter, and if the parent is not passed, 
/// then global scope will be assumed.
EXPORT_API TCppScope_t GetScope(const char* name, TCppScope_t parent);

/// When the namespace is known, then the parent doesn't need
/// to be specified. This will probably be phased-out in
/// future versions of the interop library.
EXPORT_API TCppScope_t GetScopeFromCompleteName(const char* name);

/// This function performs a lookup within the specified parent,
/// a specific named entity (functions, enums, etcetera).
EXPORT_API TCppScope_t GetNamed(const char* name, TCppScope_t parent);

/// Gets the parent of the scope that is passed as a parameter.
EXPORT_API TCppScope_t GetParentScope(TCppScope_t scope);

/// Gets the scope of the type that is passed as a parameter.
EXPORT_API TCppScope_t GetScopeFromType(TCppType_t type);

/// Gets the number of Base Classes for the Derived Class that
/// is passed as a parameter.
EXPORT_API TCppIndex_t GetNumBases(TCppType_t klass);

/// Gets a specific Base Class using its index. Typically GetNumBases()
/// is used to get the number of Base Classes, and then that number
/// can be used to iterate through the index value to get each specific
/// base class.
EXPORT_API TCppScope_t GetBaseClass(TCppType_t klass, TCppIndex_t ibase);

/// Checks if the supplied Derived Class is a sub-class of the
/// provided Base Class.
EXPORT_API bool IsSubclass(TCppScope_t derived, TCppScope_t base);

/// Each base has its own offset in a Derived Class. This offset can be
/// used to get to the Base Class fields.
EXPORT_API int64_t GetBaseClassOffset(TCppScope_t derived, TCppScope_t base);

/// Gets a list of all the Methods that are in the Class that is
/// supplied as a parameter.
EXPORT_API int GetClassMethods(TCppScope_t klass, VoidVector* list);

///\returns if a class has a default constructor.
EXPORT_API bool HasDefaultConstructor(TCppScope_t scope);

///\returns the default constructor of a class, if any.
EXPORT_API TCppFunction_t GetDefaultConstructor(TCppScope_t scope);

///\returns the class destructor, if any.
EXPORT_API TCppFunction_t GetDestructor(TCppScope_t scope);

/// Looks up all the functions that have the name that is
/// passed as a parameter in this function.
EXPORT_API int GetFunctionsUsingName(TCppScope_t scope, const char* name, VoidVector* list);

/// Gets the return type of the provided function.
EXPORT_API TCppType_t GetFunctionReturnType(TCppFunction_t func);

/// Gets the number of Arguments for the provided function.
EXPORT_API TCppIndex_t GetFunctionNumArgs(TCppFunction_t func);

/// Gets the number of Required Arguments for the provided function.
EXPORT_API TCppIndex_t GetFunctionRequiredArgs(TCppConstFunction_t func);

/// For each Argument of a function, you can get the Argument Type
/// by providing the Argument Index, based on the number of arguments
/// from the GetFunctionNumArgs() function.
EXPORT_API TCppType_t GetFunctionArgType(TCppFunction_t func, TCppIndex_t iarg);

///\returns a stringified version of a given function signature in the form:
/// void N::f(int i, double d, long l = 0, char ch = 'a').
EXPORT_API char* GetFunctionSignature(TCppFunction_t func);

///\returns if a function was marked as \c =delete.
EXPORT_API bool IsFunctionDeleted(TCppConstFunction_t function);

EXPORT_API bool IsTemplatedFunction(TCppFunction_t func);

/// This function performs a lookup to check if there is a
/// templated function of that type.
EXPORT_API bool ExistsFunctionTemplate(const char* name, TCppScope_t* parent);

/// Checks if the provided parameter is a method.
EXPORT_API bool IsMethod(TCppConstFunction_t method);

/// Checks if the provided parameter is a 'Public' method.
EXPORT_API bool IsPublicMethod(TCppFunction_t method);

/// Checks if the provided parameter is a 'Protected' method.
EXPORT_API bool IsProtectedMethod(TCppFunction_t method);

/// Checks if the provided parameter is a 'Private' method.
EXPORT_API bool IsPrivateMethod(TCppFunction_t method);

/// Checks if the provided parameter is a Constructor.
EXPORT_API bool IsConstructor(TCppConstFunction_t method);

/// Checks if the provided parameter is a Destructor.
EXPORT_API bool IsDestructor(TCppConstFunction_t method);

/// Checks if the provided parameter is a 'Static' method.
EXPORT_API bool IsStaticMethod(TCppConstFunction_t method);

///\returns the address of the function given its potentially mangled name.
EXPORT_API TCppFuncAddr_t GetFunctionAddressFromName(const char* mangled_name);

///\returns the address of the function given its function declaration.
EXPORT_API TCppFuncAddr_t GetFunctionAddress(TCppFunction_t method);

/// Checks if the provided parameter is a 'Virtual' method.
EXPORT_API bool IsVirtualMethod(TCppFunction_t method);

/// Gets all the Fields/Data Members of a Class. For now, it
/// only gets non-static data members but in a future update,
/// it may support getting static data members as well.
EXPORT_API int GetDatamembers(TCppScope_t scope, VoidVector* list);

/// This is a Lookup function to be used specifically for data members.
EXPORT_API void* LookupDatamember(const char* name, TCppScope_t parent);

/// Gets the type of the variable that is passed as a parameter.
EXPORT_API TCppType_t GetVariableType(TCppScope_t var);

/// Gets the address of the variable, you can use it to get the
/// value stored in the variable.
EXPORT_API intptr_t GetVariableOffset(TCppScope_t var);

/// Checks if the provided variable is a 'Public' variable.
EXPORT_API bool IsPublicVariable(TCppScope_t var);

/// Checks if the provided variable is a 'Protected' variable.
EXPORT_API bool IsProtectedVariable(TCppScope_t var);

/// Checks if the provided variable is a 'Private' variable.
EXPORT_API bool IsPrivateVariable(TCppScope_t var);

/// Checks if the provided variable is a 'Static' variable.
EXPORT_API bool IsStaticVariable(TCppScope_t var);

/// Checks if the provided variable is a 'Constant' variable.
EXPORT_API bool IsConstVariable(TCppScope_t var);

/// Checks if the provided parameter is a Record (struct).
EXPORT_API bool IsRecordType(TCppType_t type);

/// Checks if the provided parameter is a Plain Old Data Type (POD).
EXPORT_API bool IsPODType(TCppType_t type);

/// Gets the pure, Underlying Type (as opposed to the Using Type).
EXPORT_API TCppType_t GetUnderlyingType(TCppType_t type);

/// Gets the Type (passed as a parameter) as a String value.
EXPORT_API char* GetTypeAsString(TCppType_t type);

/// Gets the Canonical Type string from the std string. A canonical type
/// is the type with any typedef names, syntactic sugars or modifiers stripped
/// out of it.
EXPORT_API TCppType_t GetCanonicalType(TCppType_t type);

/// Used to either get the built-in type of the provided string, or
/// use the name to lookup the actual type.
EXPORT_API TCppType_t GetType(const char* type);

///\returns the complex of the provided type.
EXPORT_API TCppType_t GetComplexType(TCppType_t element_type);

/// This will convert a class into its type, so for example, you can
/// use it to declare variables in it.
EXPORT_API TCppType_t GetTypeFromScope(TCppScope_t klass);

/// Checks if a C++ type derives from another.
EXPORT_API bool IsTypeDerivedFrom(TCppType_t derived, TCppType_t base);

/// Creates a trampoline function by using the interpreter and returns a
/// uniform interface to call it from compiled code.
EXPORT_API int MakeFunctionCallable(TCppConstFunction_t func, JitCall* pcall);

/// Checks if a function declared is of const type or not.
EXPORT_API bool IsConstMethod(TCppFunction_t method);

///\returns the default argument value as string.
EXPORT_API char* GetFunctionArgDefault(TCppFunction_t func, TCppIndex_t param_index);

///\returns the argument name of function as string.
EXPORT_API char* GetFunctionArgName(TCppFunction_t func, TCppIndex_t param_index);

/// Creates an instance of the interpreter we need for the various interop
/// services.
///\param[in] Args - the list of arguments for interpreter constructor.
///\param[in] CPPINTEROP_EXTRA_INTERPRETER_ARGS - an env variable, if defined,
///           adds additional arguments to the interpreter.
EXPORT_API TInterp_t CreateInterpreterC(const char** Args, int ArgSize, const char* GpuArgs, int GpuArgsSize);

/// Checks which Interpreter backend was CppInterOp library built with (Cling,
/// Clang-REPL, etcetera). In practice, the selected interpreter should not
/// matter, since the library will function in the same way.
///\returns the current interpreter instance, if any.
EXPORT_API TInterp_t GetInterpreter();

/// Adds a Search Path for the Interpreter to get the libraries.
EXPORT_API void AddSearchPath(const char *dir, bool isUser, bool prepend);

/// Returns the resource-dir path (for headers).
EXPORT_API const char* GetResourceDir();

/// Secondary search path for headers, if not found using the
/// GetResourceDir() function.
EXPORT_API void AddIncludePath(const char *dir);

/// Only Declares a code snippet in \c code and does not execute it.
///\returns 0 on success
EXPORT_API int Declare(const char* code, bool silent);

/// Declares and executes a code snippet in \c code.
///\returns 0 on success
EXPORT_API int Process(const char *sourcecode);

/// Declares, executes and returns the execution result as a intptr_t.
///\returns the expression results as a intptr_t.
EXPORT_API intptr_t Evaluate(const char *sourcecode, bool *HadError);

/// Looks up the library if access is enabled.
///\returns the path to the library.
EXPORT_API char* LookupLibrary(const char *lib_name);

/// Finds \c lib_stem considering the list of search paths and loads it by
/// calling dlopen.
/// \returns true on success.
EXPORT_API bool LoadLibraryInPaths(const char* lib_stem, bool lookup = true);

/// Finds \c lib_stem considering the list of search paths and unloads it by
/// calling dlclose.
/// function.
EXPORT_API void UnloadLibrary(const char* lib_stem);

/// Scans all libraries on the library search path for a given potentially
/// mangled symbol name.
///\returns the path to the first library that contains the symbol definition.
EXPORT_API char* SearchLibrariesForSymbol(const char* mangled_name, bool search_system);

/// Inserts or replaces a symbol in the JIT with the one provided. This is
/// useful for providing our own implementations of facilities such as printf.
///
///\param[in] linker_mangled_name - the name of the symbol to be inserted or
///           replaced.
///\param[in] address - the new address of the symbol.
///
///\returns true on failure.
EXPORT_API bool InsertOrReplaceJitSymbol(const char* linker_mangled_name, uint64_t address);

/// Tries to load provided objects in a string format (prettyprint).
EXPORT_API char* ObjToString(const char *type, void *obj);

EXPORT_API void* InstantiateClassTemplate(TCppScope_t tmpl, TemplateArgInfo* template_args, size_t template_args_size);

/// Returns the class template instantiation arguments of \c templ_instance.
EXPORT_API int GetClassTemplateInstantiationArgs(TCppScope_t templ_instance, TemplateArgInfo* args, int count);


/// Instantiates a function template from a given string representation. This
/// function also does overload resolution.
///\returns the instantiated function template declaration.
EXPORT_API TCppFunction_t InstantiateTemplateFunctionFromString(const char* function_template);

EXPORT_API size_t GetAllCppNames(TCppScope_t scope, StringVector* list);

EXPORT_API void DumpScope(TCppScope_t scope);

/// Gets the size/dimensions of a multi-dimension array.
//EXPORT_API int GetDimensions(TCppType_t type, std::vector<long int>* list);

/// Allocates memory for a given class.
EXPORT_API TCppObject_t Allocate(TCppScope_t scope);

/// Deallocates memory for a given class.
EXPORT_API void Deallocate(TCppScope_t scope, TCppObject_t address);

/// Creates an object of class \c scope and calls its default constructor. If
/// \c arena is set it uses placement new.
EXPORT_API TCppObject_t Construct(TCppScope_t scope, void* arena);

/// Calls the destructor of object of type \c type. When withFree is true it
/// calls operator delete/free.
EXPORT_API void Destruct(TCppObject_t This, TCppScope_t type, bool withFree);

enum CaptureStreamKind : char {
  CaptureStream_StdOut = 1, ///< stdout
  CaptureStream_StdErr,     ///< stderr
  // kStdBoth,    ///< stdout and stderr
  // kSTDSTRM  // "&1" or "&2" is not a filename
};

/// Begins recording the given standard stream.
///\param[fd_kind] - The stream to be captured
EXPORT_API void BeginStdStreamCapture(CaptureStreamKind fd_kind);

/// Ends recording the standard stream and returns the result as a string.
EXPORT_API char* EndStdStreamCapture();

EXPORT_API const char* GetManagledName(TCppFunction_t method);

EXPORT_API bool LoadStaticLibary(const char* libpath);
