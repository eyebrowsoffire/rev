include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

add_executable(testGame)

target_sources(testGame PRIVATE
  src/main.cpp
)

target_compile_features(testGame PRIVATE cxx_std_17)

target_link_libraries(testGame PRIVATE
  revEngine
)

file(COPY assets DESTINATION ${CMAKE_BINARY_DIR})
