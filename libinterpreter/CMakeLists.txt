
set(
    sources
    interpreter.h
    VM.cpp
    VM.h
    VMCalls.cpp
    VMConfig.h
    VMOpt.cpp
)

set(
    dependencies
    evmc::evmc
    evmc::instructions
)

add_library(interpreter STATIC ${sources})
target_link_libraries(interpreter PRIVATE ${dependencies})

if(EVM_OPTIMIZE)
    target_compile_definitions(interpreter PRIVATE EVM_OPTIMIZE)
endif()

