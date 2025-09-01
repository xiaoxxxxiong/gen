#!/bin/bash

nme="/usr/local/run/run"
pid=$(pidof $nme)

start(){
	if [ -n "$pid" ]; then
		echo -e "service is \e[1;32mrunning\e[0m"
	else
		echo -e "\e[1;33mstarting run service >>> $nme\e[0m"
		$nme &
		echo -e "\e[1;32mdone!!!\e[0m"
	fi
}

stop(){
	if [ -n "$pid" ]; then
        	kill -9 $pid
        	echo -e "$nme was \e[1;31mkilled!!!\e[0m"
    	fi
}
status(){
	if [ -n "$pid" ]; then
		echo -e "service is \e[1;32mrunning\e[0m"
	else
		echo -e "service is \e[1;31mnot running\e[0m"
	fi
}

case "$1" in
	start)
		start
		;;
	stop)
		stop
		;;
	status)
		status
		;;
	*)
		echo -e "\e[1;32musage: run.sh [start | stop | status]\e[0m"
esac
