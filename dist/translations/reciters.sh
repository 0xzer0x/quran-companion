#!/bin/bash
# script to generate a ts file for reciter display names from the corresponding xml file
set -e

SCRIPT_DIR=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" &>/dev/null && pwd)

tsfile=$SCRIPT_DIR/reciters.ts
xmlfile=$SCRIPT_DIR/../../resources/reciters.xml

SAVEIFS=$IFS # Save current IFS (Internal Field Separator)
IFS=$'\n'    # Change IFS to newline char
names=($(awk -F'"' '/display=/ { print $6 }' $xmlfile))
IFS=$SAVEIFS # Restore original IFS

printf "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n<!DOCTYPE TS>\n<TS version=\"2.1\">\n" >"$tsfile"
printf "<context>\n\t<name>MainWindow</name>" >>$tsfile

for n in "${names[@]}"; do
	printf "\n\t<message>\n\t\t<location filename=\"../main.cpp\"/>\n\t\t<source>$n</source>\n\t\t<translation type=\"unfinished\"></translation>\n\t</message>" >>$tsfile
done

printf "\n</context>\n</TS>" >>$tsfile
