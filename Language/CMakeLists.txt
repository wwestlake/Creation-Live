add_library(creation_live_language_host STATIC
    AppLanguagePolicy.cpp
    AppLanguagePolicy.h
)

target_include_directories(creation_live_language_host PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")

if(CL_ENABLE_SCRIPTING)
    target_include_directories(creation_live_language_host PRIVATE "${CL_LLVM_ROOT}/include")
    target_compile_definitions(creation_live_language_host PUBLIC CL_ENABLE_SCRIPTING=1)
else()
    target_compile_definitions(creation_live_language_host PUBLIC CL_ENABLE_SCRIPTING=0)
endif()
