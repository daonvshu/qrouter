cmake_minimum_required(VERSION 3.12)

project(qrouter)

add_subdirectory(src)

option(QROUTER_BUILD_TEST "enable the test project of qrouter" OFF)

if (QROUTER_BUILD_TEST)
add_subdirectory(test)
endif()