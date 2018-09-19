
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <artik_module.h>
#include <artik_loop.h>
#include <artik_bluetooth.h>

#define MAX_BDADDR_LEN  17
#define SCAN_TIME		10000


static artik_bluetooth_module *bt;		// Bluetooth Device

int dev_id=0;

/***********
 * This functions is used to scan the devices
 */
void print_devices(artik_bt_device *devices, int num)
{
    int i = 0;
    for (i = 0; i < num; i++) {
    	fprintf(stdout,"{");
    	fprintf(stdout,"\"id\": \"%d\",", dev_id);
    	fprintf(stdout,"\"mac\": \"%s\",", devices[i].remote_address ? devices[i].remote_address : "(null)");
    	fprintf(stdout,"\"screen\": \"%s\",", devices[i].remote_name ? devices[i].remote_name : "(null)");
    	fprintf(stdout,"\"rssi\": \"%d\",", devices[i].rssi);
    	fprintf(stdout,"\"batt\": \"INCLUIR\",");
    	fprintf(stdout,"\"initcode\": \"INCLUIR\"");
        fprintf(stdout, "}");
    }
}



static void scan_callback(artik_bt_event event, void *data, void *user_data)
{
    artik_bt_device *dev = (artik_bt_device *) data;

    if (dev_id != 0)
    	fprintf(stdout,",");

    print_devices(dev, 1);
    dev_id++;
}

static void on_timeout_callback(void *user_data)
{
    artik_loop_module *loop = (artik_loop_module *) user_data;
    loop->quit();
}



artik_error bluetooth_scan(void)
{
    artik_loop_module *loop = (artik_loop_module *)artik_request_api_module("loop");
    artik_error ret = S_OK;
    int timeout_id = 0;

    dev_id = 0;


    ret = bt->set_callback(BT_EVENT_SCAN, scan_callback, NULL);
    if (ret != S_OK)
        goto exit;

    fprintf(stdout, "{\"device\":[");		// FOR JSON FORMAT

    ret = bt->start_scan();
    if (ret != S_OK)
        goto exit;

    loop->add_timeout_callback(&timeout_id, SCAN_TIME, on_timeout_callback,
                   (void *)loop);
    loop->run();
exit:
	fprintf(stdout, "]}");		// FOR JSON FORMAT
    bt->stop_scan();
    bt->unset_callback(BT_EVENT_SCAN);
    artik_release_api_module(loop);

    return ret;
}

int main (){
	artik_error ret = S_OK;


	/**
	 * In case of bluetooth is not avaliable we send an error
	 */
    if (!artik_is_module_available(ARTIK_MODULE_BLUETOOTH)) {
        fprintf(stderr,
            "[ERROR]: Bluetooth module is not available, skipping test...\n");
        return -1;
    }

    /**
     * We select the bluetooth module
     */
    bt = (artik_bluetooth_module *)artik_request_api_module("bluetooth");
    if (!bt) {
        fprintf(stderr,
            "[ERROR]: Failed tor request bluetooth module skipping test...\n");
        return -1;
    }

    bt->init();

    ret = bluetooth_scan();
    if (ret != S_OK)
        goto exit;

    exit:
        bt->deinit();
        artik_release_api_module(bt);

        return (ret == S_OK) ? 0 : -1;

}
