--- stk500v2.c.orig	2014-03-12 16:03:19.000000000 -0700
+++ stk500v2.c	2014-10-29 13:36:46.000000000 -0700
@@ -487,21 +487,31 @@
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
   else if (PDATA(pgm)->pgmtype == PGMTYPE_JTAGICE3)
     return stk500v2_jtag3_send(pgm, data, len);
 
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
@@ -516,13 +526,18 @@
   for (i=0;i<len+6;i++) DEBUG("0x%02x ",buf[i]);
   DEBUG(", %d)\n",len+6);
 
   if (serial_send(&pgm->fd, buf, len+6) != 0) {
     fprintf(stderr,"%s: stk500_send(): failed to send command to serial port\n",progname);
+
+    free(buf); // BBB - clean up my malloc'd buffer
+
     exit(1);
   }
 
+  free(buf); // BBB - clean up my malloc'd buffer
+
   return 0;
 }
 
 
 int stk500v2_drain(PROGRAMMER * pgm, int display)
@@ -2269,11 +2284,12 @@
                                 unsigned int addr, unsigned int n_bytes)
 {
   unsigned int block_size, last_addr, addrshift, use_ext_addr;
   unsigned int maxaddr = addr + n_bytes;
   unsigned char commandbuf[10];
-  unsigned char buf[266];
+//  unsigned char buf[266]; BBB - change out for malloc'd buffer
+  unsigned char *buf;
   unsigned char cmds[4];
   int result;
   OPCODE * rop, * wop;
 
   DEBUG("STK500V2: stk500v2_paged_write(..,%s,%u,%u,%u)\n",
@@ -2365,24 +2381,38 @@
     else
       block_size = page_size;
 
     DEBUG("block_size at addr %d is %d\n",addr,block_size);
 
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
@@ -2510,11 +2540,12 @@
                                unsigned int addr, unsigned int n_bytes)
 {
   unsigned int block_size, hiaddr, addrshift, use_ext_addr;
   unsigned int maxaddr = addr + n_bytes;
   unsigned char commandbuf[4];
-  unsigned char buf[275];	// max buffer size for stk500v2 at this point
+//  unsigned char buf[275];	// max buffer size for stk500v2 at this point (BBB - not big enough)
+  unsigned char *buf;
   unsigned char cmds[4];
   int result;
   OPCODE * rop;
 
   DEBUG("STK500V2: stk500v2_paged_load(..,%s,%u,%u,%u)\n",
@@ -2554,10 +2585,19 @@
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
   for (; addr < maxaddr; addr += page_size) {
     if ((maxaddr - addr) < page_size)
       block_size = maxaddr - addr;
     else
       block_size = page_size;
@@ -2571,30 +2611,38 @@
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
 
+  free(buf); // BBB - now I can free it as I'm done with it
+
   return n_bytes;
 }
 
 
 /*
