import subprocess


class ArmFlasher():
    """ Some convenience functions for flashing the ARM """

    def __init__(self, configfile="openocd.cfg"):
        self.configfile = configfile
        pass

    def eraseFlash(self):
        args = ["openocd", "-f", self.configfile,
                "-c", '"init"',
                "-c", "kinetis mdm mass_erase",
                "-c", "reset halt",
                "-c", "kinetis disable_wdog",
                "-c", "reset halt",
                "-c", "exit"
                ]

        return subprocess.call(args) == 0

    def writeFirmware(self, imageFile):
        args = ["openocd", "-f", self.configfile,
                "-c", '"init"',
                "-c", "reset halt",
                "-c", "kinetis disable_wdog",
                "-c", "reset halt",
                "-c", "flash write_image %s" % (imageFile),
                "-c", "reset",
                "-c", "exit"
                ]

        return subprocess.call(args) == 0


if __name__ == '__main__':
    armFlasher = ArmFlasher()
    if not armFlasher.eraseFlash():
        print("Error erasing flash")
        exit(1)

    if not armFlasher.writeFirmware("../../bootloader/blinky-boot.elf"):
        print("Error writing bootloader")
        exit(1)

    if not armFlasher.writeFirmware("../../firmware/app-image.hex"):
        print("Error writing app image")
        exit(1)
