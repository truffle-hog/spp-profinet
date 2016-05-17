# spp_profinet

[![Gitter](https://badges.gitter.im/TruffleHogProject/spp_profinet.svg)](https://gitter.im/TruffleHogProject/spp_profinet?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)

Build your Snort software from source using this guide: https://s3.amazonaws.com/snort-org-site/production/document_files/files/000/000/090/original/Snort_2.9.8.x_on_Ubuntu_12-14-15.pdf

Relevant chapters are only Chapter 9 and 10.

Copy the files from this repository into your snort source folder overwriting existing files.

Go into the snort source main folder an execute:

sudo apt-get install libtool
make clean
make distclean
find . -name "Makefile.in" -type f -delete
aclocal
autoconf
automake --add-missing
autoreconf --force --install
./configure.sh
make
sudo make install
