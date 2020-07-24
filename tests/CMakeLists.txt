find_package(GTest REQUIRED)
include(GoogleTest)
enable_testing()

add_executable(bamf-tests
    dummy.cc)
target_link_libraries(bamf-tests PRIVATE bamf GTest::Main)
gtest_discover_tests(bamf-tests)
