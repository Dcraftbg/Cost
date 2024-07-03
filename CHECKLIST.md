# Mini Checklist
This is a mini checklist of things you might do in Osdev! Note that a few of the first ones are almost completely necessary (memory allocator, GDT, IDT, TSS)

Please also checkout the [limine protocol](https://github.com/limine-bootloader/limine/blob/v7.x/PROTOCOL.md) as it has some interesting things it provides like Framebuffer for drawing stuff on screen, Boot information, kernel info (where the kernel is and its file structure)

I link to Osdev Wiki which just describes some of the things mentioned here, but doesn't solve them for you (you figure it out :D)
- [ ] - Get printf to serial0 working using [stb_sprintf](https://github.com/nothings/stb/blob/master/stb_sprintf.h) (You'll need to do a bit of tweaking and some definitions to disable floats (SSE2 isn't enabled when the computer starts))
- [ ] - A memory allocator
- [ ] - IDT - [Osdev Wiki](https://wiki.osdev.org/Interrupt_Descriptor_Table)
- [ ] - GDT - [Osdev Wiki](https://wiki.osdev.org/Global_Descriptor_Table)
- [ ] - TSS - [Osdev Wiki](https://wiki.osdev.org/Task_State_Segment)
- [ ] - PIC (simpler) or APIC - [PIC Osdev Wiki](https://wiki.osdev.org/8259_PIC) [APIC Osdev Wiki](https://wiki.osdev.org/APIC)
- [ ] - PS2 keyboard and mouse drivers - [PS2 Keyboard Osdev Wiki](https://wiki.osdev.org/PS/2_Keyboard) [PS2 Mouse Osdev Wiki](https://wiki.osdev.org/PS/2_Mouse)
- [ ] - RTC clock driver (for date and time)
- [ ] - Serial drivers for serial consoles
- [ ] - Play around with PCIe devices - [Osdev Wiki](https://wiki.osdev.org/PCI)
- [ ] - Paging - [Osdev Wiki](https://wiki.osdev.org/Paging)
- [ ] - Task Switch - (Simple task switch using PIT for example (0 bit on PIC; 0x20 interrupt))
- [ ] - Task Scheduler - [Osdev Wiki](https://wiki.osdev.org/Processes_and_Threads)
- [ ] - Load User Tasks
- [ ] - Syscalls - (either using int 0x80 or syscall)
- [ ] - Virtual File System (VFS) - [Osdev Wiki](https://wiki.osdev.org/VFS)
- [ ] - Linking and DLLs
- [ ] - Driver/Device Tree and creating drivers
- [ ] - SMP and multiple processors - [Osdev Wiki](https://wiki.osdev.org/Symmetric_Multiprocessing)
- [ ] - NVMe driver or ATA driver for reading/writing from/to disk - [NVMe Osdev Wiki](https://wiki.osdev.org/NVMe) [ATA Osdev Wiki](https://wiki.osdev.org/ATA_PIO_Mode)
- [ ] - File system drivers - FAT32 for example
- [ ] - Sound driver - [Osdev Wiki](https://wiki.osdev.org/Sound)
- [ ] - Shutdown using ACPI - [Osdev Wiki](https://wiki.osdev.org/Shutdown)
- [ ] - Networking stack
- [ ] - Using Time protocol to sync the RTC clock - [Wikipedia](https://en.wikipedia.org/wiki/Time_Protocol)
- [ ] - USB driver
- [ ] - A desktop Environment
- [ ] - Clipboard :)
- [ ] - File explorer
- [ ] - FTP file sharing
- [ ] - Task scheduling (Running tasks on specific times or events) 
- [ ] - Zip/Unzip library 
- [ ] - 3D demos
- [ ] - Porting an Assembler
- [ ] - Porting a C compiler - [Tiny C Compiler](https://repo.or.cz/w/tinycc.git) 
- [ ] - Porting libc
- [ ] - Integrated graphics card driver (for laptops for example) - [Osdev Wiki](https://wiki.osdev.org/Accelerated_Graphic_Cards)
- [ ] - Mini Browser
- [ ] - Porting gcc 
- [ ] - Building the OS from within the OS
- [ ] - Porting Busybox
- [ ] - Porting qemu or bochs
- [ ] - Subset of git
- [ ] - Porting to other architectures
