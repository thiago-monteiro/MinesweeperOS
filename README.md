# MinesweeperOS

### Why??

My incredible code shall only grace the CPU if it's run completely dependency-free and entirely natively. For this I have created an INCREDIBLE and REVOLUTIONARY operating system that runs a very bad version of Minesweeper. 

##### (The real reason I made this)
I created this to learn how computers REALLY work at a low-level. In this, I use a mixture of C and x86 assembly to create a full on OS (technically more of a kernel) that only runs Minesweeper. 

### The (excellent) tutorials I used to actually learn this stuff
* https://dev.to/frosnerd/writing-my-own-boot-loader-3mld
* https://www.youtube.com/playlist?list=PLFjM7v6KGMpiH2G-kT781ByCNC_0pKpPN

### How to run this
I've done this in WSL (Windows Subsystem for Linux). This means that you will either need to install that if you're on Windows or run Linux natively. I haven't tested it on Macs although they are Unix-based so it can probably be done natively through Homebrew. 

##### Once you've done that, install: 
* Unix ELF binary tools: https://github.com/lordmilko/i686-elf-tools#linux
* QEMU: https://www.qemu.org/download/#linux
* build-essential package for your Linux distro (Ex. `apt-get install build essential`)

##### After installing those packages, clone this repo and type:

    make run
