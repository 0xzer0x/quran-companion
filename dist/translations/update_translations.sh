#!/bin/bash

set -e

#COLORS
CDEF="\033[0m"      # default color
CCIN="\033[0;36m"   # info color
CGSC="\033[0;32m"   # success color
CRER="\033[0;31m"   # error color
CWAR="\033[0;33m"   # waring color
b_CDEF="\033[1;37m" # bold default color
b_CCIN="\033[1;36m" # bold info color
b_CGSC="\033[1;32m" # bold success color
b_CRER="\033[1;31m" # bold error color
b_CWAR="\033[1;33m" # bold warning color

# echo like with flag type and display message colors
prompt() {
	case ${1} in
	"-s" | "--success")
		echo -ne "${b_CGSC}${@/-s/}${CDEF}"
		;; # print success message
	"-e" | "--error")
		echo -ne "${b_CRER}${@/-e/}${CDEF}"
		;; # print error message
	"-w" | "--warning")
		echo -ne "${b_CWAR}${@/-w/}${CDEF}"
		;; # print warning message
	"-i" | "--info")
		echo -ne "${b_CCIN}${@/-i/}${CDEF}"
		;; # print info message
	*)
		echo -ne "$@"
		;;
	esac
}

SCRIPT_DIR=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" &>/dev/null && pwd)
languages=(ar tr)

prompt -s "----- Generating reciters TS file -----\n"
$SCRIPT_DIR/reciters.sh

prompt -s "----- Generating shortcuts TS file -----\n"
$SCRIPT_DIR/shortcuts.sh

prompt -s "----- Updating template TS file -----\n"
lupdate -recursive -no-obsolete $SCRIPT_DIR/../../src -ts qc_src.ts
lconvert -i $SCRIPT_DIR/shortcuts.ts $SCRIPT_DIR/reciters.ts $SCRIPT_DIR/qc_src.ts -no-obsolete -o qc_template.ts

for lang in "${languages[@]}"; do
	if [[ -f "$SCRIPT_DIR/qc_$lang.ts" ]]; then
		prompt -s "----- Updating ${lang^^} translation file -----\n"
		lconvert -i $SCRIPT_DIR/qc_template.ts $SCRIPT_DIR/qc_$lang.ts -no-obsolete -o combined_$lang.ts
		mv $SCRIPT_DIR/combined_$lang.ts $SCRIPT_DIR/qc_$lang.ts
	fi
done
