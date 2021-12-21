#include "esp_netif.h"
#include "esp_log.h"
/**
 * {
 *    deviceId: number,
 *    status: bool
 *    ip: char *
 * }
 */

/**
 * Send request to each avaliable ip address in the network
 * and update information about devices
 */
// void devices_init() {

// }

void get_devices_ip_adresses() {
    esp_netif_t* netif=NULL;
    netif = esp_netif_get_handle_from_ifkey("WIFI_STA_DEF");
    esp_netif_ip_info_t ip_info;
    esp_netif_get_ip_info(netif, &ip_info);

    //cJSON * config = cJSON_CreateObject();
    //cJSON_AddStringToObject(config, "ip", )
    // const uint8_t * ipkek = esp_ip4_addr4(ip);
    // ESP_LOGI("tools", "%u", ipkek);
    ESP_LOGI("tools", "%u", esp_ip4_addr_get_byte(ip_info.gw, 3));
    // ESP_LOGI("tools", "%u", esp_ip4_addr_get_byte(ip_info.netmask, 3));
}