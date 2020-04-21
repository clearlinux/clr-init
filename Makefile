
TARGET = clr-init.cpio.gz

BINFILES=`cat bin_files`

all: $(TARGET)

%.cpio.gz: %.cpio
	gzip -9 -f $<

clr-init.cpio:
	set -e;
	mkdir -p initramfs/{sys,dev,proc,tmp,var,sysroot,usr/bin,usr/lib/systemd/system-generators,usr/lib64,usr/lib64/multipath,run,root,usr/lib/udev/rules.d,usr/lib/udev/hwdb.d}
	if [ -d /usr/lib64/haswell/ ]; then \
		mkdir -p initramfs/usr/lib64/haswell;\
	fi
	if [ -d /usr/lib64/haswell/avx512_1 ]; then \
		mkdir -p initramfs/usr/lib64/haswell/avx512_1;\
	fi
	for file in $(BINFILES); do \
		cp -fL $$file initramfs/$$file || exit 1;\
		if file $$file | grep -q ELF; then \
			for lddfile in $$(ldd $$file | awk '{print $$3}' | grep "^/"); do \
				file_path=$$(dirname $$lddfile); \
				cp -Ln $$lddfile initramfs/$$file_path/; \
			done \
		fi \
	done
	if [ -d /usr/lib64/haswell/ ]; then \
		for lib in $$(ls initramfs/usr/lib64/haswell/); do \
			[ $$lib == "avx512_1" ] && continue; \
			cp -L /usr/lib64/$$lib initramfs/usr/lib64/ ; \
		done \
	fi
	if [ -d /usr/lib64/haswell/avx512_1 ]; then \
		for lib in $$(ls initramfs/usr/lib64/haswell/avx512_1/); do \
			cp -L /usr/lib64/$$lib initramfs/usr/lib64/ ; \
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
	rm -rf initramfs/{sys,dev,proc,tmp,var,sysroot,usr/bin,usr/lib/systemd/system-generators,usr/lib64,usr/lib64/multipath,run,root}
	rm -f clr-init.cpio $(TARGET)
