# @(#) Makefile for efs.o

# Location of your current kernel.
KERNEL_TOP=/usr/src/linux

# You may wish to uncomment the following line.
#DEBUG=-DDEBUG

OPT=-O2
DEFS=-D__KERNEL__ -DMODULE -D_EFS_USE_GENERIC $(DEBUG)
COPTS=-Wall -Wstrict-prototypes -fomit-frame-pointer -pipe
IDIRS=-I. -I$(KERNEL_TOP)/include
CFLAGS=$(OPT) $(DEFS) $(COPTS) $(IDIRS)

SRCS=super.c inode.c namei.c dir.c file.c symlink.c
OBJS=super.o inode.o namei.o dir.o file.o symlink.o
INCS=linux/efs_fs.h linux/efs_fs_sb.h linux/efs_vh.h linux/efs_dir.h linux/efs_fs_i.h

.c.o:
	$(CC) $(CFLAGS) -c $<

all: efs.o

efs.o: $(OBJS)
	$(LD) -r -o efs.o $(OBJS)

$(OBJS): $(INCS)

clean:
	-rm -f *.o

genhd: $(KERNEL_TOP) patch.genhd.c patch.genhd.h
	patch -p1 -d $(KERNEL_TOP) -N < patch.genhd.h
	@echo
	@echo This patch is only for kernels before 2.2.5.
	@echo It should fail for 2.2.6 and later.
	@echo
	-patch -p1 -d $(KERNEL_TOP) -N < patch.genhd.c

kernel: $(KERNEL_TOP) $(SRCS) $(INCS) Makefile.kernel patch.linux-2.2 patch.genhd.c
	@echo Applying patches to $(KERNEL_TOP)...
	-mkdir $(KERNEL_TOP)/fs/efs
	cp $(SRCS) $(KERNEL_TOP)/fs/efs
	cp Makefile.kernel $(KERNEL_TOP)/fs/efs/Makefile
	cp $(INCS) $(KERNEL_TOP)/include/linux
	patch -p1 -d $(KERNEL_TOP) < patch.linux-2.2
	@echo
	@echo This patch is only for kernels before 2.2.5.
	@echo It should fail for 2.2.6 and later.
	@echo
	-patch -p1 -d $(KERNEL_TOP) -N < patch.genhd.c
	@echo
	@echo Enable experimental code to see the EFS option in "make config"
	@echo

