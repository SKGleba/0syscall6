cmake_minimum_required(VERSION 2.8)

set(CMAKE_SYSTEM_NAME "Generic")
set(CMAKE_C_COMPILER "arm-vita-eabi-gcc")
set(CMAKE_CXX_COMPILER "arm-vita-eabi-g++")

project(HENkaku)

set(CMAKE_C_FLAGS "-Wl,-q -Wall -O3 -std=gnu99")
set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -std=c++11 -fno-rtti -fno-exceptions")

include_directories(
)

link_directories(
	${CMAKE_CURRENT_BINARY_DIR}
	${CMAKE_CURRENT_BINARY_DIR}/../../psp2renga/Kernel/stubs/
)

add_executable(0syscall6.elf
	main.c
)

target_link_libraries(0syscall6.elf
	rengaKernel_stub
)

set_target_properties(0syscall6.elf
	PROPERTIES LINK_FLAGS "-nostdlib"
	COMPILE_FLAGS "-D__VITA_KERNEL__"
)

add_custom_target(0syscall6.skprx ALL
	COMMAND vita-elf-create -e ${CMAKE_SOURCE_DIR}/0syscall6.yml 0syscall6.elf 0syscall6.velf
	COMMAND vita-make-fself -c 0syscall6.velf 0syscall6.skprx
)
add_dependencies(0syscall6.skprx 0syscall6.elf)