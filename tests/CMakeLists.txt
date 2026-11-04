add_executable(test_status test_status.cpp)
target_link_libraries(test_status PRIVATE cpu_collectives::cpu_collectives)
add_test(NAME status COMMAND test_status)

add_executable(test_lifecycle test_lifecycle.cpp)
target_link_libraries(test_lifecycle PRIVATE cpu_collectives::cpu_collectives)
add_test(NAME lifecycle COMMAND test_lifecycle)

add_executable(test_barrier test_barrier.cpp)
target_link_libraries(test_barrier PRIVATE cpu_collectives::cpu_collectives)
add_test(NAME barrier COMMAND test_barrier)

add_executable(test_all_reduce test_all_reduce.cpp)
target_link_libraries(test_all_reduce PRIVATE cpu_collectives::cpu_collectives)
add_test(NAME all_reduce COMMAND test_all_reduce)
set_tests_properties(all_reduce PROPERTIES TIMEOUT 30)

add_executable(stress_barrier stress/stress_barrier.cpp)
target_link_libraries(stress_barrier PRIVATE cpu_collectives::cpu_collectives)
add_test(NAME stress_barrier COMMAND stress_barrier)
set_tests_properties(stress_barrier PROPERTIES TIMEOUT 30)
