# Blinkinlabs Test Rig Setup

The factory test uses a Raspberry Pi 

Setup instructions for 'Raspbian Jessie Lite'

# Get the system up to date

	sudo apt-get update
	sudo apt-get install build-essential git libtool automake libusb-1.0.0-dev python-smbus python-serial i2c-tools vim screen python-dev python-pip gcc-arm-none-eabi dfu-uitl python-colorama python-rpi.gpio autopep8

    sudo pip install subprocess32

# Install openocd

	git clone git://git.code.sf.net/p/openocd/code openocd-code
	cd openocd-code/

Make this modification for Kinetis k20 parts:

diff --git a/src/flash/nor/kinetis.c b/src/flash/nor/kinetis.c
index aed37b9..8ab4c11 100644
--- a/src/flash/nor/kinetis.c
+++ b/src/flash/nor/kinetis.c
@@ -1479,6 +1479,9 @@ static int kinetis_probe(struct flash_bank *bank)
        }
 
        /* when the PFLSH bit is set, there is no FlexNVM/FlexRAM */
+
+       fcfg2_pflsh = true;
+
        if (!fcfg2_pflsh) {
                switch (fcfg1_nvmsize) {
                case 0x03:
                

./bootstrap
./configure
make
make install

# Install avrdude

	wget http://download.savannah.gnu.org/releases/avrdude/avrdude-6.2.tar.gz
	tar xfv avrdude-6.2.tar.gz
	cd avrdude-6.2
	./bootstrap
	./configure --enable-linuxgpio
	make
	sudo make install


# Install python libs

	sudo pip install adafruit-ads1x15

# Install project repository

	git clone https://github.com/Blinkinlabs/HopeBadge.git


# Fix monitor resolution

For the 8" HDMI monitor

sudo vi /boot/config.txt

	# force 800x600
	hdmi_group=2
	hdmi_mode=9
	display_rotate=3

# Suppress the raspberry pi logo

sudo vi /boot/cmdline.txt

add to the end of the line:

	logo.nologo

# Don't wait for network at boot

Run:

	sudo raspi-config

Select option 'wait for network at boot' and choose 'No'


# Configure to start at boot

Edit the file:

	sudo vi /lib/systemd/system/getty@.service

With this replacement:

	#ExecStart=-/sbin/agetty --noclear %I $TERM
	ExecStart=-/sbin/agetty --noclear -a pi %I $TERM

Edit the file:

	vi ~/.bashrc

Adding this o the end:

	if [ -z "$SSH_CLIENT" ]; then
        	setterm -blank 0
	        clear
        	cd /home/pi/HopeBadge/production/test/; sudo ./testrig.sh
	fi

And reboot.

# Rejoice!

