#!/bin/sh

cd /etc/setup

if test -e /tmp/y.txt ; then
  rm -f /tmp/y.txt
fi

ls | sed 's/.lst.gz//' >/tmp/y.txt

cd /cygdrive/C/cygwin/downloads

if test -e /tmp/x.txt ; then
  rm -f /tmp/x.txt
fi

find . -name "*.tar.bz2" >/tmp/x.txt

if test -e /tmp/z.txt ; then
  rm -f /tmp/z.txt
fi

touch /tmp/z.txt

for yy in `cat /tmp/y.txt` ; do

  ww=`grep "${yy}-" /tmp/x.txt | sort -u | tail -n 1`

  if test -z "$ww" ; then
    echo ${yy}"	(no version)" >>/tmp/z.txt
  else
    zz=`basename $ww | sed 's/.tar.bz2//'`
    echo ${yy}"	"${zz} >>/tmp/z.txt
  fi
done

sort </tmp/z.txt


