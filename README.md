patches
=======

patches for compilers and tools such as gcc, binutils, libc

These patches are subject to the same license as the programs for which
they are intended.  In the case of the gcc suite (including binutils and
libc) it is GPLv3 with the 'special exception' clause.


This software is being provided 'as-is' with no warranty, implied or otherwise

In some cases the patches may work with different versions of the software than
original specified; however, it is not recommended without significant testing.

If a feature has been implemented in a newer version of the software, please
use the newer version rather than relying on the patch.

NOTE:  the patches were tested on FreeBSD using its 'ports' system.  As a
result, other packaging systems (such as Debian) may require minor changes or
a change in the name for it to work properly.

Additionally, the FreeBSD gcc 4.5.1 patch contains startup code modifications
that have been patched upstream by the gcc developers in later versions.  You
can edit the patch files to remove this if you do not need it.  Debian and
Ubuntu already have their own fix for the same bug.  The affected file is
'gcc/config/avr/libgcc.S' (it is a C++ startup bug, unpreserved regs).

