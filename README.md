# Cost
C OS Template

Cost is a simple template for quickly getting started with making an OS in C/C++ using limine on x86\_64

Big thank you to:
- [@Tsoding](https://github.com/tsoding) - for the amazing [nob.h library](https://github.com/tsoding/musializer)
- [@lordmilko](https://github.com/lordmilko) - for the [precompiled binaries](https://github.com/lordmilko/i686-elf-tools) for gcc and binutils

# Getting Started

Requirements:
- xorriso - for creating iso files.
- (Optional) wget - for downloading cross compiler

**NOTE:** If you already have a cross-compiler, are on MacOS or are on Linux x86\_64, please skip to [Using preinstalled compiler](#using-preinstalled-compiler)

To get started, we first need to bootstrap the config.h and build system itself. To do that run:

```sh
gcc bootstrap.c -o bootstrap
./bootstrap         # Just bootstrap on windows (without ./)
```
This should download pre-compiled binaries for x86\_64-elf-gcc+ld ([Why download gcc cross compiler](#why-download-gcc-cross-compiler) if you're interested).

This should also generate two files:
- build(.exe on windows) - our [build system](#the-build-system)
- config.h - the configuration for our kernel and build system

The build system uses a subcommand type of structure so that it can be used for more than just building,
but also running a VM and doing anything you really want it to do (More in-depth [here](#the-build-system)).

By default calling build should compile the kernel and make it into an iso using xorriso.
If you want to get the full list of commands run:
```
build help
```
One of the most common commands I use is
```
build bruh                           # build AND runs the OS using qemu
```

**NOTE:** Running qemu from the build system should create a file called kernel\_logs.txt which is just the things we write to serial0

This template comes with a little demo which prints a Welcome message to serial0 (explore the code :D)

The kernel source code is located under the ./kernel/src folder and you can find the compiled object files and iso under ./bin.

I usually put my dependencies under ./kernel/vendor/ (where limine is also located)

The build system source code (one file, but you can expand it however you like!) is located under ./buildsys

And boom. You have your own environment for making your OS.

If you want you can also try and complete [this mini-checklist](./CHECKLIST.md) of some things you usually do in OS development :D.

Good luck!


## Using preinstalled compiler

If you want to use a preinstalled compiler you'll need to pass the -GCC="<path to gcc>" and -LD="<path to ld>" flags to bootstrap (rest of the steps are as usual)
If you already have a config.h generated you could either pass -fconfig to bootstrap or edit the GCC and LD macros inside of config.h


**MacOS users**
If you're on MacOS you'll have to compile binutils and gcc yourself:
- [Osdev wiki tutorial](https://wiki.osdev.org/GCC_Cross-Compiler)

Whilst it does usually work, feel free to reach out if you encounter any errors.


# Bootstrapper
## Why download gcc cross compiler?
This template is supposed to be for all people and from my experience I've noticed a big amount of
'gatekeeping' when it comes to Osdev especially towards Windows and MacOS users
as Osdev usually uses a lot of linux specific things and building a cross compiler on Windows specifically has been a pain for many years.
That however can be fixed by showing people how they can use precompiled binaries to substitute the need for building gcc themselves (sorry MacOS users, I'm trying to see if I can host precompiled binaries for MacOS myself).

Here however I need to note that, if you're on windows and want to use more linux related utilities instead of using a cross compiler,
you should consider using something like WSL or docker (wsl usually fits people best).
In which case try running bootstrapper again, but follow [Using preinstalled compiler](#using-preinstalled-compiler)

## What is a cross compiler?
If you don't understand what a cross compiler is, consider checking out the [Osdev wiki](https://wiki.osdev.org/GCC_Cross-Compiler)
If you already have a compiler that can output to elf files and don't want to download binutils and gcc, checkout the [Using preinstalled compiler](#using-preinstalled-compiler) section


# The build system
The build system is located under ./buildsys 
Whilst it does try to be simple, it does some things that are a bit complicated like incremental builds. If you choose so, you can remove them. 

The build system uses a subcommand structure (AKA, build + subcommand does something).
The subcommand structure is rather simple, you define your Commands (name + function to run + description) inside the commands array.
A command accepts the "Build" structure, which stores the argc, argv, and path to executable of the build system. 
Each part of the build system is split into different functions (run for running, build_kernel for building the kernel, link_kernel for linking the kernel etc. etc).
It generally follows the standard that nob.h has, where each function returns a boolean indicating its success (true => OK, false => ERROR).

**NOTE:** Linking happens with a linker script, because of some specific requirements limine has for the kernel (Checkout [limine barebones](https://wiki.osdev.org/Limine_Bare_Bones) on Osdev wiki)
