add_library(bamf io/InputFile.cc)
target_compile_features(bamf PUBLIC cxx_std_20)
target_include_directories(bamf PUBLIC ${PROJECT_SOURCE_DIR}/include)
