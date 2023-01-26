#!/bin/sh
cat ../../badvpn/tun2socks/build_original.sh | tr ' ' '\n' | grep -E "\.a$" | awk '{print "../../badvpn/tun2socks/"$s}' | xargs -i -r -n1 cp "{}" ./
find -iname "*\.a" | xargs -i -r -n1 ar -x "{}"
rm *.a
cat ../../badvpn/tun2socks/build_original.sh | tr ' ' '\n' | grep -E "\.o$" | awk '{print "../../badvpn/tun2socks/"$s}' | xargs -i -r -n1 cp "{}" ./
find -iname "*.o" | xargs -i -r -n1 nm "{}" |  grep -Ev "( U )|( W )|( w )" | cut -d ' ' -f3- | sort | uniq > all_exported_symbols.txt
cat all_exported_symbols.txt | awk '{print "--redefine-sym " $s "=badvpn_" $s}' | tr '\n' ' ' > cmd_line_args.txt
find -iname "*.o" | xargs -i -r -n1 objcopy `cat cmd_line_args.txt` "{}"
