#!/usr/bin/bash

INITRD_FILE=$1
mkdir -p ${MESON_BUILD_ROOT}/initramfs/{proc,dev,sys,root,usr/lib64,lib64} && \
cp ${MESON_BUILD_ROOT}/init ${MESON_BUILD_ROOT}/initramfs/ && \
cp -u /usr/lib64/{libc.so.?,libm.so.?,libgcc_s.so.?,libuuid.so.?,libdevmapper.so.?.??,libgcrypt.so.??,libgpg-error.so.?,libudev.so.?,libpthread.so.?,libcryptsetup.so.?,ld-linux-x86-64.so.?} ${MESON_BUILD_ROOT}/initramfs/usr/lib64/ && \
cd ${MESON_BUILD_ROOT}/initramfs && \
find . -print0 | cpio --null -ov --format=newc | gzip -9 > ${MESON_BUILD_ROOT}/${INITRD_FILE}
mkdir -p ${DESTDIR}/${MESON_INSTALL_PREFIX}/lib/kernel/ && \
cp ${MESON_BUILD_ROOT}/${INITRD_FILE} ${DESTDIR}/${MESON_INSTALL_PREFIX}/lib/kernel/${INITRD_FILE}
cd ${MESON_SOURCE_ROOT}
