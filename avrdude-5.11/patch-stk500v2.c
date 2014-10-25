--- stk500v2.c.orig	2011-08-26 13:22:10.000000000 -0700
+++ stk500v2.c	2014-10-24 20:11:23.000000000 -0700
@@ -416,19 +416,29 @@
   return rv;
 }
 
 static int stk500v2_send(PROGRAMMER * pgm, unsigned char * data, size_t len)
 {
-  unsigned char buf[275 + 6];		// max MESSAGE_BODY of 275 bytes, 6 bytes overhead
+// removed by BBB - seems that in some cases it can really be longer - there are 2 length bytes after all...
+//  unsigned char buf[275 + 6];		// max MESSAGE_BODY of 275 bytes, 6 bytes overhead
+  unsigned char *buf;
   int i;
 
   if (PDATA(pgm)->pgmtype == PGMTYPE_AVRISP_MKII ||
       PDATA(pgm)->pgmtype == PGMTYPE_STK600)
     return stk500v2_send_mk2(pgm, data, len);
   else if (PDATA(pgm)->pgmtype == PGMTYPE_JTAGICE_MKII)
     return stk500v2_jtagmkII_send(pgm, data, len);
 
+  // added by BBB - variable length buffer using malloc - why not before?
+  buf = malloc(8 + len);
+  if(!buf) // should not fail, machines have tons of memory these days
+  {
+    fprintf(stderr,"%s: stk500_send(): not enough memory for buffer\n",progname);
+    exit(1);
+  }
+
   buf[0] = MESSAGE_START;
   buf[1] = PDATA(pgm)->command_sequence;
   buf[2] = len / 256;
   buf[3] = len % 256;
   buf[4] = TOKEN;
@@ -446,10 +456,12 @@
   if (serial_send(&pgm->fd, buf, len+6) != 0) {
     fprintf(stderr,"%s: stk500_send(): failed to send command to serial port\n",progname);
     exit(1);
   }
 
+  free(buf); // BBB - clean up my malloc'd buffer
+
   return 0;
 }
 
 
 int stk500v2_drain(PROGRAMMER * pgm, int display)
@@ -1709,11 +1721,12 @@
 static int stk500v2_paged_write(PROGRAMMER * pgm, AVRPART * p, AVRMEM * m, 
                               int page_size, int n_bytes)
 {
   unsigned int addr, block_size, last_addr, hiaddr, addrshift, use_ext_addr;
   unsigned char commandbuf[10];
-  unsigned char buf[266];
+//  unsigned char buf[266]; BBB - change out for malloc'd buffer
+  unsigned char *buf;
   unsigned char cmds[4];
   int result;
   OPCODE * rop, * wop;
 
   DEBUG("STK500V2: stk500v2_paged_write(..,%s,%d,%d)\n",m->desc,page_size,n_bytes);
@@ -1813,24 +1826,38 @@
       if (stk500v2_is_page_empty(addr, block_size, m->buf)) {
           continue;
       }
     }
 
+    buf = malloc(sizeof(commandbuf) + block_size + 256); // BBB - added malloc and supporting code - original size 266, added 256 to make minimum similar
+    if(!buf)
+    {
+      fprintf(stderr,
+              "%s: stk500v2_paged_write: not enough memory for buffer\n",
+              progname);
+      return -1;
+    }
+
     memcpy(buf,commandbuf,sizeof(commandbuf));
 
     buf[1] = block_size >> 8;
     buf[2] = block_size & 0xff;
 
     if((last_addr==UINT_MAX)||(last_addr+block_size != addr)){
       if (stk500v2_loadaddr(pgm, use_ext_addr | (addr >> addrshift)) < 0)
+      {
+        free(buf); // BBB - must free buffer
         return -1;
     }
+    }
     last_addr=addr;
 
     memcpy(buf+10,m->buf+addr, block_size);
 
-    result = stk500v2_command(pgm,buf,block_size+10, sizeof(buf));
+    result = stk500v2_command(pgm,buf,block_size+10, /*sizeof(buf)*/sizeof(commandbuf) + block_size + 256);
+    free(buf); // buf no longer needed
+
     if (result < 0) {
       fprintf(stderr,
               "%s: stk500v2_paged_write: write command failed\n",
               progname);
       return -1;
@@ -1975,11 +2002,12 @@
 static int stk500v2_paged_load(PROGRAMMER * pgm, AVRPART * p, AVRMEM * m,
                              int page_size, int n_bytes)
 {
   unsigned int addr, block_size, hiaddr, addrshift, use_ext_addr;
   unsigned char commandbuf[4];
-  unsigned char buf[275];	// max buffer size for stk500v2 at this point
+//  unsigned char buf[275];	// max buffer size for stk500v2 at this point (BBB - not big enough)
+  unsigned char *buf;
   unsigned char cmds[4];
   int result;
   OPCODE * rop;
 
   DEBUG("STK500V2: stk500v2_paged_load(..,%s,%d,%d)\n",m->desc,page_size,n_bytes);
@@ -2018,10 +2046,19 @@
     return -1;
   }
   avr_set_bits(rop, cmds);
   commandbuf[3] = cmds[0];
 
+  buf = malloc(sizeof(commandbuf) + page_size + 266); // BBB - malloc'd buffer with min size similar to before
+  if(!buf)
+  {
+      fprintf(stderr,
+              "%s: stk500v2_paged_load: not enough memory for buffer\n",
+              progname);
+      return -1;
+  }
+
   for (addr=0; addr < n_bytes; addr += page_size) {
     report_progress(addr, n_bytes,NULL);
 
     if ((n_bytes-addr) < page_size)
       block_size = n_bytes - addr;
@@ -2037,30 +2075,38 @@
     // Ensure a new "load extended address" will be issued
     // when crossing a 64 KB boundary in flash.
     if (hiaddr != (addr & ~0xFFFF)) {
       hiaddr = addr & ~0xFFFF;
       if (stk500v2_loadaddr(pgm, use_ext_addr | (addr >> addrshift)) < 0)
+      {
+        free(buf); // BBB - must free buffer
         return -1;
     }
+    }
 
-    result = stk500v2_command(pgm,buf,4,sizeof(buf));
+    result = stk500v2_command(pgm,buf,4,/*sizeof(buf)*/sizeof(commandbuf) + page_size + 266); // BBB - correct buffer size specified
     if (result < 0) {
       fprintf(stderr,
               "%s: stk500v2_paged_load: read command failed\n",
               progname);
+      {
+        free(buf); // BBB - must free buffer
       return -1;
     }
+    }
 #if 0
     for (i=0;i<page_size;i++) {
       fprintf(stderr,"%02X",buf[2+i]);
       if (i%16 == 15) fprintf(stderr,"\n");
     }
 #endif
 
     memcpy(&m->buf[addr], &buf[2], block_size);
   }
 
+  free(buf); // now I can free it as I'm done with it
+
   return n_bytes;
 }
 
 
 /*
