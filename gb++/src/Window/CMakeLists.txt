add_library(Window
	Window.cpp
	Window.hpp
)

target_link_libraries(Window sfml-graphics sfml-window sfml-system gb++)
