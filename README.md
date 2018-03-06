# Introduction to Video Game Hacking

### The Target
We will be focusing on the Windows Demo version of Hack Slash Loot. Keep in mind that values may change when targeting the full version of the game. The majority of the code will work just fine, however you may need to scan for new values using Cheat Engine or your memory scanner of choice.

### The Tools
Will will use Cheat Engine to locate values in memory and assist with reverse engineering. We will be using Microsoft Visual Studio 2015 Community Edition (free) as our IDE and compiler, but feel free to use Mingw, Clang/LLVM, or whatever you're into.

### The Language
We'll be using straight C. C is an exceptional language to start with as we have a lot of control over memory, the ability to easily in-line assembly, access the Windows API, and keep our trainer size extremely small without requiring specific runtimes or third party libraries on the user's system.

### The API
Since we'll be hacking a Windows game, we will be relying heavily on the Windows API to make a lot of the heavy lifting easy on us. Things like memory access, memory protections, process identification, etc. will be handled through the Windows API. Other than that, we will not rely on any third-party libraries for this series.
