#!/bin/bash
# script para compilacion de aplicacion de cgi1
# 
# Atte. Guillermo Cherencio
#
gcc -Wall -o ../cgi-bin/monitor.cgi monitor.c cgi.c -I ../include

