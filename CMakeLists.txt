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

# Compile-only public API contract. It is intentionally not a CTest entry.
add_executable(tensor2_api_contract_test tests/tensor2_api_contract_test.cpp)
target_link_libraries(tensor2_api_contract_test PRIVATE openfhe_2023_1788)

# Compile-only public API contract. It is intentionally excluded from the
# default build and is not a CTest entry, so the accepted production suite can
# build before this red-first seam is compiled explicitly.
add_executable(relin2_api_contract_test EXCLUDE_FROM_ALL tests/relin2_api_contract_test.cpp)
target_link_libraries(relin2_api_contract_test PRIVATE openfhe_2023_1788)

# Compile-only public API contract. It is intentionally excluded from the
# default build and is not a CTest entry, so this red-first seam is compiled
# explicitly by CI before the production API exists.
add_executable(rs2_api_contract_test EXCLUDE_FROM_ALL tests/rs2_api_contract_test.cpp)
target_link_libraries(rs2_api_contract_test PRIVATE openfhe_2023_1788)

# The first Mult2 slice intentionally proves the absent public API in CI.
add_executable(mult2_api_contract_test EXCLUDE_FROM_ALL tests/mult2_api_contract_test.cpp)
target_link_libraries(mult2_api_contract_test PRIVATE openfhe_2023_1788)

add_executable(tensor2_test tests/tensor2_test.cpp)
target_link_libraries(tensor2_test PRIVATE openfhe_2023_1788)

add_executable(relin2_test tests/relin2_test.cpp)
target_link_libraries(relin2_test PRIVATE openfhe_2023_1788)

add_executable(rs2_test tests/rs2_test.cpp)
target_link_libraries(rs2_test PRIVATE openfhe_2023_1788)

add_executable(mult2_test tests/mult2_test.cpp)
target_link_libraries(mult2_test PRIVATE openfhe_2023_1788)

if(MSVC)
    target_compile_options(openfhe_2023_1788 PRIVATE /W4 /WX)
    target_compile_options(dcp_rcb_test PRIVATE /W4 /WX)
    target_compile_options(tensor2_api_contract_test PRIVATE /W4 /WX)
    target_compile_options(relin2_api_contract_test PRIVATE /W4 /WX)
    target_compile_options(rs2_api_contract_test PRIVATE /W4 /WX)
    target_compile_options(mult2_api_contract_test PRIVATE /W4 /WX)
    target_compile_options(tensor2_test PRIVATE /W4 /WX)
    target_compile_options(relin2_test PRIVATE /W4 /WX)
    target_compile_options(rs2_test PRIVATE /W4 /WX)
    target_compile_options(mult2_test PRIVATE /W4 /WX)
else()
    target_compile_options(openfhe_2023_1788 PRIVATE -Wall -Wextra -Wpedantic -Werror)
    target_compile_options(dcp_rcb_test PRIVATE -Wall -Wextra -Wpedantic -Werror)
    target_compile_options(tensor2_api_contract_test PRIVATE -Wall -Wextra -Wpedantic -Werror)
    target_compile_options(relin2_api_contract_test PRIVATE -Wall -Wextra -Wpedantic -Werror)
    target_compile_options(rs2_api_contract_test PRIVATE -Wall -Wextra -Wpedantic -Werror)
    target_compile_options(mult2_api_contract_test PRIVATE -Wall -Wextra -Wpedantic -Werror)
    target_compile_options(tensor2_test PRIVATE -Wall -Wextra -Wpedantic -Werror)
    target_compile_options(relin2_test PRIVATE -Wall -Wextra -Wpedantic -Werror)
    target_compile_options(rs2_test PRIVATE -Wall -Wextra -Wpedantic -Werror)
    target_compile_options(mult2_test PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()

enable_testing()
add_test(NAME dcp_rcb COMMAND dcp_rcb_test)
add_test(NAME tensor2_valid_arithmetic_immutability COMMAND tensor2_test valid_arithmetic_immutability)
add_test(NAME tensor2_result_scale_contract COMMAND tensor2_test result_scale_contract)
add_test(NAME tensor2_right_input_validation COMMAND tensor2_test right_input_validation)
add_test(NAME tensor2_mutual_compatibility COMMAND tensor2_test mutual_compatibility)
add_test(NAME tensor2_prearithmetic_key_compatibility COMMAND tensor2_test prearithmetic_key_compatibility)
add_test(NAME relin2_tensor_validation_order COMMAND relin2_test tensor_validation_order)
add_test(NAME relin2_insufficient_active_basis COMMAND relin2_test insufficient_active_basis)
add_test(NAME relin2_missing_eval_key COMMAND relin2_test missing_eval_key)
add_test(NAME relin2_key_empty COMMAND relin2_test key_empty)
add_test(NAME relin2_key_null_first COMMAND relin2_test key_null_first)
add_test(NAME relin2_key_wrong_context COMMAND relin2_test key_wrong_context)
add_test(NAME relin2_key_wrong_tag COMMAND relin2_test key_wrong_tag)
add_test(NAME relin2_key_wrong_subtype COMMAND relin2_test key_wrong_subtype)
add_test(NAME relin2_key_hybrid_a_length COMMAND relin2_test key_hybrid_a_length)
add_test(NAME relin2_key_hybrid_b_length COMMAND relin2_test key_hybrid_b_length)
add_test(NAME relin2_key_hybrid_entry_basis COMMAND relin2_test key_hybrid_entry_basis)
add_test(NAME relin2_key_hybrid_entry_format COMMAND relin2_test key_hybrid_entry_format)
add_test(NAME relin2_key_bv_zero_digit_a_length COMMAND relin2_test key_bv_zero_digit_a_length)
add_test(NAME relin2_key_bv_zero_digit_b_length COMMAND relin2_test key_bv_zero_digit_b_length)
add_test(NAME relin2_key_bv_nonzero_digit_a_length COMMAND relin2_test key_bv_nonzero_digit_a_length)
add_test(NAME relin2_key_bv_nonzero_digit_b_length COMMAND relin2_test key_bv_nonzero_digit_b_length)
add_test(NAME relin2_key_bv_zero_digit_entry_basis COMMAND relin2_test key_bv_zero_digit_entry_basis)
add_test(NAME relin2_key_bv_nonzero_digit_entry_basis COMMAND relin2_test key_bv_nonzero_digit_entry_basis)
add_test(NAME relin2_key_bv_zero_digit_entry_format COMMAND relin2_test key_bv_zero_digit_entry_format)
add_test(NAME relin2_key_bv_nonzero_digit_entry_format COMMAND relin2_test key_bv_nonzero_digit_entry_format)
add_test(NAME relin2_valid_arithmetic_state_immutability COMMAND relin2_test valid_arithmetic_state_immutability)
add_test(NAME relin2_controlled_witnesses_and_boundaries COMMAND relin2_test controlled_witnesses_and_boundaries)
add_test(NAME relin2_representative_public_input COMMAND relin2_test representative_public_input)
add_test(NAME relin2_key_extra_later_valid COMMAND relin2_test key_extra_later_valid)
add_test(NAME relin2_key_malformed_later_ignored COMMAND relin2_test key_malformed_later_ignored)
add_test(NAME relin2_hybrid_valid_shapes COMMAND relin2_test hybrid_valid_shapes)
add_test(NAME relin2_bv_zero_digit_valid_shapes COMMAND relin2_test bv_zero_digit_valid_shapes)
add_test(NAME relin2_bv_nonzero_digit_valid_shapes COMMAND relin2_test bv_nonzero_digit_valid_shapes)
add_test(NAME relin2_first_recombined_rcb_validation COMMAND relin2_test first_recombined_rcb_validation)
add_test(NAME relin2_first_recombined_tensor2_validation COMMAND relin2_test first_recombined_tensor2_validation)
add_test(NAME relin2_tensor2_requires_first_lifecycle COMMAND relin2_test tensor2_requires_first_lifecycle)
add_test(NAME rs2_wrong_lifecycle COMMAND rs2_test wrong_lifecycle)
add_test(NAME rs2_valid_arithmetic_state_immutability
         COMMAND rs2_test valid_arithmetic_state_immutability)

add_test(NAME mult2_composition_contract COMMAND mult2_test composition_contract)
