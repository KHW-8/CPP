from conan import ConanFile
from conan.tools.cmake import CMake, CMakeToolchain, cmake_layout

class Dependency(ConanFile):
    settings = "arch", "build_type", "compiler", "os"
    generators = "CMakeDeps"

    default_options = {
        "*:shared": True,
        "gmp/*:shared": False,
        "qt/*:with_pq": False,
        "boost/*:without_cobalt": True
    }

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def generate(self):
        # Set CMake tool chain
        tc = CMakeToolchain(self)
        tc.user_presets_path = "../CMakeUserPresets.json"
        tc.generate()

    def layout(self):
        cmake_layout(self)

    def requirements(self):
        self.requires("opencv/4.14.0")
        
