#!/bin/bash
# Langstone Install script 
# Buster Version G4EML 27/01/20


# Update the package manager
sudo dpkg --configure -a
sudo apt-get -y update

# Uninstall the apt-listchanges package to allow silent install of ca certificates (201704030)
# http://unix.stackexchange.com/questions/124468/how-do-i-resolve-an-apparent-hanging-update-process
sudo apt-get -y remove apt-listchanges

# -------- Upgrade distribution ------

# Update the distribution
sudo apt-get -y dist-upgrade


# Install the packages that we need
sudo apt-get -y install git
sudo apt-get -y install libxml2 libxml2-dev bison flex libcdk5-dev cmake
sudo apt-get -y install libaio-dev libusb-1.0-0-dev libserialport-dev libxml2-dev libavahi-client-dev sudo apt-get install doxygen graphviz
sudo apt-get -y install gr-iio


# install scratchradio which includes Limesuite and Gnu Radio 
git clone https://github.com/myriadrf/ScratchRadio
cd Scratchradio
sudo ./scripts/install_deps.sh
make LimeSuite
make GnuRadio
make clean

# install WiringPi
cd /tmp
wget https://project-downloads.drogon.net/wiringpi-latest.deb
sudo dpkg -i wiringpi-latest.deb
cd ~

#install libiio
git clone https://github.com/analogdevicesinc/libiio.git
cd libiio
cmake ./
make all
sudo make install

cd ~

# install the Langstone Files



