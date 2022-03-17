# CMPT-433-Project READ ME

### Must do before execution

#### Edit the device tree on your BeagleBone Green:

    1. Navigate to 

    ```cd /boot/
    ```

    2. Create a copy of your ```uEnv.txt``` in case anything goes wrong

    ```sudo cp /boot/uEnv.txt /boot/uEnv-BeforePwm.txt
    ```

    3. Open the ```uEnv.txt``` file to edit
    ```sudo nano uEnv.txt
    ```

    4. Add these additional capes under *```###Additional custom capes```
    ```###Additional custom capes
        uboot_overlay_addr4=/lib/firmware/BB-BONE-AUDI-02-00A0.dtbo
        uboot_overlay_addr5=/lib/firmware/BB-I2C1-00A0.dtbo
        uboot_overlay_addr6=/lib/firmware/BB-PWM0-00A0.dtbo
        uboot_overlay_addr7=/lib/firmware/BB-PWM1-00A0.dtbo
        uboot_overlay_addr3=/lib/firmware/BB-PWM2-00A0.dtbo
    ```

    5. Reboot your BeagleBone Green

