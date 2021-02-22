# Read me to keep track of steps/documentation

## Cosim irqs
- Add debugdev to top level device tree so that linux kernel can see it
    - /home/peta/xilinx-zcu102-2019.2/project-spec/meta-user/recipes-bsp/device-tree/files/system-user.dtsi
    - this device tree get compiled by petalinux-build cli

- Add uio support to kernel 
    -petalinux-config -c kernel
    - Device Drivers -> Userspace I/O drivers
        - <*> Userspace I/O platform driver with generic IRQ handling
        - <*> Userspace platform driver with generic irq and dynamic memory
        - <*> Xilinx AI Engine driver

- Use uio module to probe our device 
    -debugdevice should contain compability string that uio is looking for 
    - added directly in device tree with 
        chosen {
        bootargs = "earlycon clk_ignore_unused   uio_pdrv_genirq.of_id=generic-uio";
        stdout-path = "serial0:115200n8";
    };
    - or through petalinux-config and changing the boot args there


## What I have done so far 
- I created a seperate .dtsi file 
    - this file contains the debugdev information 
- This file in included on system-user.dtsi
- I can visually confirm that the debugdevice has been added to the compiled device tree that QEMU  uses.
    - decompiled commmand: dtc -I dtb -O dts -o decompiled.dts system.dtb 

- I added bootargs through petalinux-config
    - uio_pdrv_genirq.of_id=debuginc,generic-uio,ui_pdrv
- Debugdev still not visible from QEMU side