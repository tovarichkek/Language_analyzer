cmake_minimum_required(VERSION 3.10)

project(LR0)

find_package(GTest)

###LR0_MAIN
add_executable(LR0_main 
    src/main.cpp 
    src/lex.yy.cc 
    src/analys.cpp
)
target_include_directories(LR0_main PRIVATE "${CMAKE_SOURCE_DIR}/include")
target_compile_definitions(LR0_main PRIVATE PROJECT_PATH="${CMAKE_SOURCE_DIR}")




###RL0_testing
if(NOT GTest_FOUND)
    message(WARNING "Did not find GTEST LIBS")
else()
    add_executable(LR0_test
        src/analys.cpp 
        test/test.cpp 
        src/lex.yy.cc
    )
    target_include_directories(LR0_test PRIVATE "${CMAKE_SOURCE_DIR}/include")
    target_compile_definitions(LR0_test PRIVATE PROJECT_PATH="${CMAKE_SOURCE_DIR}")
    target_link_libraries(LR0_test GTest::gtest GTest::gtest_main)
    target_link_libraries(LR0_test PRIVATE ${FL_LIBRARY})
endif()











