include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

add_library(rev STATIC)

target_sources(rev PRIVATE
  include/rev/Camera.h
  include/rev/CompositeModel.h
  include/rev/CurveFile.h
  include/rev/DebugOverlay.h
  include/rev/DrawMaterialsProgram.h
  include/rev/Engine.h
  include/rev/Environment.h
  include/rev/IActor.h
  include/rev/IKeyboardListener.h
  include/rev/IntegerSequenceUtilities.h
  include/rev/MaterialProperties.h
  include/rev/Mesh.h
  include/rev/MtlFile.h
  include/rev/ObjFile.h
  include/rev/NurbsCurve.h
  include/rev/ProgramFactory.h
  include/rev/RenderStage.h
  include/rev/Scene.h
  include/rev/SceneObjectGroup.h
  include/rev/SceneView.h
  include/rev/Types.h
  include/rev/Unit.h
  include/rev/Utilities.h
  include/rev/WavefrontHelpers.h
  include/rev/Window.h

  include/rev/geometry/KDTree.h
  include/rev/geometry/Tools.h

  include/rev/gl/Buffer.h
  include/rev/gl/Context.h
  include/rev/gl/FrameBuffer.h
  include/rev/gl/OpenGL.h
  include/rev/gl/ProgramResource.h
  include/rev/gl/Resource.h
  include/rev/gl/Texture.h
  include/rev/gl/Uniform.h
  include/rev/gl/VertexArray.h

  include/rev/lights/LightShaders.h
  include/rev/lights/LightModels.h

  include/rev/physics/Gravity.h
  include/rev/physics/Particle.h
  include/rev/physics/PhysicsUnits.h
  include/rev/physics/System.h

  include/rev/shaders/Program.h
  include/rev/shaders/Shader.h
  include/rev/shaders/ShaderLibrary.h

  include/rev/track/ExtrusionTrackElement.h
  include/rev/track/ITrackElement.h
  include/rev/track/TrackBuilder.h
  include/rev/track/TrackModel.h
  include/rev/track/TrackVertexData.h

  src/Camera.cpp
  src/CurveFile.cpp
  src/DebugOverlay.cpp
  src/Engine.cpp
  src/Environment.cpp
  src/MtlFile.cpp
  src/ObjFile.cpp
  src/Scene.cpp
  src/SceneView.cpp
  src/WavefrontHelpers.cpp
  src/Window.cpp

  src/lights/LightModel.cpp

  src/physics/Gravity.cpp
  src/physics/Particle.cpp
  src/physics/System.cpp

  src/shaders/Program.cpp
  src/shaders/Shader.cpp
  src/shaders/ShaderLibrary.cpp

  src/track/ExtrusionTrackElement.cpp
  src/track/TrackBuilder.cpp
  src/track/TrackModel.cpp
)

target_include_directories(rev PUBLIC include)
target_compile_features(rev PRIVATE cxx_std_17)

target_link_libraries(rev PRIVATE
  CONAN_PKG::fmt
  CONAN_PKG::glad
  CONAN_PKG::glfw
  CONAN_PKG::glm
  CONAN_PKG::gsl_microsoft
)

add_subdirectory(tests)