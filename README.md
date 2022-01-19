# gb-plus-plus
Gameboy Emulator in C++ with SFML 2.5

This is gameboy emulator that is a work in progress! Many parts have not yet been implemented. Here's the current progress as of commit [ad1cea6b](https://github.com/dfrias100/gb-plus-plus/commit/ad1cea6b5652fec5290ae3de13bd1135b4247ac3):
* CPU: 324 of 500 instructions implemented (64.8%)
	- 84 of 260 Prefix 0xCB shift, rotate, bit instructions (32.3%)
	- 92 of 92 8-bit ALU instructions (100.0%)
	- 85 of 85 8-bit load instructions (100.0%)
	- 30 of 30 Jump / Call instructions (100.0%)
	- 15 of 15 16-bit load instructions (100.0%)
	- 13 of 13 16-bit ALU instructions (100.0%)
	-  5 of 5 Miscellaneous instructions (100.0%)*
		- 2 of these instructions are partially implemented due to lack of general system implementation
* Memory: Framework established but no components can write to any memory yet. CPU can be ticked.
  - Cartridge mapping: ROM only right now, no mappers implemented.
* Video: No progress yet.
* Audio: No progress yet, it may not even be implemented at all.
* Input: Not available yet, as at least CPU, Memory, and Video have to be somewhat implemented.
* General: Barebones console program, only opens an SFML window with a white background. I may implement a basic UI later.
