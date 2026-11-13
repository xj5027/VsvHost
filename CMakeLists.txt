cmake_minimum_required(VERSION 3.12 FATAL_ERROR)
project(SJ2414_VSVHOST)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()
message("CMAKE_BUILD_TYPE = ${CMAKE_BUILD_TYPE}")


if (MSVC)
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${PROJECT_SOURCE_DIR}/bin64)
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${PROJECT_SOURCE_DIR}/bin64)
  set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${PROJECT_SOURCE_DIR}/lib)
else()
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin64)
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin64)
  set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)  
endif()





file(GLOB_RECURSE ${PROJECT_NAME}_SOURCE code/VsvHost/*.cpp  code/VsvHost/*.h  code/common/*.h  code/common/*.cpp)

add_executable(${PROJECT_NAME} ${${PROJECT_NAME}_SOURCE})

target_compile_definitions(${PROJECT_NAME} PRIVATE
  _MBCS
  _UNICODE
  UNICODE
)
target_include_directories(${PROJECT_NAME} PRIVATE
  #SYSTEM 
  #${PROJECT_SOURCE_DIR} 
  ${PROJECT_SOURCE_DIR}/code/common/
  ${PROJECT_SOURCE_DIR}/dependencies/include/
  ${PROJECT_SOURCE_DIR}/dependencies/include/boost-1_74/
)

# target_link_directories(${PROJECT_NAME} PRIVATE 
#   ${VIWO_SDK_DIRECTORY}/lib
#   ${VIWO_SDK_DIRECTORY}/bin64
#   ${VIWO_SDK_DIRECTORY}/dependencies/lib
# )
if(MSVC)
  target_compile_definitions(${PROJECT_NAME} PRIVATE _CONSOLE _CRT_SECURE_NO_WARNINGS)
else()
  set(ALL_LIBS ${ALL_LIBS} stdc++fs pthread)
  add_compile_options("-ldl -rdynamic")
endif(MSVC)

target_link_libraries(${PROJECT_NAME} PRIVATE ${ALL_LIBS})
