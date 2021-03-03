# Read me to keep track of steps/documentation

## Cosim irqs
- Add debugdev to top level device tree so that linux kernel can see it
    - /home/peta/xilinx-zcu102-2019.2/project-spec/meta-user/recipes-bsp/device-tree/files/system-user.dtsi
    - add the new device directly under the amba bus 
    - &amba{
            your_device: device0 {
            ....
            ...
            };
        };
    - this device tree gets compiled by petalinux-build cli

- Add uio support to kernel 
    -petalinux-config -c kernel
    - Device Drivers -> Userspace I/O drivers
        - <M> Userspace I/O platform driver with generic IRQ handling
        - <M> Userspace platform driver with generic irq and dynamic memory
        - <M> Xilinx AI Engine driver

    - Make sure that you use M instead of *

- Use uio module to probe our device 
    -debugdevice should contain compability string that uio is looking for 
    - added directly in device tree with 
        chosen {
        bootargs = "earlycon clk_ignore_unused   uio_pdrv_genirq.of_id=generic-uio";
        stdout-path = "serial0:115200n8";
    };
    - or through petalinux-config and changing the boot args there
    - **use petalinux-config tool instead. I have found that adding bootargs directly on device tree does not work b/c. It gets over written **
- debugdev entry is in /proc/device-tree
    - /proc/device-tree/debugdev@0xa0000000
    - catting interrupts prints Y
    - catting /proc/interrupts shows debugdev which contains a GIC V2 interrupt number of +32 from what is specified on the device tree



## What I have done so far 
- I created a seperate .dtsi file 
    - this file contains the debugdev information 
- This file in included on system-user.dtsi
- I can visually confirm that the debugdevice has been added to the compiled device tree that QEMU  uses.
    - decompiled commmand: dtc -I dtb -O dts -o decompiled.dts system.dtb 

- I added bootargs through petalinux-config
    - uio_pdrv_genirq.of_id=debuginc,generic-uio,ui_pdrv
- Debugdev still not visible from QEMU side
- Debugdev is not visible and recieving irqs from the PL side
    - the main problem I was having was the bootargs were getting overwritten when I directly changed it on the system-user.dtsi file 
    - use the petalinux-config tool for this to work correctly


## Resources 
- https://forums.xilinx.com/t5/Embedded-Linux/Does-anyone-have-or-know-of-an-example-using-UIO-with-a-private/td-p/1083861
    - could be a problem with petalinux 2019.1

- https://www.kernel.org/doc/html/v4.17/driver-api/uio-howto.html#using-uio-pdrv-genirq-for-platform-devices
    - how the uio framework works

- https://harmoninstruments.com/posts/uio.html
    - uio user level example
