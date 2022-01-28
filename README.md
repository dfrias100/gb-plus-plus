# gb-plus-plus
Gameboy Emulator in C++ with SFML 2.5

This is gameboy emulator that is a work in progress! Many parts have not yet been implemented. Here's the current progress as of commit [5577f80](https://github.com/dfrias100/gb-plus-plus/commit/5577f8049a566ba7b7da4d2ae3d53c82b7f5e862):
* CPU: All instructions and interrupt handling have been implemented. Timer emulation is also implemented, LCD updating is implemented.
* Memory: CPU and PPU can reasonably read and write to memory, some edge cases implemented, all memory except that explicitly not usable (as defined in the Pandocs) is readable and writable. Some roms can be loaded, see below.
  - Cartridge mapping implemented
  	* MBC0 (ROM only)
  	* MBC1 (32KB> ROM and RAM)
* Video: PPU fully implemented with basic and essential function, timing is not proper, but this emulator is not focused on extreme accuracy. Only B/W palette supported, may add ability to add a custom monochrome palette later.
* Audio: No progress yet, it may not even be implemented at all.
* Input: Keyboard support has been implemented, games can now be played. Inputs are handled through a dedicated function.
* General: Main has some form of structure: loads a rom, and the DMG bootrom, then starts the emulation loop.
