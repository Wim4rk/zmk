#!/usr/bin/env bash
# Bygger ZMK för Aardvark 68 och flashar över USB via STM32:ans inbyggda
# DFU-bootloader, utan att öppna lådan.
#
# Användning: ./flasha_aardvark.sh
#   1. Redigera app/boards/wim4rk/aardvark_68/aardvark_68_1_0_0_default.keymap
#   2. Kör skriptet. När det ber om det: håll Caps + Fn och tryck Enter
#      (&bootloader i adjust-lagret).
set -euo pipefail

ZMK="$(cd "$(dirname "$0")" && pwd)"
KEYMAP="$ZMK/app/boards/wim4rk/aardvark_68/aardvark_68_1_0_0_default.keymap"
BUILD="$ZMK/build/aardvark_68"
BIN="$BUILD/zephyr/zmk.bin"
DFU_ID="0483:df11"

# BOOT0-knappen (SW1) sitter inne i lådan. Utan &bootloader i keymappen
# går det inte att flasha igen över USB, bara via SWD.
if ! grep -v '^\s*//' "$KEYMAP" | grep -q '&bootloader'; then
	echo "Keymappen saknar &bootloader. Flashar inte, då skulle du låsa dig ute." >&2
	exit 1
fi

source "$ZMK/.venv/bin/activate"
west build -s "$ZMK/app" -d "$BUILD" -b aardvark_68@1.0.0/stm32wb55xx/zmk -- \
	-DEXTRA_CONF_FILE="$ZMK/app/boards/wim4rk/aardvark_68/aardvark_68_1_0_0.conf"

echo
echo "Håll Caps + Fn och tryck Enter för att starta bootloadern..."
for _ in $(seq 60); do
	dfu-util -l -d "$DFU_ID" 2>/dev/null | grep -q "Found DFU" && break
	sleep 1
done
if ! dfu-util -l -d "$DFU_ID" 2>/dev/null | grep -q "Found DFU"; then
	echo "Hittade ingen DFU-enhet ($DFU_ID) inom 60 sekunder." >&2
	exit 1
fi

dfu-util -d "$DFU_ID" -a 0 -s 0x08000000:leave -D "$BIN"
echo "Klart."
