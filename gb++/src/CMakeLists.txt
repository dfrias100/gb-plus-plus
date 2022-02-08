add_subdirectory(Window)
add_subdirectory(Sharp)
add_subdirectory(Mapper)
add_subdirectory(Input)
add_subdirectory(Memory)
add_subdirectory(PPU)
add_subdirectory(Cartridge)

add_executable(gb++ main.cpp)

target_link_libraries(gb++ Memory Window Cartridge Input)
