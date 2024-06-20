#include <stdio.h> // for basic printf commands
#include <string.h> // for handling strings
#include "freertos/FreeRTOS.h" // for delay, mutexes, semaphores, RTOS operations
#include "freertos/task.h"
#include "esp_system.h" // for esp_init functions, esp_err_t
#include "esp_wifi.h" // for esp_wifi_init functions and Wi-Fi operations
#include "esp_log.h" // for showing logs
#include "esp_event.h" // for Wi-Fi event
#include "nvs_flash.h" // for non-volatile storage
#include "lwip/err.h" // for lightweight IP packets error handling
#include "lwip/sys.h" // for system applications for lightweight IP apps
#include "esp_netif.h" // for network interface functions
#include "driver/gpio.h" // for GPIO functions
#include "esp_https_ota.h"
#include "certs.h"
// #include "time.h"
// #include "process.h"

const char *ssid = "AndroidAP";
const char *pass = "";
int retry_num = 0;

#define LED_PIN GPIO_NUM_38 // Change this to your built-in LED GPIO pin
static const char *TAG = "wifi_event_handler"; // Tag for logging

static void wifi_event_handler(void *event_handler_arg, esp_event_base_t event_base, int32_t event_id, void *event_data) {
    if (event_id == WIFI_EVENT_STA_START) {
        ESP_LOGI(TAG, "WIFI CONNECTING....");
        esp_wifi_connect();
    } else if (event_id == WIFI_EVENT_STA_CONNECTED) {
        ESP_LOGI(TAG, "WiFi CONNECTED");
    } else if (event_id == WIFI_EVENT_STA_DISCONNECTED) {
        ESP_LOGI(TAG, "WiFi lost connection");
        if (retry_num < 5) {
            esp_wifi_connect();
            retry_num++;
            ESP_LOGI(TAG, "Retrying to Connect...");
        }
        // Turn off LED if disconnected
        gpio_set_level(LED_PIN, 0);
    } else if (event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
        char ip[16]; // Buffer to store IP address string
        esp_ip4addr_ntoa(&event->ip_info.ip, ip, sizeof(ip));
        ESP_LOGI(TAG, "Wifi got IP: %s", ip);

        // Turn on LED (Green) when connected
        gpio_set_level(LED_PIN, 1);
    }
}

void init_gpio() {
    gpio_config_t io_conf;
    // Configure the LED pin as output
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = (1ULL << LED_PIN);
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    gpio_config(&io_conf);
    // Initialize LED to off
    gpio_set_level(LED_PIN, 0);
}

void wifiConnector(const char *ssid, const char *pass) {
    // 1 - Initialization Phase
    esp_netif_init();
    esp_event_loop_create_default(); // event loop
    esp_netif_create_default_wifi_sta(); // Wi-Fi station
    wifi_init_config_t wifi_initiation = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&wifi_initiation); // Initialize Wi-Fi

    // 2 - Wi-Fi Configuration Phase
    esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, wifi_event_handler, NULL);
    esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, wifi_event_handler, NULL);

    wifi_config_t wifi_configuration = {
        .sta = {
            .ssid = "",
            .password = "",
        }
    };

    strcpy((char*)wifi_configuration.sta.ssid, ssid);
    strcpy((char*)wifi_configuration.sta.password, pass);
    esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_configuration);

    // 3 - Wi-Fi Start Phase
    esp_wifi_start();
    esp_wifi_set_mode(WIFI_MODE_STA);

    // 4 - Wi-Fi Connect Phase
    esp_wifi_connect();

    ESP_LOGI(TAG, "wifi_init_softap finished. SSID:%s  password:%s", ssid, pass);
}

esp_err_t check_wifi_status() {
    wifi_ap_record_t ap_info;
    esp_err_t ret = esp_wifi_sta_get_ap_info(&ap_info);
    if (ret == ESP_OK) {
        return ESP_OK;
    } else {
        return ESP_FAIL;
    }
}




void app_main(void) {
    nvs_flash_init();
    uint16_t count = 0;
    init_gpio(); // Initialize GPIO for LED
    while (1){
        esp_err_t status = check_wifi_status();
        if (status == ESP_FAIL){
            wifiConnector(ssid, pass);
            ESP_LOGI(TAG, "Reconecting");
        }else{
            ESP_LOGI(TAG,"Connected");
        };
        sleep(1);
        if (count<= 10){
            count = count + 1;
            continue;
        }

        count = 0;

        ESP_LOGI(TAG,"After continue\n");
        // After the Wi-Fi connection is established, perform the OTA update
        esp_https_ota_config_t ota_config = {
            .http_config = &(esp_http_client_config_t) {
                .url = "https://raw.githubusercontent.com/VentsiIliev/ESP32OTA/main/test2.bin",
                .cert_pem = server_cert_pem_start,
                .skip_cert_common_name_check = true,  // Disable common name check
                // .skip_cert_not_after_check = true,
            },
        };
            

        ESP_LOGI(TAG, "Starting OTA update");  // Log message before OTA update
        esp_err_t ret = esp_https_ota(&ota_config);
        // esp_err_t ret = esp_https_ota(&config);

        ESP_LOGI(TAG, "OTA update status: %d", ret);  // Log the return value of esp_https_ota

        if (ret == ESP_OK) {
            ESP_LOGI(TAG, "Firmware upgraded");
            esp_restart();
        } else {
            ESP_LOGI(TAG, "Firmware upgrade failed");
        }

        // sleep(5);
    }
}
