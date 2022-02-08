add_subdirectory(MBC0)
add_subdirectory(MBC1)
add_subdirectory(MBC2)
add_subdirectory(MBC3)
add_subdirectory(MBC5)

add_library(Mapper
	Mapper.cpp
	Mapper.hpp
)

target_link_libraries(Mapper gb++)
