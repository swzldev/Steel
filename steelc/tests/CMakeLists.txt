# steelc/tests/CMakeLists.txt

add_executable(steelc_tests
	"tests_main.cpp"

	"test.cpp"
	"test.h"
	"tests_main.cpp"
	"test_manager.cpp"
	"test_manager.h"

	# lexer_tests/
	"lexer_tests/lexer_tests.cpp"
	"lexer_tests/lexer_tests.h"
)

target_include_directories(steelc_core
PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}"
)

target_link_libraries(steelc_tests PRIVATE
	steelc_core
)
