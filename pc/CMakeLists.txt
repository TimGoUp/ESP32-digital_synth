add_executable(test_parser tests/test_parser.cpp)
target_link_libraries(test_parser PRIVATE synth_core)
add_test(NAME parser COMMAND test_parser)
 
# Live MIDI monitor (PC only)
include(FetchContent)
FetchContent_Declare(rtmidi
  GIT_REPOSITORY https://github.com/thestk/rtmidi.git
  GIT_TAG 6.0.0)
set(RTMIDI_BUILD_TESTING OFF CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(rtmidi)
 
add_executable(midi_monitor midi_monitor.cpp)
target_link_libraries(midi_monitor PRIVATE synth_core rtmidi)