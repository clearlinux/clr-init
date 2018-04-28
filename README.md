## clr-init
Initrd created using systemd as init program

## Luks Encryption support

The systemd rd parameters for luks most be added to the kernel cmdline to know who is the encrypted root partition:
https://www.freedesktop.org/software/systemd/man/systemd-cryptsetup-generator.html#Kernel%20Command%20Line

This initrd most not be using with LVM partitons, how ever it could be complemented adding services in an additional initrd

## How this works

This is the flow of an initrd with systemd:
```
    +----------------+
    | systemd (init) |
    +----------------+
               +
               v
    +--------------------------------+
    | system-generators              |
    |--------------------------------|
    | nfs-server-generator           |
    | rpc-pipefs-generator           |
    | systemd-cryptsetup-generator   |
    | systemd-debug-generator        |
    | systemd-fstab-generator        |
    | systemd-getty-generator        |
    | systemd-gpt-auto-generator     |
    | systemd-system-update-generator|
    | systemd-veritysetup-generator  |
    +--------------------------------+
               +
               |
               v
     +-------------------+
     |local-fs-pre.target|
     +-------------------+
               +
               |
               |
               |
               v
 +-----------------------------+        +-----------------+         +-----------------------+
 |  local-fs.target            |        |  swap.target    |         |  cryptsetup.target    |
 |-----------------------------|        |-----------------|         |-----------------------|
 |                             |        |                 |         |                       |
 |  tmp.mount                  |        |  (various swap  |         |  (various cryptsetup  |
 |  systemd-remount-fs.service |        |   devices...)   |         |      devices...)      |
 |                             |        |                 |         |                       |
 +-----------------------------+        +-----------------+         +-----------------------+
               +                               |                               |
               |                               |                               |
               +------------------------------>|<------------------------------+
                                               v
                           +--------------------------------------+
                           |  sysinit.target                      |
                           |--------------------------------------|
                           |  systemd-ask-password-console.path   |
                           |  systemd-tmpfiles-setup-dev.service  |
                           |  systemd-tmpfiles-setup.service      |
                           |  systemd-udevd.service               |
                           |  systemd-udev-trigger.service        |
                           +--------------------------------------+
                                               +
                                               |
         +------------------+-----------------+|+---------------+------------------------+
         |                  |                  |                |                        |
         |                  |                  |                |                        |
         v                  v                  |                v                        v
 +----------------+ +----------------+         |  +------------------------------++---------------+
 |  timers.target | |  paths.target  |         |  |  sockets.target              || rescue.target |
 |----------------| |----------------|         |  |------------------------------|+---------------+
 |   (various     | |  (various      |         |  |  systemd-udevd-control.socket|
 |  timers...)    | |  paths...)     |         |  |  systemd-udevd-kernel.socket |
 |                | |                |         |  |                              |
 +----------------+ +----------------+         |  +------------------------------+
                           +                   |                +
                           |                   |                |
                           |                   |                |
                           +------------------>|<---------------+
                                               |
                                               |
                                               |
                                               v
                                        +------------+
                                        |basic.target|
                                        +------------+
                                               +
                                               |
                                               v
                                +--------------------------------+              +----------------+
                                |  initrd-root-device.target     |              |emergency.target|
                                |--------------------------------|              +----------------+
                                |         sysroot.mount          |
                                +--------------------------------+
                                               +
                                               |
                                               v
                                +--------------------------------+
                                |    initrd-root-fs.target       |
                                |--------------------------------|
                                |   initrd-parse-etc.service     |
                                +--------------------------------+
                                               +
                                               |
                                               v
                                +--------------------------------+
                                |      initrd-fs.target          |
                                |--------------------------------|
                                |   (sysroot-usr.mount and       |
                                |    various mounts marked       |
                                |      with fstab option         |
                                |     x-initrd.mount...)         |
                                +--------------------------------+
                                               +
                                               |
                                               v
                              +------------------------------------+
                              |           initrd.target            |
                              |------------------------------------|
                              |      initrd-cleanup.service        |
                              | initrd-udevadm-cleanup-db.service  |
                              +------------------------------------+
                                               +
                                               |
                                               v
                                +-------------------------------+
                                |  initrd-switch-root.target    |
                                |-------------------------------|
                                |                               |
                                |  initrd-switch-root.service   |
                                |                               |
                                +-------------------------------+
                                               +
                                               |
                                               v
                                   +------------------------+
                                   | Transition to Host OS  |
                                   +------------------------+
```

## Create and Install

This will install clr-init.cpio.gz in /usr/lib/initrd
```
make && make install
```
