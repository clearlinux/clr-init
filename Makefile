BINFILES=`cat bin_files`

all:
	mkdir -p initramfs/{sys,dev,proc,tmp,var,sysroot,usr/sbin,usr/bin,usr/lib/systemd/system-generators,usr/lib64,usr/lib64/multipath,run,root} && \
    if [ -d /usr/lib64/haswell/ ]; then \
        mkdir -p initramfs/usr/lib64/haswell;\
    fi &&\
    for file in $(BINFILES); \
    do \
        cp -f $$file initramfs/$$file;\
    done && \
    for file in $$(ldd $(BINFILES) | awk '{print $$3}' | grep "^/" | sort | uniq); \
    do \
        file_path=$$(dirname $$file); \
        cp $$file initramfs/$$file_path/; \
    done && \
    if [ -d /usr/lib64/haswell/ ]; then \
        for lib in $$(ls initramfs/usr/lib64/haswell/); \
        do \
            cp /usr/lib64/$$lib initramfs/usr/lib64/ ; \
        done \
    fi &&\
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
    rm -rf initramfs/{sys,dev,proc,tmp,var,sysroot,usr/sbin,usr/bin,usr/lib/systemd/system-generators,usr/lib64,usr/lib64/multipath,run,root} && \
    rm clr-init.cpio.gz
