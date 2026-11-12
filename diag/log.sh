#!/bin/sh
# Log série horodaté de l'ESP dans diag/ ; reprend après reset ou débranchement.
PORT=/dev/serial/by-id/usb-Adafruit_MatrixPortal_ESP32-S3_a4:cb:8f:af:e2:fc-if00
LOG="$(dirname "$0")/$(date +%y%m%d-%H%M%S).log"
while true; do
	if [ -e "$PORT" ] && stty -F "$PORT" raw -echo igncr 115200; then
		cat "$PORT" | while IFS= read -r line; do echo "$(date '+%F %T') $line"; done >> "$LOG"
	fi
	sleep 1
done
