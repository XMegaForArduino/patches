--- config/override.m4.orig	2014-10-31 14:17:52.519531200 -0700
+++ config/override.m4	2014-10-31 14:18:14.214843700 -0700
@@ -27,11 +27,11 @@
 m4_copy_force([_AC_PREREQ], [AC_PREREQ])
 
 
 dnl Ensure exactly this Autoconf version is used
 m4_ifndef([_GCC_AUTOCONF_VERSION],
-  [m4_define([_GCC_AUTOCONF_VERSION], [2.64])])
+  [m4_define([_GCC_AUTOCONF_VERSION], [2.69])])
 
 dnl Test for the exact version when AC_INIT is expanded.
 dnl This allows to update the tree in steps (for testing)
 dnl by putting
 dnl   m4_define([_GCC_AUTOCONF_VERSION], [X.Y])
