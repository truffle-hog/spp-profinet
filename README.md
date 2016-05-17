# spp_profinet

## Setup

1. Build Snort from source using this [guide](https://s3.amazonaws.com/snort-org-site/production/document_files/files/000/000/090/original/Snort_2.9.8.x_on_Ubuntu_12-14-15.pdf) (chapter 9 and 10).
2. Extract the [zipped version of spp_profinet](https://github.com/TruffleHog/spp_profinet/archive/master.zip) into your _Snort root_ __overwriting existing files__.
3. Execute the following in your _Snort root_:
```
sudo apt-get install automake
sudo apt-get install autoconf
sudo apt-get install libtool
make clean
make distclean
## Be careful!! MAKE SURE YOU ARE IN YOUR SNORT MAIN SOURCE FOLDER
find . -name "Makefile.in" -type f -delete
aclocal
autoconf
automake --add-missing
autoreconf --force --install
./configure.sh
make
sudo make install
```

Comment: _Snort root_ is the main source folder you built Snort in. 
