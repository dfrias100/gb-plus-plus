add_library(Cartridge
    Cartridge.cpp
    Cartridge.hpp
)

target_link_libraries(Cartridge MBC0 MBC1 MBC2 MBC3 MBC5 gb++)
