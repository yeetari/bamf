add_executable(bamf-tests
    graph/DepthFirstSearchTest.cc
    graph/DominanceComputerTest.cc
    graph/GraphTest.cc
    ir/ValueTest.cc
    support/StackTest.cc
    x86/BackendTest.cc
    x86/DecoderTest.cc)
target_link_libraries(bamf-tests PRIVATE bamf GTest::Main)
gtest_discover_tests(bamf-tests)
