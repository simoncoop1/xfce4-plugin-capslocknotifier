#include <libxfce4panel/xfce-panel-plugin.h>
#include <X11/Xlib.h>

#include "X11/extensions/XInput.h"
#include <stdio.h>
#include <stdlib.h>

#ifndef EXIT_SUCCESS
#define EXIT_SUCCESS 1
#endif
#ifndef EXIT_FAILURE
#define EXIT_FAILURE 0
#endif

#include <ctype.h>
#include <string.h>
#include <time.h>

#define PAUSE_TIME 150000000  //IN NANO SECONDS
#define MAX_WAIT 5000000000  //HOW TO TO WAIT FOR A
#define DEFAULT_DEVICE_ID "8"  //UPDATE FOR lEDS

//#include "fileoperations.h"

gboolean caps;
//char* g_device_id = DEFAULT_DEVICE_ID;

static void constructor(XfcePanelPlugin *plugin);
XFCE_PANEL_PLUGIN_REGISTER_INTERNAL(constructor);
GtkWidget *label;



/*  queue this method in GTK main thread */
int updateUI(gpointer data){
  //gboolean *caps = data;

  if(caps){
  gtk_label_set_text (GTK_LABEL(label),"ON");
  }
  else{
  gtk_label_set_text (GTK_LABEL(label),"OFF");
  }

  return 0;
}

/*   worker thread  */
/*void *Loop(void *void_ptr){
  Display *dpy;
  XKeyboardState values;
  int updateInterval =1;
  gboolean caps = FALSE;

  dpy = XOpenDisplay(NULL);
  gdk_threads_add_idle(updateUI,&caps);

  while(TRUE){

    XGetKeyboardControl(dpy, &values);
    unsigned long *led = &values.led_mask;
    //printf("the led is: %ld\n",*led);

    if(*led &  1){
      //Caps lock ON
      if(caps==FALSE){
        caps=TRUE;
        gdk_threads_add_idle(updateUI,&caps);

        }
    }
    else{
      //Caps lock OFF
      if(caps==TRUE){
        caps=FALSE;
        gdk_threads_add_idle(updateUI,&caps);

        }
    }

    sleep(updateInterval);

  }

  return 0;
}
*/

#define INVALID_EVENT_TYPE	-1

static int           motion_type = INVALID_EVENT_TYPE;
static int           button_press_type = INVALID_EVENT_TYPE;
static int           button_release_type = INVALID_EVENT_TYPE;
static int           key_press_type = INVALID_EVENT_TYPE;
static int           key_release_type = INVALID_EVENT_TYPE;
static int           proximity_in_type = INVALID_EVENT_TYPE;
static int           proximity_out_type = INVALID_EVENT_TYPE;


XDeviceInfo*
find_device_info(Display	*display,
		 char		*name,
		 Bool		only_extended)
{
    XDeviceInfo	*devices;
    XDeviceInfo *found = NULL;
    int		loop;
    int		num_devices;
    int		len = strlen(name);
    Bool	is_id = True;
    XID		id = (XID)-1;

    for(loop=0; loop<len; loop++) {
	if (!isdigit(name[loop])) {
	    is_id = False;
	    break;
	}
    }

    if (is_id) {
	id = atoi(name);
    }

    devices = XListInputDevices(display, &num_devices);

    for(loop=0; loop<num_devices; loop++) {
	if ((!only_extended || (devices[loop].use >= IsXExtensionDevice)) &&
	    ((!is_id && strcmp(devices[loop].name, name) == 0) ||
	     (is_id && devices[loop].id == id))) {
	    if (found) {
	        fprintf(stderr,
	                "Warning: There are multiple devices named '%s'.\n"
	                "To ensure the correct one is selected, please use "
	                "the device ID instead.\n\n", name);
		return NULL;
	    } else {
		found = &devices[loop];
	    }
	}
    }
    return found;
}

int ReadSettings(){
	//if(file_exist("~/.mycaps.config")){
	
  //}

  //struct stat   buffer;
  //if(stat ("~/.mycaps.config", &buffer) == 0){
  //}

  //check


  return 1;
}

static int
register_events(Display		*dpy,
		XDeviceInfo	*info,
		char		*dev_name,
		Bool		handle_proximity)
{
    int			number = 0;	/* number of events registered */
    XEventClass		event_list[7];
    int			i;
    XDevice		*device;
    Window		root_win;
    unsigned long	screen;
    XInputClassInfo	*ip;

    screen = DefaultScreen(dpy);
    root_win = RootWindow(dpy, screen);

    device = XOpenDevice(dpy, info->id);

    if (!device) {
	fprintf(stderr, "unable to open device '%s'\n", dev_name);
	return 0;
    }

    if (device->num_classes > 0) {
	for (ip = device->classes, i=0; i<info->num_classes; ip++, i++) {
	    switch (ip->input_class) {
	    case KeyClass:
		DeviceKeyPress(device, key_press_type, event_list[number]); number++;
		DeviceKeyRelease(device, key_release_type, event_list[number]); number++;
		break;

	    case ButtonClass:
		DeviceButtonPress(device, button_press_type, event_list[number]); number++;
		DeviceButtonRelease(device, button_release_type, event_list[number]); number++;
		break;

	    case ValuatorClass:
		DeviceMotionNotify(device, motion_type, event_list[number]); number++;
		if (handle_proximity) {
		    ProximityIn(device, proximity_in_type, event_list[number]); number++;
		    ProximityOut(device, proximity_out_type, event_list[number]); number++;
		}
		break;

	    default:
		fprintf(stderr, "unknown class\n");
		break;
	    }
	}

	if (XSelectExtensionEvent(dpy, root_win, event_list, number)) {
	    fprintf(stderr, "error selecting extended events\n");
	    return 0;
	}
    }
    return number;
}

/*
  initialise gui from the initial value of LEDS
*/
void InitLedsAndGui(Display *dpy){
  XKeyboardState values;
  XGetKeyboardControl(dpy, &values);
  unsigned long *led = &values.led_mask;

  if(*led &  1){
  //Caps lock ON
  caps=TRUE;
  gdk_threads_add_idle(updateUI, NULL);
  }
  else{
  //Caps lock OFF
  caps = FALSE;
  gdk_threads_add_idle(updateUI,NULL);
  }

}

/*
  get the current status of leds and if they are different
  to the state of the gui, then queue gui update
*/
void LedsAndGui(Display *dpy)
{
  XKeyboardState values;
  XGetKeyboardControl(dpy, &values);
  unsigned long *led = &values.led_mask;
  unsigned long  led1 = *led & 1;

  for(int i = PAUSE_TIME; i < 5000000000; i+=PAUSE_TIME){
    struct timespec mytimespec = { .tv_sec=0, .tv_nsec=PAUSE_TIME };
    nanosleep(&mytimespec,NULL);
    XGetKeyboardControl(dpy, &values);
    led = &values.led_mask;

    if(*led !=  led1){
      break;
    }
  }

  if(*led &  1){
  //Caps lock ON
  caps=TRUE;
  gdk_threads_add_idle(updateUI, NULL);
  }
  else{
  //Caps lock OFF
  caps = FALSE;
  gdk_threads_add_idle(updateUI,NULL);
  }

}

static void
print_events(Display	*dpy)
{
    XEvent        Event;

    //setvbuf(stdout, NULL, _IOLBF, 0);

    InitLedsAndGui(dpy);

    while(1) {
	XNextEvent(dpy, &Event);

	if (Event.type == motion_type) {
	    int	loop;
	    XDeviceMotionEvent *motion = (XDeviceMotionEvent *) &Event;

	    printf("motion ");

	    for(loop=0; loop<motion->axes_count; loop++) {
		printf("a[%d]=%d ", motion->first_axis + loop, motion->axis_data[loop]);
	    }
	    printf("\n");
	} else if ((Event.type == button_press_type) ||
		   (Event.type == button_release_type)) {
	    int	loop;
	    XDeviceButtonEvent *button = (XDeviceButtonEvent *) &Event;

	    printf("button %s %d ", (Event.type == button_release_type) ? "release" : "press  ",
		   button->button);

	    for(loop=0; loop<button->axes_count; loop++) {
		printf("a[%d]=%d ", button->first_axis + loop, button->axis_data[loop]);
	    }
	    printf("\n");
	} else if(Event.type == key_release_type){
        } else if ((Event.type == key_press_type) /*||
		   (Event.type == key_release_type)*/) {

	    XDeviceKeyEvent *key = (XDeviceKeyEvent *) &Event;

            if(key->keycode ==  66){
              XKeyboardState values;
              XGetKeyboardControl(dpy, &values);
              unsigned long *led = &values.led_mask;
              unsigned long  led1 = *led & 1;

	     for(int i = PAUSE_TIME; i < 5000000000; i+=PAUSE_TIME)
             {
	        struct timespec mytimespec = { .tv_sec=0, .tv_nsec=PAUSE_TIME };
                nanosleep(&mytimespec,NULL);
                XGetKeyboardControl(dpy, &values);
                led = &values.led_mask;

                if(*led !=  led1)
                {
                  break;
                }
             }



//              *led & 1;
//              for(int i = 0; i < 5


              if(*led &  1){
              //Caps lock ON
                caps=TRUE;
                gdk_threads_add_idle(updateUI, NULL);
              }
              else{
                //Caps lock OFF
                caps = FALSE;
                gdk_threads_add_idle(updateUI,NULL);

              }

            }
	} else if ((Event.type == proximity_out_type) ||
		   (Event.type == proximity_in_type)) {
	    int	loop;
	    XProximityNotifyEvent *prox = (XProximityNotifyEvent *) &Event;

	    printf("proximity %s ", (Event.type == proximity_in_type) ? "in " : "out");

	    for(loop=0; loop<prox->axes_count; loop++) {
		printf("a[%d]=%d ", prox->first_axis + loop, prox->axis_data[loop]);
	    }
	    printf("\n");
	}
	else {
	    printf("what's that %d\n", Event.type);
	}
    }
}


int
test(Display	*display,
     int	argc,
     char	*argv[],
     char	*name,
     char	*desc)
{
    XDeviceInfo		*info;

    if (argc != 1 && argc != 2) {
	fprintf(stderr, "usage: xinput %s %s\n", name, desc);
	return EXIT_FAILURE;
    } else {
	Bool	handle_proximity = False;
	int	idx = 0;

	if (argc == 2) {
	    if (strcmp("-proximity", argv[0]) != 0) {
		fprintf(stderr, "usage: xinput %s %s\n", name, desc);
		return EXIT_FAILURE;
	    }
	    handle_proximity = 1;
	    idx = 1;
	}

	info = find_device_info(display, argv[idx], True);

	if (!info) {
	    fprintf(stderr, "unable to find device '%s'\n", argv[idx]);
	    return EXIT_FAILURE;
	} else {
	    if (register_events(display, info, argv[idx], handle_proximity)) {
		print_events(display);
	    }
	    else {
		fprintf(stderr, "no event registered...\n");
		return EXIT_FAILURE;
	    }
	}
    }
    return EXIT_FAILURE;
}


void *Loop(void *void_ptr){

  Display *dpy;
  XDeviceInfo *info;

  dpy = XOpenDisplay(NULL);

  info = find_device_info(dpy, DEFAULT_DEVICE_ID , True);

	if (!info) {
	    fprintf(stderr, "unable to find device '%s'\n", DEFAULT_DEVICE_ID);
	    //return EXIT_FAILURE;
	} else {
	    if (register_events(dpy, info, DEFAULT_DEVICE_ID, False)) {
		print_events(dpy);
	    }
	    else {
		fprintf(stderr, "no event registered...\n");
		//return EXIT_FAILURE;
	    }
	}

  return 0;

}

static void
constructor(XfcePanelPlugin *plugin) {
  GtkWidget *my_label;

  // Use any GTK_CONTAINER you wish, it will be packed into the XfcePanelPlugin (plugin) variable.
  GtkWidget *scroll_win = gtk_scrolled_window_new(NULL, NULL);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW(scroll_win),GTK_POLICY_NEVER,GTK_POLICY_NEVER);
  //gtk_scrolled_window_set_max_content_width(GTK_SCROLLED_WINDOW(scroll_win),50);

  label = gtk_label_new ("LABEL");
  my_label = label;

  /* Now program as if you were creating any old GTK application. */
  pthread_t tid1;
  pthread_create(&tid1,NULL,Loop,NULL);

  // Pack your container into the plugin's container.
  gtk_container_add (GTK_CONTAINER (scroll_win), my_label);
  gtk_container_add(GTK_CONTAINER(plugin), scroll_win);

  // Run these commands so GTK shows your widgets.
  gtk_widget_show_all(scroll_win);
  xfce_panel_plugin_set_expand (XFCE_PANEL_PLUGIN(plugin), FALSE); // nothing to change here

}

