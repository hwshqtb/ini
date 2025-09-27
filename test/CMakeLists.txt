function(common_test dir)
    add_executable(test_${dir} ${dir}/main.cpp)
    target_link_libraries(test_${dir} PRIVATE hwshqtb::ini)
endfunction()

common_test("aimed")
common_test("value")