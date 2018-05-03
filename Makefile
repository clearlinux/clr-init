BINFILES=`cat bin_files`

all:
	@mkdir -p initramfs/{sys,dev,proc,tmp,var,sysroot,usr/bin,usr/lib/systemd/system-generators,usr/lib64,run,root} && \
    for file in $(BINFILES); \
    do \
        cp -f $$file initramfs/$$file;\
    done && \
    cp $$(ldd $(BINFILES) | awk '{print $$3}' | grep "^/" | sort | uniq) initramfs/usr/lib64/ && \
    cd initramfs && \
    find . -print0 | cpio -o --null --format=newc | gzip -9 > ../clr-init.cpio.gz && \
    cd ..

install: clr-init.cpio.gz
	@mkdir -p $(DESTDIR)/usr/lib/initrd.d/ && \
    cp $< $(DESTDIR)/usr/lib/initrd.d/

clean:
	@for file in $(BINFILES); \
    do \
		rm -f initramfs/$$file; \
	done && \
    rm -rf initramfs/{sys,dev,proc,tmp,var,sysroot,usr/bin,usr/lib/systemd/system-generators,usr/lib64,run,root} && \
    rm clr-init.cpio.gz
