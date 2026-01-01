# steelc/CMakeLists.txt

project(steelc LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# LLVM
find_package(LLVM REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

# Add LLVM include directories and definitions
include_directories(${LLVM_INCLUDE_DIRS})
separate_arguments(LLVM_DEFINITIONS_LIST NATIVE_COMMAND ${LLVM_DEFINITIONS})
add_definitions(${LLVM_DEFINITIONS_LIST})

llvm_map_components_to_libnames(LLVM_LIBS
    core
    support
    irreader
    linker
    bitwriter
    passes
    analysis
    transformutils
    scalaropts
    ipo
    mc
    mcparser
    mcdisassembler
    targetparser
    codegen
    selectiondag
    target
    asmprinter
    native
    nativecodegen
)

# steelc core library
add_subdirectory("src")

# steelc executable
add_executable(steelc
	"src/main.cpp"
)

target_link_libraries(steelc PRIVATE
	steelc_core
)

# testing
option(ENABLE_TESTS "Build steelc tests" ON)
if (ENABLE_TESTS)
	add_subdirectory("tests")
endif()
