cmake_minimum_required(VERSION 3.20)
project(go_back_n C)

add_compile_options(-Wall -Werror)

add_executable(go_back_n)

target_sources(go_back_n PUBLIC
	src/main.c
	src/sender.c
	src/receiver.c
	src/pipe.c
	src/player.c
	src/gbn.c
)

target_link_libraries(go_back_n)
set_property(TARGET go_back_n PROPERTY C_STANDARD 11)
set_property(TARGET go_back_n PROPERTY CMAKE_EXPORT_COMPILE_COMMANDS true)