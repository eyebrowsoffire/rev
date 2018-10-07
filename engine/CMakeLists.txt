include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

add_library(revEngine STATIC)

target_sources(revEngine PRIVATE
  include/rev/BasicModel.h
  include/rev/Camera.h
  include/rev/Engine.h
  include/rev/Environment.h
  include/rev/IActor.h
  include/rev/IKeyboardListener.h
  include/rev/IModel.h
  include/rev/IndexedModel.h
  include/rev/Light.h
  include/rev/ObjFile.h
  include/rev/RenderStage.h
  include/rev/Scene.h
  include/rev/SceneObject.h
  include/rev/SceneView.h
  include/rev/Types.h
  include/rev/Utilities.h
  include/rev/Window.h

  include/rev/gl/Buffer.h
  include/rev/gl/Context.h
  include/rev/gl/FrameBuffer.h
  include/rev/gl/OpenGL.h
  include/rev/gl/Program.h
  include/rev/gl/Resource.h
  include/rev/gl/Texture.h
  include/rev/gl/Uniform.h
  include/rev/gl/VertexArray.h

  src/BasicModel.cpp
  src/Camera.cpp
  src/Engine.cpp
  src/Environment.cpp
  src/IndexedModel.cpp
  src/Light.cpp
  src/ObjFile.cpp
  src/Scene.cpp
  src/SceneObject.cpp
  src/SceneView.cpp
  src/Window.cpp
)

target_include_directories(revEngine PUBLIC include)
target_compile_features(revEngine PRIVATE cxx_std_17)

target_link_libraries(revEngine PRIVATE
  ${CONAN_LIBS}
)
