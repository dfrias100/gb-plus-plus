# gb-plus-plus
Gameboy Emulator in C++ with SFML 2.5

This is gameboy emulator that is a work in progress! Many parts have not yet been implemented. Here's the current progress as of commit [c52dc53](https://github.com/dfrias100/gb-plus-plus/commit/c52dc53b5ddbe8d10dc6657e1ad2520e71b2da95):
* CPU: 14 of 501 instructions implemented (2.7%)
* Memory: Framework established but no components can write to any memory yet.
  - Cartridge mapping: ROM only right now, no mappers implemnted.
* Video: No progress yet.
* Audio: No progress yet, it may not even be implemented at all.
* Input: Not available yet, as at least CPU, Memory, and Video have to be somewhat implemented.
* General: Barebones console program, only opens an SFML window with a white background. I may implement a basic UI later.
