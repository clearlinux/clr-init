
TARGET = clr-init.cpio.gz

BINFILES=`cat bin_files`

all: $(TARGET)

%.cpio.gz: %.cpio
	gzip -9 -f $<

clr-init.cpio:
	set -e;
	mkdir -p initramfs/{sys,dev,proc,tmp,var,sysroot,usr/sbin,usr/bin,usr/lib/systemd/system-generators,usr/lib64,usr/lib64/multipath,run,root}
	if [ -d /usr/lib64/haswell/ ]; then \
		mkdir -p initramfs/usr/lib64/haswell;\
	fi
	if [ -d /usr/lib64/haswell/avx512_1 ]; then \
		mkdir -p initramfs/usr/lib64/haswell/avx512_1;\
	fi
	for file in $(BINFILES); do \
		cp -frn $$file initramfs/$$file;\
	done
	for file in $$(ldd $(BINFILES) | awk '{print $$3}' | grep "^/" | sort | uniq); do \
		file_path=$$(dirname $$file); \
		cp -rn $$file initramfs/$$file_path/; \
	done
	if [ -d /usr/lib64/haswell/ ]; then \
		for lib in $$(ls initramfs/usr/lib64/haswell/); do \
			[ $$lib == "avx512_1" ] && continue; \
			cp -rn /usr/lib64/$$lib initramfs/usr/lib64/ ; \
		done \
	fi
	if [ -d /usr/lib64/haswell/avx512_1 ]; then \
		for lib in $$(ls initramfs/usr/lib64/haswell/avx512_1/); do \
			cp -rn /usr/lib64/$$lib initramfs/usr/lib64/ ; \
		done \
	fi
	( cd initramfs && \
		find . -print0 | cpio -o --null --format=newc ) > $@

install: clr-init.cpio.gz
	@mkdir -p $(DESTDIR)/usr/lib/initrd.d/ && \
	cp $< $(DESTDIR)/usr/lib/initrd.d/

clean:
	@for file in $(BINFILES); do \
		rm -f initramfs/$$file; \
	done
	rm -rf initramfs/{sys,dev,proc,tmp,var,sysroot,usr/sbin,usr/bin,usr/lib/systemd/system-generators,usr/lib64,usr/lib64/multipath,run,root}
	rm -f clr-init.cpio $(TARGET)
