# xfce4-plugin-capslocknotifier

Created for a wireless keyboard with no caps lock LED

Setup:

1.  install libraries
    apt-get install build-essential libxfce4ui-common libxfce4ui-2-0
2.  use xinput and look for your keyboard under "Virtual Core Keyboard". Use the id value here to modify mycaps.c. For example if id is 8 it will look like #define DEFAULT_DEVICE_ID "8"
1.  run the compile script
    bash make.sh
2.  copy the plug-in to correct dir
    sudo cp libmycaps.so /usr/lib/x86_64-linux-gnu/xfce4/panel-plugins/
3.  move desktop file to correct dir
    sudo cp mycaps.desktop /usr/share/xfce4/panel-plugins/
    
Usage:

available like other plugins in Panel->Panel Preferences->Items->Add New Items

To Do:

refactor code
