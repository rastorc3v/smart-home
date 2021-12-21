#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "esp_log.h"
#include "string.h"
#include "cJSON.h"

/**
 * @brief Get the nvs data by provided key
 * type_key - is what should be returned in case of
 * error (or NOT_INITIALIZED_YET).
 * 0 - "object"
 * 1- "array" - //default//
 * @param key 
 * @param type_key 
 * @return cJSON* 
 */
cJSON * get_nvs(char * key, int type_key) {
    nvs_handle_t my_handle;
    esp_err_t err = nvs_open("storage", NVS_READONLY, &my_handle);
    if (err == ESP_OK) {
        size_t required_size;
        nvs_get_str(my_handle, key, NULL, &required_size);
        char * data = malloc(required_size);
        err = nvs_get_str(my_handle, key, data, &required_size);
        if (err == ESP_OK) {
            cJSON * result = cJSON_Parse(data);
            nvs_close(my_handle); 
            return result; 
        }
        ESP_LOGI("tools", "Not OK NVS result");
        ESP_LOGI("tools", "The value is not initialized yet!");
        ESP_LOGI("tools", "Error (%s)\n", esp_err_to_name(err));
    }
    nvs_close(my_handle);
    switch (type_key)
    {
    case 0:
        return cJSON_CreateObject();
        break;
    case 1:
        return cJSON_CreateArray();
    default:
        break;
    }
    return cJSON_CreateArray();
}

int get_device_max_id(cJSON * devices_list) {
    int array_size = cJSON_GetArraySize(devices_list);
    int max_id = -1;
    int id;
    for (int i=0; i<array_size; i++) {
        id = cJSON_GetObjectItem(cJSON_GetArrayItem(devices_list, i), "id")->valueint;
        if (id > max_id) {
            max_id = id;
        }
    }
    return max_id;
}

int get_device_index_by_id(cJSON * devices_list, int id) {
    int array_size = cJSON_GetArraySize(devices_list);
    int _id;
    for (int i=0; i<array_size; i++) {
        _id = cJSON_GetObjectItem(cJSON_GetArrayItem(devices_list, i), "id")->valueint;
        if (_id == id) {
            return i;
        }
    }
    return -1;
}

bool add_to_array_nvs(char * key, char * new_object) {
    nvs_handle_t my_handle;
    esp_err_t err = nvs_open("storage", NVS_READWRITE, &my_handle);
    if (err == ESP_OK) {
        cJSON * data_json = get_nvs(key, 1);
        cJSON * new_object_json = cJSON_Parse(new_object);
        cJSON_AddNumberToObject(new_object_json, "id", get_device_max_id(data_json) + 1);
        cJSON_AddItemToArray(data_json, new_object_json);
        
        char * result = cJSON_PrintUnformatted(data_json);

        err = nvs_set_str(my_handle, key, result);
        nvs_commit(my_handle);
        nvs_close(my_handle);
        return true;
    }
    nvs_close(my_handle);
    return false;
}

bool delete_from_array(char * key, int id) {
    nvs_handle_t my_handle;
    esp_err_t err = nvs_open("storage", NVS_READWRITE, &my_handle);
    if (err == ESP_OK) {
        cJSON * data_json = get_nvs(key, 1);

        cJSON_DeleteItemFromArray(data_json, get_device_index_by_id(data_json, id));

        char * result = cJSON_PrintUnformatted(data_json);

        err = nvs_set_str(my_handle, key, result);
        nvs_commit(my_handle);
        nvs_close(my_handle);
        return true;
    }
    nvs_close(my_handle);
    return false;
}

cJSON * edit_object_in_array(char * key, int id, char * new_obj_version) {
    nvs_handle_t my_handle;
    esp_err_t err = nvs_open("storage", NVS_READWRITE, &my_handle);
    if (err == ESP_OK) {
        cJSON * data_json = get_nvs(key, 1);
        cJSON * new_obj_version_json = cJSON_Parse(new_obj_version);

        cJSON_ReplaceItemInArray(data_json, get_device_index_by_id(data_json, id), new_obj_version_json);

        char * result = cJSON_PrintUnformatted(data_json);
        err = nvs_set_str(my_handle, key, result);
        nvs_commit(my_handle);
        nvs_close(my_handle);
        return new_obj_version_json;
    }
    nvs_close(my_handle);
    cJSON * error_obj = cJSON_CreateObject();
    cJSON_AddBoolToObject(error_obj, "isError", true);
    return error_obj;
}