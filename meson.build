project('clr-init', 'c',
    version : '0.1',
    license : 'GPL-3.0',
    meson_version : '>= 0.44'
)
src = [
    'src/crypting.c',
    'src/device_handler.c',
    'src/initrd.c',
    'src/error.c',
    'src/main.c'
]

# Default to 10-clr-init.img.gz
initrd_img = get_option('with-initrd-name')
if initrd_img == ''
    initrd_img = 'clr-init.img.gz'
endif

cryptsetup = dependency('libcryptsetup')
executable('init', src, dependencies : cryptsetup)

install_man('clr-init.1')
meson.add_install_script('install_initrd.sh', initrd_img)
