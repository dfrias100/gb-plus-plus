# gb-plus-plus
Gameboy Emulator in C++ with SFML 2.5

This is gameboy emulator that is a work in progress! Many parts have not yet been implemented. Here's the current progress as of commit [d46fda0](https://github.com/dfrias100/gb-plus-plus/commit/d46fda06315394b7caa056634b4c194e21718b2a):
* CPU: 500 of 500 instructions implemented (100.0%). All instructions and interrupt handling have been implemented. Timer emulation is also implemented, LCD updating is implemented.
	- 260 of 260 Prefix 0xCB shift, rotate, bit instructions (100.0%)
	- 92 of 92 8-bit ALU instructions (100.0%)
	- 85 of 85 8-bit load instructions (100.0%)
	- 30 of 30 Jump / Call instructions (100.0%)
	- 15 of 15 16-bit load instructions (100.0%)
	- 13 of 13 16-bit ALU instructions (100.0%)
	-  5 of 5 Miscellaneous instructions (100.0%)
* Memory: CPU and PPU can reasonably read and write to memory, some edge cases implemented, all memory except that explicitly not usable (as defined in the Pandocs) is readable and writable. Some roms can be loaded, see below.
  - Cartridge mapping implemented
  	* MBC0 (ROM only)
  	* MBC1 (No RAM)
* Video: PPU fully implemented, timing is not proper, but this emulator is not focused on extreme accuracy. Only B/W palette supported, may add ability to add a custom monochrome palette later.
* Audio: No progress yet, it may not even be implemented at all.
* Input: Keyboard support has been implemented, games can now be played. Inputs are handled through a dedicated function.
* General: Main has some form of structure: loads a rom, and the DMG bootrom, then starts the emulation loop.
