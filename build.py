import os
import subprocess
import sys

def run_command(command):
    result = subprocess.run(command, shell=True)
    if result.returncode != 0:
        print(f"Command failed: {command}")
        sys.exit(result.returncode)

def main():
    # Get the directory where build.py is located
    script_dir = os.path.dirname(os.path.abspath(__file__))
    project_path = script_dir
    build_path = os.path.join(project_path, "build")
    configuration = "Debug"  # Change to "Release" if needed

    # Create build directory if it doesn't exist
    if not os.path.exists(build_path):
        os.makedirs(build_path)

    # Install Conan
    run_command("pip install conan==1.64.0 --index-url https://pypi.org/simple")

    # Run Conan install
    conan_install_command = f'conan install . --install-folder="{build_path}" --build=missing'
    run_command(conan_install_command)

    # Detect platform and set CMake generator and options accordingly
    if sys.platform == "darwin":
        # macOS
        cmake_generator = "Xcode"
        cmake_architecture = "-DCMAKE_OSX_ARCHITECTURES=arm64"
        cmake_configure_command = f'cmake -S "{project_path}" -B "{build_path}" -G "{cmake_generator}" {cmake_architecture}'
    elif sys.platform == "win32":
        # Windows
        cmake_generator = "Visual Studio 17 2022"
        cmake_architecture = "-A x64"
        cmake_configure_command = f'cmake -S "{project_path}" -B "{build_path}" -G "{cmake_generator}" {cmake_architecture}'
    else:
        print("Unsupported platform")
        sys.exit(1)

    # Run CMake configuration command
    run_command(cmake_configure_command)

    # Run CMake build command
    cmake_build_command = f'cmake --build "{build_path}" --config {configuration} --verbose'
    run_command(cmake_build_command)

if __name__ == "__main__":
    main()