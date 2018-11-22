diff -ru linux.orig/drivers/block/genhd.h linux/drivers/block/genhd.h
--- linux.orig/include/linux/genhd.h	Mon Apr 19 09:45:13 1999
+++ linux/include/linux/genhd.h	Mon Apr 19 09:45:30 1999
@@ -22,9 +22,7 @@
 #define CONFIG_SUN_PARTITION 1
 #endif
 
-#if defined(CONFIG_SGI)
 #define CONFIG_SGI_PARTITION 1
-#endif
 
 /* These three have identical behaviour; use the second one if DOS fdisk gets
    confused about extended/logical partitions starting past cylinder 1023. */
