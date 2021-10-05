include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)


enable_testing()

add_executable(
  bump_test
  main.cpp
  rect_spec.cpp
  world_spec.cpp
  response_spec.cpp
)
target_link_libraries(
  bump_test
  PUBLIC
  bump
  gtest
  gmock
  gtest_main
  gmock_main
)
list(APPEND googletest_includes
    "${FETCHCONTENT_BASE_DIR}/googletest-src/googletest/include"
    "${FETCHCONTENT_BASE_DIR}/googletest-src/googlemock/include"
)
target_include_directories(bump_test PUBLIC ${googletest_includes})

include(GoogleTest)
gtest_discover_tests(bump_test)
