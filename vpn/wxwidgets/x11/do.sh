#!/bin/sh
echo - - - - Deleting all files
rm -rf *.o *.a *.txt
echo - - - - Copying intact .a files
/usr/local/lib/wx/config/x11univ-ansi-debug-static-2.8 --universal=no --libs std | tr ' ' '\n' | sort | uniq | grep "\.a$" | xargs -i -r -n1 cp "{}" ./
echo - - - - Extracting .o files from .a files
find -name "*\.a" | xargs -i -r -n1 ar -x "{}"
echo - - - - Deleting all .a files
rm -rf *.a
echo - - - - Compiling an exhaustive list of exported symbols
nm --defined-only *.o | cut -d ' ' -f 3 | sort | uniq > exported_symbols.txt
echo - - - - Making command line options for objcopy \(one per line\)
cat exported_symbols.txt | awk '{print "--redefine-sym "$s"=wxX11_"$s}' > redefs.txt
echo - - - - Arranging command line options for objcopy \(40 per line\)
cat redefs.txt | xargs -r -L 40 echo >> redefs40.txt
echo - - - - Adding a prefix to all the exported symbols
cat redefs40.txt | while read options; do find -name "*\.o" | xargs --max-procs=10 -r -L1 objcopy ${options}; done
echo - - - - Making an archive file x11.a \(i.e. a static library\)
ar -r x11.a *.o
echo - - - - FINISHED
