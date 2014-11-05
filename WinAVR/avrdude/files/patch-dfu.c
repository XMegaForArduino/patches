--- dfu.c.orig	2014-11-04 07:45:55.547851500 -0800
+++ dfu.c	2014-11-04 07:46:12.277343700 -0800
@@ -16,10 +16,11 @@
  * along with this program. If not, see <http://www.gnu.org/licenses/>.
  */
 
 /* $Id: dfu.c 1294 2014-03-12 23:03:18Z joerg_wunsch $ */
 
+#include "windows.h"
 #include "ac_cfg.h"
 #include "dfu.h"
 
 #include "avrdude.h"
 
