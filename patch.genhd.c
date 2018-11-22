diff -ru linux.orig/drivers/block/genhd.c linux/drivers/block/genhd.c
--- linux.orig/drivers/block/genhd.c	Sat Jan  9 06:54:17 1999
+++ linux/drivers/block/genhd.c	Sun Apr 18 17:08:37 1999
@@ -762,11 +762,14 @@
 #endif /* CONFIG_SUN_PARTITION */
 
 #ifdef CONFIG_SGI_PARTITION
+#include <asm/byteorder.h>
 
 static int sgi_partition(struct gendisk *hd, kdev_t dev, unsigned long first_sector)
 {
 	int i, csum;
 	unsigned int *ui;
+	unsigned int start, blocks, cs;
+	int magic;
 	struct buffer_head *bh;
 	struct sgi_disklabel {
 		int magic_mushroom;         /* Big fat spliff... */
@@ -796,15 +799,18 @@
 	}
 	label = (struct sgi_disklabel *) bh->b_data;
 	p = &label->partitions[0];
-	if(label->magic_mushroom != SGI_LABEL_MAGIC) {
+	magic = label->magic_mushroom;
+	if(be32_to_cpu(magic) != SGI_LABEL_MAGIC) {
 		printk("Dev %s SGI disklabel: bad magic %08x\n",
-		       kdevname(dev), label->magic_mushroom);
+		       kdevname(dev), magic);
 		brelse(bh);
 		return 0;
 	}
 	ui = ((unsigned int *) (label + 1)) - 1;
-	for(csum = 0; ui >= ((unsigned int *) label);)
-		csum += *ui--;
+	for(csum = 0; ui >= ((unsigned int *) label);) {
+		cs = *ui--;
+		csum += be32_to_cpu(cs);
+	}
 	if(csum) {
 		printk("Dev %s SGI disklabel: csum bad, label corrupted\n",
 		       kdevname(dev));
@@ -817,9 +823,11 @@
 	 * current_minor.
 	 */
 	for(i = 0; i < 16; i++, p++) {
-		if(!(p->num_blocks))
+		blocks = be32_to_cpu(p->num_blocks);
+		start  = be32_to_cpu(p->first_block);
+		if(!blocks)
 			continue;
-		add_partition(hd, current_minor, p->first_block, p->num_blocks);
+		add_partition(hd, current_minor, start, blocks);
 		current_minor++;
 	}
 	printk("\n");
