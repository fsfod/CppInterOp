add_executable(DLLTest main.cpp)
set(CMAKE_CXX_STANDARD 20)

target_compile_options(DLLTest PRIVATE "/ZI")
set_target_properties(DLLTest PROPERTIES
    LINK_FLAGS "/INCREMENTAL"
)


target_link_libraries(DLLTest PRIVATE "clangCppInterOp")