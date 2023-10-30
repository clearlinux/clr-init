
TARGET = clr-init.cpio.gz

BINFILES=`cat bin_files`

all: $(TARGET)

%.cpio.gz: %.cpio
	gzip -9 -f $<

clr-init.cpio:
	set -e;
	mkdir -p initramfs/{sys,dev,proc,tmp,var,sysroot,usr/bin,usr/lib/systemd/system-generators,usr/lib64,usr/lib64/multipath,run,root,usr/lib/udev/rules.d,usr/lib/udev/hwdb.d}
	for file in $(BINFILES); do \
		mkdir -p initramfs/$$(dirname "$${file}"); \
		cp -fL $$file initramfs/$$file || exit 1;\
		if file $$file | grep -q ELF; then \
			for lddfile in $$(ldd $$file | awk '{print $$3}' | grep "^/"); do \
				base_lib=$$(sed 's/glibc-hwcaps\/x86-64-v[34]\///' <<< $${lddfile}); \
				file_path=$$(dirname $${base_lib}); \
				mkdir -p initramfs/$${file_path}; \
				echo cp -Lu $$base_lib initramfs/$$file_path/; \
				cp -Lu $$base_lib initramfs/$$file_path/; \
			done \
		fi \
	done
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
