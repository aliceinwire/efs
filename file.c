/*
 * file.c
 *
 * Copyright (c) 1999 Al Smith
 *
 * Portions derived from work (c) 1995,1996 Christian Vogelgsang.
 */

#include <linux/efs_fs.h>

static struct file_operations efs_file_operations = {
	NULL,			/* lseek */
	generic_file_read,	/* read */
	NULL,			/* write */
	NULL,			/* readdir */
	NULL,			/* poll */
	NULL,			/* ioctl */
	generic_file_mmap,	/* mmap */
	NULL,			/* open */
	NULL,			/* flush */
	NULL,			/* release */
	NULL,			/* fsync */
	NULL,			/* fasync */
	NULL,			/* check_media_change */
	NULL			/* revalidate */
};

struct inode_operations efs_file_inode_operations = {
	&efs_file_operations,	/* default file operations */
	NULL,			/* create */
	NULL,			/* lookup */
	NULL,			/* link */
	NULL,			/* unlink */
	NULL,			/* symlink */
	NULL,			/* mkdir */
	NULL,			/* rmdir */
	NULL,			/* mknod */
	NULL,			/* rename */
	NULL,			/* readlink */
	NULL,			/* follow_link */
	generic_readpage,	/* readpage */
	NULL,			/* writepage */
	efs_bmap,		/* bmap */
	NULL,			/* truncate */
	NULL,			/* permission */
	NULL			/* smap */
};
 
int efs_bmap(struct inode *inode, efs_block_t block) {
	int result;

	if (block < 0) {
		printk(KERN_WARNING "EFS: bmap(): block < 0\n");
		return 0;
	}

	/* are we about to read past the end of a file ? */
	if (block >= inode->i_blocks) {
		/*
		 * linux likes to read 8 blocks at a time.
		 */
		if (block >= inode->i_blocks + 8) {
			printk(KERN_WARNING
				"EFS: bmap(): block %d >= %ld (filesize %ld)\n",
				block,
				inode->i_blocks,
				inode->i_size);
		}
		return 0;
	}

	result = efs_map_block(inode, block);

/*
 * BUG: for some reason that I really don't understand, a file on an
 * efs CD will sometimes contain a small number of blocks that are
 * simply wrong. they're not garbage, they appear to be coming from
 * files that are also in the filesystem cache. the efs_map_block()
 * code is correct, and it's irrelevant whether the file consists of
 * direct or indirect extents. inserting artificial delays into
 * efs_map_block() also produces no improvement, so the conclusion
 * could be drawn that there is no race condition here.
 *
 * however, dd if=/dev/cdrom of=/tmp/image results in a correct image,
 * and mounting that image through losetup /dev/loop0 /tmp/image
 * isn't problematic.
 *
 * if HORRIBLE_KLUDGE is defined, efs will actually read the block
 * itself before returning the physical location on the disk. as
 * described, this is a horrible kludge, however it works as designed.
 * this bug should really be fixed somewhere higher up the VFS layers,
 * but unfortunately i have no idea where to start looking.
 *
 * Al Smith - September 23 1999.
 */
#define HORRIBLE_KLUDGE

#ifdef HORRIBLE_KLUDGE
	{ struct buffer_head * bh;
		bh = bread(inode->i_dev, result, EFS_BLOCKSIZE);
		if (bh) brelse(bh);
	}
#endif

	return(result);
}

