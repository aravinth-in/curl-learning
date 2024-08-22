from conan import ConanFile
from conan.tools.cmake import CMakeToolchain, CMake, cmake_layout

class MyProjectConan(ConanFile):
    name = "curl-learning"
    version = "1.0.0"
    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeToolchain", "CMakeDeps"

    requires = "libcurl/8.8.0"
    
    def layout(self):
        cmake_layout(self)

    def generate(self):
        tc = CMakeToolchain(self)
        tc.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        cmake = CMake(self)
        cmake.install()

    def package_info(self):
        self.cpp_info.libs = ["curl-learning"]