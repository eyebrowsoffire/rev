include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

add_library(revEngine STATIC)

target_sources(revEngine PRIVATE
  include/rev/Actor.h
  include/rev/Camera.h
  include/rev/Engine.h
  include/rev/Environment.h
  include/rev/Scene.h
  include/rev/SceneView.h
  include/rev/Types.h
  include/rev/Window.h

  src/Camera.cpp
  src/Engine.cpp
  src/Environment.cpp
  src/SceneView.cpp
  src/Window.cpp
)

target_include_directories(revEngine PUBLIC include)
target_compile_features(revEngine PRIVATE cxx_std_17)

target_link_libraries(revEngine PRIVATE
  ${CONAN_LIBS}
)
