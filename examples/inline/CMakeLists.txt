add_executable(inline_1 inline_1.cpp)
target_link_libraries(inline_1 executors)

add_executable(inline_2 inline_2.cpp)
target_link_libraries(inline_2 executors)

executors_add_example(inline_3)
executors_add_example(inline_4)
executors_add_example(inline_5)
executors_add_example(inline_6)
executors_add_example(inline_7)
executors_add_example(inline_8)
