# gb-plus-plus
Gameboy Emulator in C++ with SFML 2.5

This is gameboy emulator that is a work in progress! Many parts have not yet been implemented. Here's the current progress as of commit [20d12d5](https://github.com/dfrias100/gb-plus-plus/commit/20d12d56110604aa3997385eb403757cd567615e):
* CPU: 500 of 500 instructions implemented (100.0%). All instructions and interrupt handling have been implemented. Timer emulation is also implemented, LCD updating is implemented.
	- 260 of 260 Prefix 0xCB shift, rotate, bit instructions (100.0%)
	- 92 of 92 8-bit ALU instructions (100.0%)
	- 85 of 85 8-bit load instructions (100.0%)
	- 30 of 30 Jump / Call instructions (100.0%)
	- 15 of 15 16-bit load instructions (100.0%)
	- 13 of 13 16-bit ALU instructions (100.0%)
	-  5 of 5 Miscellaneous instructions (100.0%)*
		- 1 of these instructions are partially implemented due to lack of general system implementation	
* Memory: CPU and PPU can reasonably read and write to memory, some edge cases implemented, all memory except that explicitly not usable (as defined in the Pandocs) is readable and writable. Cartriges can be loaded (ROM ONLY, NO MAPPERS), boot ROM boots.
  - Cartridge mapping: ROM only right now, no mappers implemented.
* Video: PPU renders backgrounds.
* Audio: No progress yet, it may not even be implemented at all.
* Input: Not available yet, as at least CPU, Memory, and Video have to be somewhat implemented.
* General: Barebones console program, loads ROM-only carts opens an SFML window with a white background. I may implement a basic UI later.
