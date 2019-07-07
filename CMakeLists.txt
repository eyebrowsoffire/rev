cmake_minimum_required(VERSION 3.14.5)
project(rev CXX)

if(CONAN_EXPORTED)
  include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
  conan_basic_setup(TARGETS)
else()
  include(conan.cmake)
  conan_check(VERSION 1.16.1 REQUIRED)
  conan_add_remote(NAME bincrafters URL https://api.bintray.com/conan/bincrafters/public-conan)
  conan_cmake_run(CONANFILE conanfile.txt
    BASIC_SETUP CMAKE_TARGETS
    BUILD missing
  )
endif()

add_subdirectory(engine)
add_subdirectory(testGame)
