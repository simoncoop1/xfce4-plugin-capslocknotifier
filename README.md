### xfce4-plugin-capslocknotifier

### About

Created for a wireless keyboard with no caps lock LED

### Setup:

1.  clone project:
    ````shell
        git clone https://github.com/simoncoop1/xfce4-plugin-capslocknotifier.git
    ````    
2.  install libraries:
    ````shell
    apt-get install build-essential libxfce4ui-common libxfce4ui-2-0
    ````    
3.  use command:
    ````shell
    xinput
    ````
    Look for your keyboard under "Virtual Core Keyboard". Use the id value here to modify mycaps.c. For example if id is 8 it will look like #define DEFAULT_DEVICE_ID "8"
4.  run the compile script
    ````shell
    bash make.sh
    ````
5.  copy the plug-in to correct dir:
    ````shell
    sudo cp libmycaps.so /usr/lib/x86_64-linux-gnu/xfce4/panel-plugins/
    ````
6.  move desktop file to correct dir:
    ````shell
    sudo cp mycaps.desktop /usr/share/xfce4/panel-plugins/
    ````
    
### Usage:

available like other plugins in Panel->Panel Preferences->Items->Add New Items

### To Do:

refactor code
