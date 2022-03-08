run the following command in the build dir to prepare Visual Studio projects:
(the build dir is assumed to be the sibling of the src)
cmake.exe ../src -DCMAKE_TOOLCHAIN_FILE=D:\Projects\vcpkg/scripts/buildsystems/vcpkg.cmake

The above command must be run in Visual Studio 2019 command line prompt.
The vcpkg should be installed.
The CMAKE_TOOLCHAIN_FILE is located in <vcpkg root dir>/scripts/buildsystems/vcpkg.cmake

The vcpkg packages don't have to be installed.
The vcpkg.json file takes care of all that.
