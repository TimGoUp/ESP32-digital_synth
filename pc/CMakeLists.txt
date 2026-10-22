add_executable(test_parser tests/test_parser.cpp)
target_link_libraries(test_parser PRIVATE synth_core)
add_test(NAME parser COMMAND test_parser)
