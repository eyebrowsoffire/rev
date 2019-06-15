add_executable(revTests)

target_compile_features(revTests PRIVATE cxx_std_17)

target_sources(revTests PRIVATE
  GeometryToolsTests.cpp
  IntegerSequenceUtilitiesTests.cpp
  KDTreeTests.cpp
  NurbsCurveTests.cpp
  UnitUnitTests.cpp
)

target_link_libraries(revTests PRIVATE
  CONAN_PKG::gtest
  revEngine
)
