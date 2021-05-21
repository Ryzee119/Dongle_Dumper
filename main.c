#include <hal/debug.h>
#include <hal/video.h>
#include <windows.h>
#include <assert.h>
#include <usbh_lib.h>
#include <xid_driver.h>

#define ROM_NAME "D:\\dvd-dongle-rom.bin"
#define REQUEST_INFO 1
#define REQUEST_ROM  2

typedef struct __attribute__((packed)) 
{
    uint16_t version;
    uint32_t rom_size;
} xremote_info;

void xid_connection_callback(xid_dev_t *xid_dev, int status)
{

    xid_type type = usbh_xid_get_type(xid_dev);
    if (type != XREMOTE)
        return;

    debugPrint("Peeked at your dongle :) VID: %04x %04x\n", xid_dev->idVendor, xid_dev->idProduct);

    //Ref https://github.com/XboxDev/dump-dvd-kit/blob/master/dump-dvd-kit.py
    //Ref https://xboxdevwiki.net/Xbox_DVD_Movie_Playback_Kit
    int ret;
    uint32_t rx_bytes = 0;
    xremote_info *info = usbh_alloc_mem(sizeof(xremote_info));
    ret = usbh_ctrl_xfer(xid_dev->iface->udev,                                  //Device
                         REQ_TYPE_IN | REQ_TYPE_VENDOR_DEV | REQ_TYPE_TO_IFACE, //bmRequestType
                         REQUEST_INFO,                                          //bRequest
                         0x0000,                                                //wValue
                         1,                                                     //wIndex (FIXME See 1.))
                         6,                                                     //wLength
                         (uint8_t *)info,
                         &rx_bytes,
                         10                                                     //Timeout (in 10ms blocks)
    );

    if (ret != USBH_OK)
    {
        debugPrint("Error: Could not read info from dongle\n");
        assert(0);
    }

    debugPrint("Version: %04x\n", info->version);
    debugPrint("Size: %d bytes\n", info->rom_size);

    FILE* dongle_rom_file = fopen(ROM_NAME, "wb");
    if (dongle_rom_file == NULL)
    {
        debugPrint("Error: Could not open %s for write\n", ROM_NAME);
        assert(0);
    }

    debugPrint("Starting ROM dump... \n");
    uint8_t *rom_data = malloc(info->rom_size);
    if (rom_data == NULL)
    {
        debugPrint("Error: Could not allocate memory for rom data\n");
        assert(0);
    }
    int32_t remaining = info->rom_size;
    uint32_t cursor = 0;
    uint16_t max_chunk = 1024;
    uint16_t chunk_size = 0;
    uint8_t *rx_buff = usbh_alloc_mem(max_chunk); //Must be allocated from USB pool for DMA.
    while (remaining > 0)
    {
        chunk_size = remaining < max_chunk ? remaining : max_chunk;
        ret = usbh_ctrl_xfer(xid_dev->iface->udev,                                  //Device
                             REQ_TYPE_IN | REQ_TYPE_VENDOR_DEV | REQ_TYPE_TO_IFACE, //bmRequestType
                             REQUEST_ROM,                                           //bRequest
                             cursor >> 10,                                          //wValue
                             1,                                                     //wIndex (FIXME See 1.))
                             chunk_size,                                            //wLength
                             rx_buff,
                             &rx_bytes,
                             100                                                     //Timeout (in 10ms blocks)
        );

        if (ret != USBH_OK)
        {
            debugPrint("Error: Could not read rom data at pos %d for %d bytes\n", cursor >> 10, chunk_size);
            assert(0);
        }

        if (rx_bytes != chunk_size)
        {
            debugPrint("Error: Request vs received byte mismatch. %d vs %d\n", rx_bytes, chunk_size);
            assert(0);
        }

        memcpy(rom_data + cursor, rx_buff, chunk_size);
        cursor += chunk_size;
        remaining -= chunk_size;
        
    }

    debugPrint("Read %d bytes. Writing to %s\n", info->rom_size, ROM_NAME);
    if (fwrite(rom_data, 1, info->rom_size, dongle_rom_file) != info->rom_size)
    {
        debugPrint("Error: Could not write %s\n", ROM_NAME);
        assert(0);
    }
    fclose(dongle_rom_file);

    usbh_free_mem(info, sizeof(xremote_info));
    usbh_free_mem(rx_buff, max_chunk);
    free(rom_data);

    debugPrint("Done!");

    //FIXME
    //1. wIndex of one points to interface one with bInterfaceClass = 0x59., bInterfaceSubClass = 0. This is right for my Xbox dongles on hand,
    //but ideally we should search for an interface with the specified bInterfaceClass and store the interface number incase they differ?
}

int main(void)
{
    XVideoSetMode(640, 480, 32, REFRESH_DEFAULT);

    usbh_core_init();
    usbh_xid_init();
    usbh_install_xid_conn_callback(xid_connection_callback, NULL);
    debugPrint("Insert your dongle into the slot\n");

    while (1) {
        usbh_pooling_hubs();
    }

    //Never reached, but shown for clarity
    usbh_core_deinit();
    return 0;
}
