cmake_minimum_required(VERSION 3.25)
project(laser_keyboard VERSION 0.1 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

# Absolute python path baked into the binary so the tracker spawns regardless of
# the launch shell's PATH or working directory.
find_program(PYTHON_EXECUTABLE NAMES python python3 py)
if(NOT PYTHON_EXECUTABLE)
  set(PYTHON_EXECUTABLE "python")
endif()
configure_file(${CMAKE_SOURCE_DIR}/src/generated/tracker_paths.h.in
               ${CMAKE_BINARY_DIR}/generated/tracker_paths.h @ONLY)

# --- Tests ---
enable_testing()
add_executable(unit_tests
  tests/test_main.cpp
  tests/test_keyboard_layout.cpp
  tests/test_config.cpp
  tests/test_calibration.cpp
  tests/test_tap_detector.cpp
  tests/test_key_injector.cpp
  src/keyboard/KeyboardLayout.cpp
  src/keyboard/KeyDetector.cpp
  src/keyboard/TapDetector.cpp
  src/config/Config.cpp
  src/input/KeyInjector.cpp
)
target_include_directories(unit_tests PRIVATE
  ${PROJECT_SOURCE_DIR}/src
  ${PROJECT_SOURCE_DIR}/third_party/doctest
  ${PROJECT_SOURCE_DIR}/third_party
)
target_link_libraries(unit_tests PRIVATE user32) # KeyInjector -> SendInput
add_test(NAME unit_tests COMMAND unit_tests)

# --- Overlay app (tracking comes from the Python MediaPipe sidecar over UDP) ---
add_executable(laser_keyboard
  src/main.cpp
  src/window/OverlayWindow.cpp
  src/renderer/Renderer.cpp
  src/renderer/ShaderProgram.cpp
  src/renderer/KeyRenderer.cpp
  src/renderer/TextRenderer.cpp
  src/keyboard/KeyboardLayout.cpp
  src/keyboard/KeyDetector.cpp
  src/keyboard/Calibrator.cpp
  src/keyboard/TapDetector.cpp
  src/config/Config.cpp
  src/input/LandmarkReceiver.cpp
  src/input/KeyInjector.cpp
  third_party/glad/src/glad.c
)
target_include_directories(laser_keyboard PRIVATE
  ${PROJECT_SOURCE_DIR}/src
  ${PROJECT_SOURCE_DIR}/third_party/glad/include
  ${PROJECT_SOURCE_DIR}/third_party
  ${CMAKE_BINARY_DIR}/generated
)
target_link_libraries(laser_keyboard PRIVATE opengl32 gdi32 user32 dwmapi ws2_32)
if(WIN32)
  target_compile_definitions(laser_keyboard PRIVATE PLATFORM_WINDOWS)
endif()
