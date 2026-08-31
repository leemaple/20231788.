cmake_minimum_required(VERSION 3.16.3)

project(openfhe_2023_1788 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(OpenFHE 1.5.0 CONFIG REQUIRED)

if(NOT BASE_OPENFHE_VERSION VERSION_EQUAL "1.5.0")
    message(FATAL_ERROR "This project requires official OpenFHE 1.5.0; found ${BASE_OPENFHE_VERSION}")
endif()

if(UNIX AND NOT APPLE)
    add_link_options(-Wl,--no-as-needed)
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenFHE_CXX_FLAGS}")

include_directories(${OPENMP_INCLUDES})
include_directories(SYSTEM
    ${OpenFHE_INCLUDE}
    ${OpenFHE_INCLUDE}/third-party/include
    ${OpenFHE_INCLUDE}/core
    ${OpenFHE_INCLUDE}/pke
    ${OpenFHE_INCLUDE}/binfhe
)
link_directories(${OpenFHE_LIBDIR} ${OPENMP_LIBRARIES})

add_library(openfhe_2023_1788 src/double_ckks.cpp)
target_include_directories(openfhe_2023_1788 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(openfhe_2023_1788 PUBLIC ${OpenFHE_SHARED_LIBRARIES})
if(MINGW)
    target_compile_definitions(openfhe_2023_1788 PUBLIC _USE_MATH_DEFINES)
endif()

add_executable(dcp_rcb_test tests/dcp_rcb_test.cpp)
target_link_libraries(dcp_rcb_test PRIVATE openfhe_2023_1788)

if(MSVC)
    target_compile_options(openfhe_2023_1788 PRIVATE /W4 /WX)
    target_compile_options(dcp_rcb_test PRIVATE /W4 /WX)
else()
    target_compile_options(openfhe_2023_1788 PRIVATE -Wall -Wextra -Wpedantic -Werror)
    target_compile_options(dcp_rcb_test PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()

enable_testing()
add_test(NAME dcp_rcb COMMAND dcp_rcb_test)
