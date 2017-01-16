#!/bin/bash

case "$1" in

	run)

		
		
		#oat posisock pub pos -e 'tcp://*:5555' &
		oat posisock std  pos&
		oat posidet planarled raw pos -T [250,255] -e 12 -d 17  --tune &
		oat view frame raw &
		oat frameserve usb raw -r 30 -s 30 -g 16  -C GREY
	

		#sleep 1
		#oat frameserve file RAW -f mouse.mpg -c config.toml video   
		;;

	clean)

		oat clean raw filt pos final
		;;

	*)
		echo $"Usage: $0 {run|clean}"
		exit 1

esac
