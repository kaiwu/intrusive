set(TEST_FILES
    catch_main.cpp
    test_intrusive.cpp
)

add_executable(intrusive ${TEST_FILES})
target_include_directories(intrusive PRIVATE ${PROJECT_SOURCE_DIR}/include)
add_test(NAME test_intrusive COMMAND intrusive)
  

