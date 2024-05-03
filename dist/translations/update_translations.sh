#!/bin/bash

set -e

#COLORS
CDEF="\033[0m"      # default color
b_CCIN="\033[1;36m" # bold info color
b_CGSC="\033[1;32m" # bold success color
b_CRER="\033[1;31m" # bold error color
b_CWAR="\033[1;33m" # bold warning color

# echo like with flag type and display message colors
prompt() {
	case ${1} in
	"-s" | "--success")
		echo -ne "${b_CGSC}${*/-s/}${CDEF}"
		;; # print success message
	"-e" | "--error")
		echo -ne "${b_CRER}${*/-e/}${CDEF}"
		;; # print error message
	"-w" | "--warning")
		echo -ne "${b_CWAR}${*/-w/}${CDEF}"
		;; # print warning message
	"-i" | "--info")
		echo -ne "${b_CCIN}${*/-i/}${CDEF}"
		;; # print info message
	*)
		echo -ne "$@"
		;;
	esac
}

SCRIPT_DIR=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" &>/dev/null && pwd)
prompt -s "----- Generating reciters TS file -----\n"
"$SCRIPT_DIR/reciters.sh"

prompt -s "----- Generating shortcuts TS file -----\n"
"$SCRIPT_DIR/shortcuts.sh"

prompt -s "----- Generating tafasir TS file -----\n"
"$SCRIPT_DIR/tafasir.sh"

prompt -s "----- Generating sources TS file -----\n"
~/Qt/6.6.1/gcc_64/bin/lupdate -recursive -no-obsolete "$SCRIPT_DIR/../../src" -ts "$SCRIPT_DIR/generated/sources.ts"

prompt -s "----- Updating template TS file -----\n"
declare -a generated
for f in "$SCRIPT_DIR/generated/"*; do
	generated=("${generated[@]}" "$f")
done
~/Qt/6.6.1/gcc_64/bin/lconvert -i "${generated[@]}" -no-obsolete -o template.ts
