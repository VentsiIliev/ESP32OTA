// #include "WiFi_Connection.h"
// #include "esp_log.h"
// #include "driver/gpio.h"

// #define LED_PIN GPIO_NUM_38
// static const char *TAG = "wifi_event_handler";

// void wifi_event_handler(void *event_handler_arg, esp_event_base_t event_base, int32_t event_id, void *event_data) {
//     // Your existing event handling code here
//     if (event_id == WIFI_EVENT_STA_START) {
//         ESP_LOGI(TAG, "WIFI CONNECTING....");
//         esp_wifi_connect();
//     } else if (event_id == WIFI_EVENT_STA_CONNECTED) {
//         ESP_LOGI(TAG, "WiFi CONNECTED");
//     } else if (event_id == WIFI_EVENT_STA_DISCONNECTED) {
//         ESP_LOGI(TAG, "WiFi lost connection");
//         if (retry_num < 5) {
//             esp_wifi_connect();
//             retry_num++;
//             ESP_LOGI(TAG, "Retrying to Connect...");
//         }
//         gpio_set_level(LED_PIN, 0);
//     } else if (event_id == IP_EVENT_STA_GOT_IP) {
//         ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
//         char ip[16];
//         esp_ip4addr_ntoa(&event->ip_info.ip, ip, sizeof(ip));
//         ESP_LOGI(TAG, "Wifi got IP: %s", ip);
//         gpio_set_level(LED_PIN, 1);
//     }
// }

// void init_gpio(void) {
//     gpio_config_t io_conf;
//     io_conf.intr_type = GPIO_INTR_DISABLE;
//     io_conf.mode = GPIO_MODE_OUTPUT;
//     io_conf.pin_bit_mask = (1ULL << LED_PIN);
//     io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
//     io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
//     gpio_config(&io_conf);
//     gpio_set_level(LED_PIN, 0);
// }

// void wifiConnector(const char *ssid, const char *pass) {
//     esp_netif_init();
//     esp_event_loop_create_default();
//     esp_netif_create_default_wifi_sta();
//     wifi_init_config_t wifi_initiation = WIFI_INIT_CONFIG_DEFAULT();
//     esp_wifi_init(&wifi_initiation);

//     esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, wifi_event_handler, NULL);
//     esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, wifi_event_handler, NULL);

//     wifi_config_t wifi_configuration = {
//         .sta = {
//             .ssid = "",
//             .password = "",
//         }
//     };

//     strcpy((char*)wifi_configuration.sta.ssid, ssid);
//     strcpy((char*)wifi_configuration.sta.password, pass);
//     esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_configuration);

//     esp_wifi_start();
//     esp_wifi_set_mode(WIFI_MODE_STA);

//     esp_wifi_connect();

//     ESP_LOGI(TAG, "wifi_init_softap finished. SSID:%s  password:%s", ssid, pass);
// }

// esp_err_t check_wifi_status(void) {
//     wifi_ap_record_t ap_info;
//     esp_err_t ret = esp_wifi_sta_get_ap_info(&ap_info);
//     if (ret == ESP_OK) {
//         return ESP_OK;
//     } else {
//         return ESP_FAIL;
//     }
// }
