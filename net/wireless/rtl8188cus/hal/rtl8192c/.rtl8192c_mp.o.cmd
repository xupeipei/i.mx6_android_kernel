cmd_drivers/net/wireless/rtl8188cus/hal/rtl8192c/rtl8192c_mp.o := arm-eabi-gcc -Wp,-MD,drivers/net/wireless/rtl8188cus/hal/rtl8192c/.rtl8192c_mp.o.d  -nostdinc -isystem /home/only/myandroid/prebuilt/linux-x86/toolchain/arm-eabi-4.4.3/bin/../lib/gcc/arm-eabi/4.4.3/include -I/home/only/myandroid/kernel_imx/arch/arm/include -Iarch/arm/include/generated -Iinclude  -include include/generated/autoconf.h -D__KERNEL__ -mlittle-endian -Iarch/arm/mach-mx6/include -Iarch/arm/plat-mxc/include -Wall -Wundef -Wstrict-prototypes -Wno-trigraphs -fno-strict-aliasing -fno-common -Werror-implicit-function-declaration -Wno-format-security -fno-delete-null-pointer-checks -O2 -marm -fno-dwarf2-cfi-asm -mabi=aapcs-linux -mno-thumb-interwork -funwind-tables -D__LINUX_ARM_ARCH__=7 -march=armv7-a -msoft-float -Uarm -Wframe-larger-than=1024 -fno-stack-protector -fomit-frame-pointer -Wdeclaration-after-statement -Wno-pointer-sign -fno-strict-overflow -fconserve-stack -O1 -Wno-unused-variable -Wno-unused-value -Wno-unused-label -Wno-unused-parameter -Wno-unused-function -Wno-unused -Wno-uninitialized -Idrivers/net/wireless/rtl8188cus/include -DCONFIG_POWER_SAVING -DCONFIG_LITTLE_ENDIAN  -DMODULE  -D"KBUILD_STR(s)=\#s" -D"KBUILD_BASENAME=KBUILD_STR(rtl8192c_mp)"  -D"KBUILD_MODNAME=KBUILD_STR(8192cu)" -c -o drivers/net/wireless/rtl8188cus/hal/rtl8192c/.tmp_rtl8192c_mp.o drivers/net/wireless/rtl8188cus/hal/rtl8192c/rtl8192c_mp.c

source_drivers/net/wireless/rtl8188cus/hal/rtl8192c/rtl8192c_mp.o := drivers/net/wireless/rtl8188cus/hal/rtl8192c/rtl8192c_mp.c

deps_drivers/net/wireless/rtl8188cus/hal/rtl8192c/rtl8192c_mp.o := \
    $(wildcard include/config/mp/included.h) \
    $(wildcard include/config/rtl8192c.h) \
    $(wildcard include/config/usb/hci.h) \
    $(wildcard include/config/rtl8192d.h) \
    $(wildcard include/config/mp/include.h) \

drivers/net/wireless/rtl8188cus/hal/rtl8192c/rtl8192c_mp.o: $(deps_drivers/net/wireless/rtl8188cus/hal/rtl8192c/rtl8192c_mp.o)

$(deps_drivers/net/wireless/rtl8188cus/hal/rtl8192c/rtl8192c_mp.o):
