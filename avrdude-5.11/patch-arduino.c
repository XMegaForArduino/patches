--- arduino.c.orig	2010-11-02 09:52:16.000000000 -0700
+++ arduino.c	2012-01-02 12:45:52.000000000 -0800
@@ -91,11 +91,11 @@
   }
 
   /* Clear DTR and RTS to unload the RESET capacitor 
    * (for example in Arduino) */
   serial_set_dtr_rts(&pgm->fd, 0);
-  usleep(50*1000);
+  usleep(250*1000); // increased from 50msec to 250msec - BBB
   /* Set DTR and RTS back to high */
   serial_set_dtr_rts(&pgm->fd, 1);
   usleep(50*1000);
 
   /*
