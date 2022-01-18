# gb-plus-plus
Gameboy Emulator in C++ with SFML 2.5

This is gameboy emulator that is a work in progress! Many parts have not yet been implemented. Here's the current progress as of commit [c4cc72f8](https://github.com/dfrias100/gb-plus-plus/commit/c4cc72f8b6aa9998f1bd341540b9a3c0a8cd78f9):
* CPU: 207 of 500 instructions implemented (41.4%)
	-  4 of 260 Prefix 0xCB shift, rotate, bit insturctions (1.5%)
	- 86 of 92 8-bit ALU instructions (93.5%)
	- 79 of 85 8-bit load instructions (92.9%)
	- 16 of 30 Jump / Call instructions (46.7%)
	-  7 of 15 16-bit load instructions (33.3%)
	- 12 of 13 16-bit ALU instructions (92.3%)
	-  3 of 5 Miscellaneous instructions (60.0%)*
		- 2 of these instructions are partially implemented due to lack of general system implementation
* Memory: Framework established but no components can write to any memory yet. CPU can be ticked.
  - Cartridge mapping: ROM only right now, no mappers implemented.
* Video: No progress yet.
* Audio: No progress yet, it may not even be implemented at all.
* Input: Not available yet, as at least CPU, Memory, and Video have to be somewhat implemented.
* General: Barebones console program, only opens an SFML window with a white background. I may implement a basic UI later.
