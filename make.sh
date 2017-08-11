#!/bin/sh
main=mycaps # Change this to your plugin's name

gcc -Wall -shared -o lib${main}.so -fPIC ${main}.c `pkg-config --cflags --libs libxfce4panel-2.0` `pkg-config --cflags --libs gtk+-3.0` `pkg-config --cflags --libs x11` `pkg-config --cflags --libs xi`
