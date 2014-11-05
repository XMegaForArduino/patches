#!/bin/sh

echo "This will build apply patches, then build binutils, gcc, libc"
echo "for the avr platform, to run under cygwin."
echo ""

read -n 1 -p "Enter Y to continue, anything else to quit: " YN

if test -z "$YN"; then
  exit
fi

if test "$YN" == "Y" || test "$YN" == "y" ; then
  echo ""
  echo "Here we go..."
  echo ""
else
  exit
fi

# cleaning first

pushd avr-binutils

if test -e work ; then
  echo "cleaning up old workfiles"
  rm -rf work
fi

echo "extracting source"

mkdir work
cd work
tar -xj -f ../../binutils-2.24.tar.bz2

cd binutils-2.24

if test -e ../../files ; then
  echo "patching"

  for xx in ../../files/* ; do
    pwd
    echo patch using $xx
    patch -p0 <$xx
  done
fi

popd

# now do the same with avr-gcc and avr-libc

pushd avr-gcc

if test -e work ; then
  echo "cleaning up old workfiles"
  rm -rf work
fi

echo "extracting source"

mkdir work
cd work
tar -xj -f ../../gcc-4.8.3.tar.bz2

cd gcc-4.8.3

if test -e ../../files ; then
  echo "patching"

  for xx in ../../files/* ; do
    pwd
    echo patch using $xx
    patch -p0 <$xx
  done
fi

# fix a bug in the build
mkdir gcc/include
chmod 777 gcc/include

popd

# and finally, avr-libc

pushd avr-libc

if test -e work ; then
  echo "cleaning up old workfiles"
  rm -rf work
fi

echo "extracting source"

mkdir work
cd work
tar -xj -f ../../avr-libc-1.8.1.tar.bz2

cd avr-libc-1.8.1

if test -e ../../files ; then
  echo "patching"

  for xx in ../../files/* ; do
    pwd
    echo patch using $xx
    patch -p0 <$xx
  done
fi

popd


# again for avrdude

pushd avrdude

if test -e work ; then
  echo "cleaning up old workfiles"
  rm -rf work
fi

echo "extracting source"

mkdir work
cd work
tar -xz -f ../../avrdude-6.1.tar.gz

cd avrdude-6.1

if test -e ../../files ; then
  echo "patching"

  for xx in ../../files/* ; do
    pwd
    echo patch using $xx
    patch -p0 <$xx
  done
fi

popd



echo "Now we are finished with extract and patch"
echo ""
read -n 1 -p "Enter Y to continue, anything else to quit: " YN

if test -z "$YN"; then
  exit
fi

if test "$YN" == "Y" || test "$YN" == "y" ; then
  echo ""
  echo "Here we go again..."
  echo ""
else
  exit
fi


make -f MegaMake

