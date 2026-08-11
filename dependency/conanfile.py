from conan import ConanFile
from conan.tools.files import copy
from conan.tools.cmake import CMake, CMakeToolchain, cmake_layout
import os

class Dependency(ConanFile):
    settings = "arch", "build_type", "compiler", "os"
    generators = "CMakeDeps"

    default_options = {
        "*:shared": True,
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

        # Copy dependencies to build folder
        asset_folder = os.path.join(self.build_folder, "..", "asset")

        for dep in self.dependencies.values():
            # Copy headers
            for includedir in dep.cpp_info.includedirs:
                copy(self, 
                    pattern="*", 
                    src=includedir, 
                    dst=os.path.join(asset_folder, "include", dep.ref.name, str(dep.ref.version))
                )
                

            # Copy sources
            for srcdir in dep.cpp_info.srcdirs:
                copy(self, 
                    pattern="*", 
                    src=srcdir, 
                    dst=os.path.join(asset_folder, "src", dep.ref.name, str(dep.ref.version))
                )

    def layout(self):
        cmake_layout(self)

    def requirements(self):
        self.requires("boost/1.91.0")
        # self.requires("gtest/1.17.0")
        # self.requires("opencv/4.14.0")
        # self.requires("qt/6.11.1")
        self.requires("serial/1.2.1")