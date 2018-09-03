# Exclave Tests for Tomu

This is a configuration directory for Tomu tests, written for use with Exclave.

Currently the test sequence is:

1. Load firmware
1. Blink LEDs 3x

Future versions of the test should perform a more rigorous inspection of the connectivity, but for now we assume that if the firmware load is successful, and if both LEDs can blink, then everything else was assembled correctly.

## Hardware

This hardware was designed to run on a Raspberry Pi.  With the current configuration, the following connections must be made from the Pi header:

````
+--------------------+
|55G---G-D-C---G-G---|
|----GRYg----G-----SG|
+--------------------+

G = GND
R = Red LED
Y = Yellow LED
g = Green LED
S = Switch
D = SWD
C = SWC
5 = 5V
````

| Pin | Target
| --- | ------
| 2   | Tomu +5V
| 6   | Tomu GND
| 9   | LEDs GND
| 11  | Red LED
| 13  | Yellow LED
| 15  | Green LED
| 18  | Tomu SWD
| 22  | Tomu SWC
| 37  | START button
| 39  | START button (GND)

## Usage

1. Build [Exclave](https://exclave.io/)
1. Run exclave -c [path-to-this-directory]

## Tester Setup

In order to set up the tester, perform the following steps:

### Building Dependencies (host system)

```sh
mkdir tomu-tester
cd tomu-tester
sudo apt install gcc-arm-linux-gnueabihf
mkdir boot

# Create the new /boot/ directory and tell fedberry to launch in "headless" mode
touch boot/headless

# Inform exclave that this is a Tomu jig by creating a magic file
touch boot/tomu-jig

# Build openocd
git clone --recurse-submodules git://git.code.sf.net/p/openocd/code openocd
cd openocd
./bootstrap
./configure --prefix=/boot --host=arm-linux-gnueabihf --enable-bcm2835gpio --enable-sysfsgpio --disable-jlink
make install DESTDIR=$(pwd)/../
cd ..

# Build exclave
curl https://sh.rustup.rs -sSf | sh
export PATH="$PATH:~/.cargo/bin"
rustup target install armv7-unknown-linux-gnueabihf
git clone https://github.com/exclave/exclave.git
cd exclave
cargo build --target=armv7-unknown-linux-gnueabihf --release
cp target/armv7-unknown-linux-gnueabihf/release/exclave ../boot/bin/
cd ..

# Check out the tests
git clone https://github.com/im-tomu/tomu-tests.git boot/tomu-tests
```

### Installing FedBerry

1. Download it from http://download.fedberry.org/releases/
1. Write it to an SD card (using dd or win32diskimager)
1. Copy the contents of `boot/` from above onto the root of partition #2 (the FAT partition)
1. Insert the SD card into the Raspberry Pi and boot it
1. Connect to the Raspberry Pi via ssh.  Username is `root` and password is `fedberry`
1. Log back in after setting the password
1. Run the following script to set everything up:

```sh
echo "kernel.sysrq=0" >> /etc/sysrq.conf
echo 'dwc_otg.lpm_enable=0 root=/dev/mmcblk0p2 quiet splash loglevel=0 logo.nologo vt.global_cursor_default=0 ro rootfstype=ext4 elevator=deadline fsck.repair=yes rootwait libahci.ignore_sss=1 raid=noautodetect nortc selinux=0 audit=0 quiet' > /boot/cmdline.txt
systemctl mask serial-getty@ttyAMA0.service
echo exclave > /etc/hostname
fedberry-config --dto-enable pi3-disable-wifi --dto-enable pi3-disable-bt --vc4-disable --dto-enable watchdog
dnf install -y dfu-util rsync
cat > /etc/systemd/system/exclave.service <<EOF
[Unit]
Description=Launcher for Exclave

[Service]
Type=simple
ExecStart=/boot/bin/exclave -c /boot/tomu-tests
User=root
WorkingDirectory=/boot/tomu-tests

[Install]
WantedBy=getty.target
EOF
cat > /etc/fstab <<EOF
/dev/mmcblk0p2  /       ext4    defaults,noatime,ro         0 0
/dev/mmcblk0p1  /boot   vfat    defaults,noatime,ro         0 0
tmpfs           /tmp    tmpfs   defaults,noatime,size=100m  0 0
EOF
systemctl enable exclave.service
reboot
```

Note that the filesystem is much bigger than it needs to be.  You can copy it to a smaller disk by connecting a replacement SD card via USB and running the following:
```sh
dd if=/dev/zero of=/dev/sda bs=1M count=1
fdisk /dev/sda <<EOF
n
p
1

+200M
t
c
n
p
2


a
1
w
EOF
mkfs.vfat -F 16 /dev/sda1
mkfs.ext4 -F /dev/sda2
mount /dev/sda1 /mnt
rsync -avxHAX --progress /boot/ /mnt
umount /mnt
mount /dev/sda2 /mnt
rsync -avxHAX --progress / /mnt
umount /mnt
```
