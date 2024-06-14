##
# Copyright (C) 2019 Adaptech s.r.o., Robert Pösel
#
# Licensed under the Apache License, Version 2.0 (the "License"); you may not
# use this file except in compliance with the License. You may obtain a copy of
# the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
# WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
# License for the specific language governing permissions and limitations under
# the License.
##

# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.18.1)
project("Tesseract4Android")

# Tesseract requires newer C++ standard, so let's enable it for all libraries
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Adds the CMakeLists.txt file located in the specified directory as a build dependency.
add_subdirectory(libjpeg)
add_subdirectory(libpng)
add_subdirectory(leptonica)
add_subdirectory(tesseract)
#
#add_library( # Sets the name of the library.
#        native-lib
#
#        # Sets the library as a shared library.
#        SHARED
#
#        # Provides a relative path to your source file(s).
#        native-lib.cpp)
#
## Searches for a specified prebuilt library and stores the path as a
## variable. Because CMake includes system libraries in the search path by
## default, you only need to specify the name of the public NDK library
## you want to add. CMake verifies that the library exists before
## completing its build.
#
#find_library( # Sets the name of the path variable.
#        log-lib
#
#        # Specifies the name of the NDK library that
#        # you want CMake to locate.
#        log)
#
## Specifies libraries CMake should link to your target library. You
## can link multiple libraries, such as libraries you define in this
## build script, prebuilt third-party libraries, or system libraries.
#
#target_link_libraries( # Specifies the target library.
#        native-lib
#
#        lib_opencv
#
#        # Links the target library to the log library
#        # included in the NDK.
#        ${log-lib})