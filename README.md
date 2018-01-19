## clr-init
small init wrote with C programming language that takes information from the cmdline in proc to mount root and execute
/sbin/init from the root partition.

## Luks Encryption support

for partitions encrypted with luks a password input will be displayed in boot time to decrypt root partition, add options
to cmdline is not needed to know that root partition is encrypted.

## How it works

Taken two paramters from cmdline root and rootfstype root partition is mounted and its systemd(/sbin/init) is running
nothing different than other initrd.

Initrd just work when root option use the device name directly or PARTUUID for encryption initrd read the first part of
the root partition looking for a luks header.

## Compilation and Install

This will will install clr-init.img.gz in /usr/local/lib/initrd
```
meson builddir && ninja -C builddir install
```
install path could be changed using as sample:
```
meson --prefix /usr builddir
```
in this case the installation path will be /usr/lib/initrd
