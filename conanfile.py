from conans import ConanFile, CMake, tools


class RevConan(ConanFile):
    name = "rev"
    version = "0.1"
    license = "MIT"
    author = "Jackson Gardner (eyebrowsoffire@gmail.com)"
    url = "https://github.com/eyebrowsoffire/rev"
    description = "A Toy 3D Game Engine"
    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False]}
    default_options = "shared=False"
    generators = "cmake"
    scm = {
        "type": "git",
        "url": "auto",
        "revision": "auto"
    }
    requires = "glad/0.1.29@bincrafters/stable", "glfw/3.3@bincrafters/stable", "gsl_microsoft/2.0.0@bincrafters/stable", "glm/0.9.9.4@g-truc/stable", "gtest/1.8.1@bincrafters/stable", "fmt/6.0.0@bincrafters/stable"

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        self.copy("*.h", dst="include", src="engine/include")
        self.copy("engine/lib/librev.a", dst="lib", keep_path=False)

    def package_info(self):
        if not self.in_local_cache:
            self.cpp_info.libdirs = ["build/engine/lib"]
            self.cpp_info.includedirs = ["engine/include"]
        
        self.cpp_info.libs = ["rev"]

