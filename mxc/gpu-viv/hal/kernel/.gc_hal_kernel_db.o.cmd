cmd_drivers/mxc/gpu-viv/hal/kernel/gc_hal_kernel_db.o := /home/only/myandroid/prebuilt/linux-x86/toolchain/arm-eabi-4.4.3/bin/arm-eabi-gcc -Wp,-MD,drivers/mxc/gpu-viv/hal/kernel/.gc_hal_kernel_db.o.d  -nostdinc -isystem /home/only/myandroid/prebuilt/linux-x86/toolchain/arm-eabi-4.4.3/bin/../lib/gcc/arm-eabi/4.4.3/include -I/home/only/myandroid/kernel_imx/arch/arm/include -Iarch/arm/include/generated -Iinclude  -include include/generated/autoconf.h -D__KERNEL__ -mlittle-endian -Iarch/arm/mach-mx6/include -Iarch/arm/plat-mxc/include -Wall -Wundef -Wstrict-prototypes -Wno-trigraphs -fno-strict-aliasing -fno-common -Werror-implicit-function-declaration -Wno-format-security -fno-delete-null-pointer-checks -O2 -marm -fno-dwarf2-cfi-asm -mabi=aapcs-linux -mno-thumb-interwork -funwind-tables -D__LINUX_ARM_ARCH__=7 -march=armv7-a -msoft-float -Uarm -Wframe-larger-than=1024 -fno-stack-protector -fomit-frame-pointer -Wdeclaration-after-statement -Wno-pointer-sign -fno-strict-overflow -fconserve-stack -Werror -DLINUX -DDRIVER -DENUM_WORKAROUND=0 -DDBG=0 -DgcdPOWER_MANAGEMENT=1 -DUSE_PLATFORM_DRIVER=1 -DVIVANTE_PROFILER=0 -DENABLE_GPU_CLOCK_BY_DRIVER=1 -DUSE_NEW_LINUX_SIGNAL=0 -DNO_USER_DIRECT_ACCESS_FROM_KERNEL=1 -DgcdPAGED_MEMORY_CACHEABLE=0 -DgcdNONPAGED_MEMORY_CACHEABLE=0 -DgcdNONPAGED_MEMORY_BUFFERABLE=1 -DgcdCACHE_FUNCTION_UNIMPLEMENTED=0 -DgcdSUPPORT_SWAP_RECTANGLE=0 -DgcdENABLE_VG=1 -DgcdSMP=1 -DgcdENABLE_OUTER_CACHE_PATCH=1 -DgcdENABLE_BANK_ALIGNMENT=1 -DgcdBANK_BIT_START=13 -DgcdBANK_BIT_END=15 -DgcdBANK_CHANNEL_BIT=12 -I/home/only/myandroid/kernel_imx/drivers/mxc/gpu-viv/hal/kernel/inc -I/home/only/myandroid/kernel_imx/drivers/mxc/gpu-viv/hal/kernel -I/home/only/myandroid/kernel_imx/drivers/mxc/gpu-viv/arch/XAQ2/hal/kernel -I/home/only/myandroid/kernel_imx/drivers/mxc/gpu-viv/hal/os/linux/kernel -I/home/only/myandroid/kernel_imx/drivers/mxc/gpu-viv/arch/GC350/hal/kernel    -D"KBUILD_STR(s)=\#s" -D"KBUILD_BASENAME=KBUILD_STR(gc_hal_kernel_db)"  -D"KBUILD_MODNAME=KBUILD_STR(galcore)" -c -o drivers/mxc/gpu-viv/hal/kernel/.tmp_gc_hal_kernel_db.o drivers/mxc/gpu-viv/hal/kernel/gc_hal_kernel_db.c

source_drivers/mxc/gpu-viv/hal/kernel/gc_hal_kernel_db.o := drivers/mxc/gpu-viv/hal/kernel/gc_hal_kernel_db.c

deps_drivers/mxc/gpu-viv/hal/kernel/gc_hal_kernel_db.o := \
  drivers/mxc/gpu-viv/hal/kernel/gc_hal_kernel_precomp.h \
  /home/only/myandroid/kernel_imx/drivers/mxc/gpu-viv/hal/kernel/inc/gc_hal.h \
  /home/only/myandroid/kernel_imx/drivers/mxc/gpu-viv/hal/kernel/inc/gc_hal_rename.h \
  /home/only/myandroid/kernel_imx/drivers/mxc/gpu-viv/hal/kernel/inc/gc_hal_types.h \
  /home/only/myandroid/kernel_imx/drivers/mxc/gpu-viv/hal/kernel/inc/gc_hal_version.h \
  /home/only/myandroid/kernel_imx/drivers/mxc/gpu-viv/hal/kernel/inc/gc_hal_options.h \
    $(wildcard include/config/smp.h) \
  /home/only/myandroid/kernel_imx/drivers/mxc/gpu-viv/hal/kernel/inc/gc_hal_enum.h \
  /home/only/myandroid/kernel_imx/drivers/mxc/gpu-viv/hal/kernel/inc/gc_hal_base.h \
    $(wildcard include/config/format/info.h) \
  /home/only/myandroid/kernel_imx/drivers/mxc/gpu-viv/hal/kernel/inc/gc_hal_dump.h \
  /home/only/myandroid/kernel_imx/drivers/mxc/gpu-viv/hal/kernel/inc/gc_hal_profiler.h \
  /home/only/myandroid/kernel_imx/drivers/mxc/gpu-viv/hal/kernel/inc/gc_hal_driver.h \
  /home/only/myandroid/kernel_imx/drivers/mxc/gpu-viv/hal/kernel/inc/gc_hal_driver_vg.h \
  /home/only/myandroid/kernel_imx/drivers/mxc/gpu-viv/hal/kernel/inc/gc_hal_vg.h \
  /home/only/myandroid/kernel_imx/drivers/mxc/gpu-viv/hal/kernel/inc/gc_hal_driver.h \
  drivers/mxc/gpu-viv/hal/kernel/gc_hal_kernel.h \
  /home/only/myandroid/kernel_imx/drivers/mxc/gpu-viv/arch/XAQ2/hal/kernel/gc_hal_kernel_hardware.h \
  /home/only/myandroid/kernel_imx/drivers/mxc/gpu-viv/arch/GC350/hal/kernel/gc_hal_kernel_hardware_vg.h \
  drivers/mxc/gpu-viv/hal/kernel/gc_hal_kernel_vg.h \

drivers/mxc/gpu-viv/hal/kernel/gc_hal_kernel_db.o: $(deps_drivers/mxc/gpu-viv/hal/kernel/gc_hal_kernel_db.o)

$(deps_drivers/mxc/gpu-viv/hal/kernel/gc_hal_kernel_db.o):
