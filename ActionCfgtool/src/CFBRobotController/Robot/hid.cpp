#ifdef Q_OS_WIN



#include <windows.h>

#ifndef _NTDEF_
typedef LONG NTSTATUS;
#endif

#ifdef __MINGW32__
#include <ntdef.h>
#include <winbase.h>
#endif

#ifdef __CYGWIN__
#include <ntdef.h>
#define _wcsdup wcsdup
#endif



#ifdef __cplusplus
extern "C" {
#endif
    #include <setupapi.h>
    #include <winioctl.h>
    #ifdef HIDAPI_USE_DDK
        #include <hidsdi.h>
    #endif

    
    #define HID_OUT_CTL_CODE(id)  \
        CTL_CODE(FILE_DEVICE_KEYBOARD, (id), METHOD_OUT_DIRECT, FILE_ANY_ACCESS)
    #define IOCTL_HID_GET_FEATURE                   HID_OUT_CTL_CODE(100)

#ifdef __cplusplus
} 
#endif

#include <stdio.h>
#include <stdlib.h>


#include "hidapi.h"

#ifdef _MSC_VER
    
    #pragma warning(disable:4996)
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifndef HIDAPI_USE_DDK
    
    typedef struct _HIDD_ATTRIBUTES{
        ULONG Size;
        USHORT VendorID;
        USHORT ProductID;
        USHORT VersionNumber;
    } HIDD_ATTRIBUTES, *PHIDD_ATTRIBUTES;

    typedef USHORT USAGE;
    typedef struct _HIDP_CAPS {
        USAGE Usage;
        USAGE UsagePage;
        USHORT InputReportByteLength;
        USHORT OutputReportByteLength;
        USHORT FeatureReportByteLength;
        USHORT Reserved[17];
        USHORT fields_not_used_by_hidapi[10];
    } HIDP_CAPS, *PHIDP_CAPS;
    typedef void* PHIDP_PREPARSED_DATA;
    #define HIDP_STATUS_SUCCESS 0x110000

    typedef BOOLEAN (__stdcall *HidD_GetAttributes_)(HANDLE device, PHIDD_ATTRIBUTES attrib);
    typedef BOOLEAN (__stdcall *HidD_GetSerialNumberString_)(HANDLE device, PVOID buffer, ULONG buffer_len);
    typedef BOOLEAN (__stdcall *HidD_GetManufacturerString_)(HANDLE handle, PVOID buffer, ULONG buffer_len);
    typedef BOOLEAN (__stdcall *HidD_GetProductString_)(HANDLE handle, PVOID buffer, ULONG buffer_len);
    typedef BOOLEAN (__stdcall *HidD_SetFeature_)(HANDLE handle, PVOID data, ULONG length);
    typedef BOOLEAN (__stdcall *HidD_GetFeature_)(HANDLE handle, PVOID data, ULONG length);
    typedef BOOLEAN (__stdcall *HidD_GetIndexedString_)(HANDLE handle, ULONG string_index, PVOID buffer, ULONG buffer_len);
    typedef BOOLEAN (__stdcall *HidD_GetPreparsedData_)(HANDLE handle, PHIDP_PREPARSED_DATA *preparsed_data);
    typedef BOOLEAN (__stdcall *HidD_FreePreparsedData_)(PHIDP_PREPARSED_DATA preparsed_data);
    typedef NTSTATUS (__stdcall *HidP_GetCaps_)(PHIDP_PREPARSED_DATA preparsed_data, HIDP_CAPS *caps);
    typedef BOOLEAN (__stdcall *HidD_SetNumInputBuffers_)(HANDLE handle, ULONG number_buffers);

    static HidD_GetAttributes_ HidD_GetAttributes;
    static HidD_GetSerialNumberString_ HidD_GetSerialNumberString;
    static HidD_GetManufacturerString_ HidD_GetManufacturerString;
    static HidD_GetProductString_ HidD_GetProductString;
    static HidD_SetFeature_ HidD_SetFeature;
    static HidD_GetFeature_ HidD_GetFeature;
    static HidD_GetIndexedString_ HidD_GetIndexedString;
    static HidD_GetPreparsedData_ HidD_GetPreparsedData;
    static HidD_FreePreparsedData_ HidD_FreePreparsedData;
    static HidP_GetCaps_ HidP_GetCaps;
    static HidD_SetNumInputBuffers_ HidD_SetNumInputBuffers;

    static HMODULE lib_handle = NULL;
    static BOOLEAN initialized = FALSE;
#endif 

struct hid_device_ {
        HANDLE device_handle;
        BOOL blocking;
        USHORT output_report_length;
        size_t input_report_length;
        void *last_error_str;
        DWORD last_error_num;
        BOOL read_pending;
        char *read_buf;
        OVERLAPPED ol;
};

static hid_device *new_hid_device()
{
    hid_device *dev = (hid_device*) calloc(1, sizeof(hid_device));
    dev->device_handle = INVALID_HANDLE_VALUE;
    dev->blocking = TRUE;
    dev->output_report_length = 0;
    dev->input_report_length = 0;
    dev->last_error_str = NULL;
    dev->last_error_num = 0;
    dev->read_pending = FALSE;
    dev->read_buf = NULL;
    memset(&dev->ol, 0, sizeof(dev->ol));
    dev->ol.hEvent = CreateEvent(NULL, FALSE, FALSE , NULL);

    return dev;
}

static void free_hid_device(hid_device *dev)
{
    CloseHandle(dev->ol.hEvent);
    CloseHandle(dev->device_handle);
    LocalFree(dev->last_error_str);
    free(dev->read_buf);
    free(dev);
}

static void register_error(hid_device *device, const char *op)
{
#if 0
    WCHAR *ptr = NULL, *msg = NULL;

    FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER |
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        GetLastError(),
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPWSTR)&msg, 0,
        NULL);

    
    ptr = msg;
    while (*ptr) {
        if (*ptr == '\r') {
            *ptr = 0x0000;
            break;
        }
        ptr++;
    }

    
    if(device->last_error_str)
    {
        LocalFree(device->last_error_str);
        device->last_error_str = NULL;
    }
    device->last_error_str = msg;
#endif
}

#ifndef HIDAPI_USE_DDK
static int lookup_functions()
{
    lib_handle = LoadLibraryA("hid.dll");
    if (lib_handle) {
#define RESOLVE(x) x = (x##_)GetProcAddress(lib_handle, #x); if (!x) return -1;
        RESOLVE(HidD_GetAttributes);
        RESOLVE(HidD_GetSerialNumberString);
        RESOLVE(HidD_GetManufacturerString);
        RESOLVE(HidD_GetProductString);
        RESOLVE(HidD_SetFeature);
        RESOLVE(HidD_GetFeature);
        RESOLVE(HidD_GetIndexedString);
        RESOLVE(HidD_GetPreparsedData);
        RESOLVE(HidD_FreePreparsedData);
        RESOLVE(HidP_GetCaps);
        RESOLVE(HidD_SetNumInputBuffers);
#undef RESOLVE
    }
    else
        return -1;

    return 0;
}
#endif

static HANDLE open_device(const char *path, BOOL enumerate)
{
    HANDLE handle;
    DWORD desired_access = (enumerate)? 0: (GENERIC_WRITE | GENERIC_READ);
    DWORD share_mode = (enumerate)?
                          FILE_SHARE_READ|FILE_SHARE_WRITE:
                          FILE_SHARE_READ;

    handle = CreateFileA(path,
        desired_access,
        share_mode,
        NULL,
        OPEN_EXISTING,
        FILE_FLAG_OVERLAPPED,
        0);

    return handle;
}

int HID_API_EXPORT hid_init(void)
{
#ifndef HIDAPI_USE_DDK
    if (!initialized) {
        if (lookup_functions() < 0) {
            hid_exit();
            return -1;
        }
        initialized = TRUE;
    }
#endif
    return 0;
}

int HID_API_EXPORT hid_exit(void)
{
#ifndef HIDAPI_USE_DDK
    if (lib_handle)
        FreeLibrary(lib_handle);
    lib_handle = NULL;
    initialized = FALSE;
#endif
    return 0;
}

struct hid_device_info HID_API_EXPORT * HID_API_CALL hid_enumerate(unsigned short vendor_id, unsigned short product_id)
{
    BOOL res;
    struct hid_device_info *root = NULL; 
    struct hid_device_info *cur_dev = NULL;

    
    GUID InterfaceClassGuid = {0x4d1e55b2, 0xf16f, 0x11cf, {0x88, 0xcb, 0x00, 0x11, 0x11, 0x00, 0x00, 0x30} };
    SP_DEVINFO_DATA devinfo_data;
    SP_DEVICE_INTERFACE_DATA device_interface_data;
    SP_DEVICE_INTERFACE_DETAIL_DATA_A *device_interface_detail_data = NULL;
    HDEVINFO device_info_set = INVALID_HANDLE_VALUE;
    int device_index = 0;
    int i;

    if (hid_init() < 0)
        return NULL;

    
    memset(&devinfo_data, 0x0, sizeof(devinfo_data));
    devinfo_data.cbSize = sizeof(SP_DEVINFO_DATA);
    device_interface_data.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);

    
    device_info_set = SetupDiGetClassDevsA(&InterfaceClassGuid, NULL, NULL, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);

    

    for (;;) {
        HANDLE write_handle = INVALID_HANDLE_VALUE;
        DWORD required_size = 0;
        HIDD_ATTRIBUTES attrib;

        res = SetupDiEnumDeviceInterfaces(device_info_set,
            NULL,
            &InterfaceClassGuid,
            device_index,
            &device_interface_data);

        if (!res) {
            
            break;
        }

        
        res = SetupDiGetDeviceInterfaceDetailA(device_info_set,
            &device_interface_data,
            NULL,
            0,
            &required_size,
            NULL);

        
        device_interface_detail_data = (SP_DEVICE_INTERFACE_DETAIL_DATA_A*) malloc(required_size);
        device_interface_detail_data->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA_A);

        
        res = SetupDiGetDeviceInterfaceDetailA(device_info_set,
            &device_interface_data,
            device_interface_detail_data,
            required_size,
            NULL,
            NULL);

        if (!res) {
            
            goto cont;
        }

        
        for (i = 0; ; i++) {
            char driver_name[256];

            
            res = SetupDiEnumDeviceInfo(device_info_set, i, &devinfo_data);
            if (!res)
                goto cont;

            res = SetupDiGetDeviceRegistryPropertyA(device_info_set, &devinfo_data,
                           SPDRP_CLASS, NULL, (PBYTE)driver_name, sizeof(driver_name), NULL);
            if (!res)
                goto cont;

            if (strcmp(driver_name, "HIDClass") == 0) {
                
                res = SetupDiGetDeviceRegistryPropertyA(device_info_set, &devinfo_data,
                           SPDRP_DRIVER, NULL, (PBYTE)driver_name, sizeof(driver_name), NULL);
                if (res)
                    break;
            }
        }

        //wprintf(L"HandleName: %s\n", device_interface_detail_data->DevicePath);

        
        write_handle = open_device(device_interface_detail_data->DevicePath, TRUE);

        
        if (write_handle == INVALID_HANDLE_VALUE) {
            
            //register_error(dev, "CreateFile");
            goto cont_close;
        }


        
        attrib.Size = sizeof(HIDD_ATTRIBUTES);
        HidD_GetAttributes(write_handle, &attrib);
        //wprintf(L"Product/Vendor: %x %x\n", attrib.ProductID, attrib.VendorID);

        
        if ((vendor_id == 0x0 || attrib.VendorID == vendor_id) &&
            (product_id == 0x0 || attrib.ProductID == product_id)) {

            #define WSTR_LEN 512
            const char *str;
            struct hid_device_info *tmp;
            PHIDP_PREPARSED_DATA pp_data = NULL;
            HIDP_CAPS caps;
            BOOLEAN res;
            NTSTATUS nt_res;
            wchar_t wstr[WSTR_LEN]; 
            size_t len;

            
            tmp = (struct hid_device_info*) calloc(1, sizeof(struct hid_device_info));
            if (cur_dev) {
                cur_dev->next = tmp;
            }
            else {
                root = tmp;
            }
            cur_dev = tmp;

            
            res = HidD_GetPreparsedData(write_handle, &pp_data);
            if (res) {
                nt_res = HidP_GetCaps(pp_data, &caps);
                if (nt_res == HIDP_STATUS_SUCCESS) {
                    cur_dev->usage_page = caps.UsagePage;
                    cur_dev->usage = caps.Usage;
                }

                HidD_FreePreparsedData(pp_data);
            }

            
            cur_dev->next = NULL;
            str = device_interface_detail_data->DevicePath;
            if (str) {
                len = strlen(str);
                cur_dev->path = (char*) calloc(len+1, sizeof(char));
                strncpy(cur_dev->path, str, len+1);
                cur_dev->path[len] = '\0';
            }
            else
                cur_dev->path = NULL;

            
            res = HidD_GetSerialNumberString(write_handle, wstr, sizeof(wstr));
            wstr[WSTR_LEN-1] = 0x0000;
            if (res) {
                cur_dev->serial_number = _wcsdup(wstr);
            }

            
            res = HidD_GetManufacturerString(write_handle, wstr, sizeof(wstr));
            wstr[WSTR_LEN-1] = 0x0000;
            if (res) {
                cur_dev->manufacturer_string = _wcsdup(wstr);
            }

            
            res = HidD_GetProductString(write_handle, wstr, sizeof(wstr));
            wstr[WSTR_LEN-1] = 0x0000;
            if (res) {
                cur_dev->product_string = _wcsdup(wstr);
            }

            
            cur_dev->vendor_id = attrib.VendorID;
            cur_dev->product_id = attrib.ProductID;

            
            cur_dev->release_number = attrib.VersionNumber;

            
            cur_dev->interface_number = -1;
            if (cur_dev->path) {
                char *interface_component = strstr(cur_dev->path, "&mi_");
                if (interface_component) {
                    char *hex_str = interface_component + 4;
                    char *endptr = NULL;
                    cur_dev->interface_number = strtol(hex_str, &endptr, 16);
                    if (endptr == hex_str) {
                        
                        cur_dev->interface_number = -1;
                    }
                }
            }
        }

cont_close:
        CloseHandle(write_handle);
cont:
        
        free(device_interface_detail_data);

        device_index++;

    }

    
    SetupDiDestroyDeviceInfoList(device_info_set);

    return root;

}

void  HID_API_EXPORT HID_API_CALL hid_free_enumeration(struct hid_device_info *devs)
{
    
    struct hid_device_info *d = devs;
    while (d) {
        struct hid_device_info *next = d->next;
        free(d->path);
        free(d->serial_number);
        free(d->manufacturer_string);
        free(d->product_string);
        free(d);
        d = next;
    }
}


HID_API_EXPORT hid_device * HID_API_CALL hid_open(unsigned short vendor_id, unsigned short product_id, const wchar_t *serial_number)
{
    
    struct hid_device_info *devs, *cur_dev;
    const char *path_to_open = NULL;
    hid_device *handle = NULL;

    devs = hid_enumerate(vendor_id, product_id);
    cur_dev = devs;
    while (cur_dev) {
        if (cur_dev->vendor_id == vendor_id &&
            cur_dev->product_id == product_id) {
            if (serial_number) {
                if (wcscmp(serial_number, cur_dev->serial_number) == 0) {
                    path_to_open = cur_dev->path;
                    break;
                }
            }
            else {
                path_to_open = cur_dev->path;
                break;
            }
        }
        cur_dev = cur_dev->next;
    }

    if (path_to_open) {
        
        handle = hid_open_path(path_to_open);
    }

    hid_free_enumeration(devs);

    return handle;
}

HID_API_EXPORT hid_device * HID_API_CALL hid_open_path(const char *path)
{
    hid_device *dev;
    HIDP_CAPS caps;
    PHIDP_PREPARSED_DATA pp_data = NULL;
    BOOLEAN res;
    NTSTATUS nt_res;

    if (hid_init() < 0) {
        return NULL;
    }

    dev = new_hid_device();

    
    dev->device_handle = open_device(path, FALSE);

    
    if (dev->device_handle == INVALID_HANDLE_VALUE) {
        
        register_error(dev, "CreateFile");
        goto err;
    }

    
    res = HidD_SetNumInputBuffers(dev->device_handle, 64);
    if (!res) {
        register_error(dev, "HidD_SetNumInputBuffers");
        goto err;
    }

    
    res = HidD_GetPreparsedData(dev->device_handle, &pp_data);
    if (!res) {
        register_error(dev, "HidD_GetPreparsedData");
        goto err;
    }
    nt_res = HidP_GetCaps(pp_data, &caps);
    if (nt_res != HIDP_STATUS_SUCCESS) {
        register_error(dev, "HidP_GetCaps");
        goto err_pp_data;
    }
    dev->output_report_length = caps.OutputReportByteLength;
    dev->input_report_length = caps.InputReportByteLength;
    HidD_FreePreparsedData(pp_data);

    dev->read_buf = (char*) malloc(dev->input_report_length);

    return dev;

err_pp_data:
        HidD_FreePreparsedData(pp_data);
err:
        free_hid_device(dev);
        return NULL;
}

int HID_API_EXPORT HID_API_CALL hid_write(hid_device *dev, const unsigned char *data, size_t length)
{
    DWORD bytes_written;
    BOOL res;

    OVERLAPPED ol;
    unsigned char *buf;
    memset(&ol, 0, sizeof(ol));

    
    if (length >= dev->output_report_length) {
        
        buf = (unsigned char *) data;
    } else {
        
        buf = (unsigned char *) malloc(dev->output_report_length);
        memcpy(buf, data, length);
        memset(buf + length, 0, dev->output_report_length - length);
        length = dev->output_report_length;
    }

    res = WriteFile(dev->device_handle, buf, length, NULL, &ol);

    if (!res) {
        if (GetLastError() != ERROR_IO_PENDING) {
            
            register_error(dev, "WriteFile");
            bytes_written = -1;
            goto end_of_function;
        }
    }

    
    res = GetOverlappedResult(dev->device_handle, &ol, &bytes_written, TRUE);
    if (!res) {
        
        register_error(dev, "WriteFile");
        bytes_written = -1;
        goto end_of_function;
    }

end_of_function:
    if (buf != data)
        free(buf);

    return bytes_written;
}


int HID_API_EXPORT HID_API_CALL hid_read_timeout(hid_device *dev, unsigned char *data, size_t length, int milliseconds)
{
    DWORD bytes_read = 0;
    size_t copy_len = 0;
    BOOL res;

    
    HANDLE ev = dev->ol.hEvent;

    if (!dev->read_pending) {
        
        dev->read_pending = TRUE;
        memset(dev->read_buf, 0, dev->input_report_length);
        ResetEvent(ev);
        res = ReadFile(dev->device_handle, dev->read_buf, dev->input_report_length, &bytes_read, &dev->ol);

        if (!res) {
            if (GetLastError() != ERROR_IO_PENDING) {
                
                CancelIo(dev->device_handle);
                dev->read_pending = FALSE;
                goto end_of_function;
            }
        }
    }

    if (milliseconds >= 0) {
        
        res = WaitForSingleObject(ev, milliseconds);
        if (res != WAIT_OBJECT_0) {
            
            return 0;
        }
    }

    
    res = GetOverlappedResult(dev->device_handle, &dev->ol, &bytes_read, TRUE);

    
    dev->read_pending = FALSE;

    if (res && bytes_read > 0) {
        if (dev->read_buf[0] == 0x0) {
            
            bytes_read--;
            copy_len = length > bytes_read ? bytes_read : length;
            memcpy(data, dev->read_buf+1, copy_len);
        }
        else {
            
            copy_len = length > bytes_read ? bytes_read : length;
            memcpy(data, dev->read_buf, copy_len);
        }
    }

end_of_function:
    if (!res) {
        register_error(dev, "GetOverlappedResult");
        return -1;
    }

    return copy_len;
}

int HID_API_EXPORT HID_API_CALL hid_read(hid_device *dev, unsigned char *data, size_t length)
{
    return hid_read_timeout(dev, data, length, (dev->blocking)? -1: 0);
}

int HID_API_EXPORT HID_API_CALL hid_set_nonblocking(hid_device *dev, int nonblock)
{
    dev->blocking = !nonblock;
    return 0; 
}

int HID_API_EXPORT HID_API_CALL hid_send_feature_report(hid_device *dev, const unsigned char *data, size_t length)
{
    BOOL res = HidD_SetFeature(dev->device_handle, (PVOID)data, length);
    if (!res) {
        register_error(dev, "HidD_SetFeature");
        return -1;
    }

    return length;
}


int HID_API_EXPORT HID_API_CALL hid_get_feature_report(hid_device *dev, unsigned char *data, size_t length)
{
    BOOL res;
#if 0
    res = HidD_GetFeature(dev->device_handle, data, length);
    if (!res) {
        register_error(dev, "HidD_GetFeature");
        return -1;
    }
    return 0; 
#else
    DWORD bytes_returned;

    OVERLAPPED ol;
    memset(&ol, 0, sizeof(ol));

    res = DeviceIoControl(dev->device_handle,
        IOCTL_HID_GET_FEATURE,
        data, length,
        data, length,
        &bytes_returned, &ol);

    if (!res) {
        if (GetLastError() != ERROR_IO_PENDING) {
            
            register_error(dev, "Send Feature Report DeviceIoControl");
            return -1;
        }
    }

    
    res = GetOverlappedResult(dev->device_handle, &ol, &bytes_returned, TRUE);
    if (!res) {
        
        register_error(dev, "Send Feature Report GetOverLappedResult");
        return -1;
    }

    
    bytes_returned++;

    return bytes_returned;
#endif
}

void HID_API_EXPORT HID_API_CALL hid_close(hid_device *dev)
{
    if (!dev)
        return;
    CancelIo(dev->device_handle);
    free_hid_device(dev);
}

int HID_API_EXPORT_CALL HID_API_CALL hid_get_manufacturer_string(hid_device *dev, wchar_t *string, size_t maxlen)
{
    BOOL res;

    res = HidD_GetManufacturerString(dev->device_handle, string, sizeof(wchar_t) * maxlen);
    if (!res) {
        register_error(dev, "HidD_GetManufacturerString");
        return -1;
    }

    return 0;
}

int HID_API_EXPORT_CALL HID_API_CALL hid_get_product_string(hid_device *dev, wchar_t *string, size_t maxlen)
{
    BOOL res;

    res = HidD_GetProductString(dev->device_handle, string, sizeof(wchar_t) * maxlen);
    if (!res) {
        register_error(dev, "HidD_GetProductString");
        return -1;
    }

    return 0;
}

int HID_API_EXPORT_CALL HID_API_CALL hid_get_serial_number_string(hid_device *dev, wchar_t *string, size_t maxlen)
{
    BOOL res;

    res = HidD_GetSerialNumberString(dev->device_handle, string, sizeof(wchar_t) * maxlen);
    if (!res) {
        register_error(dev, "HidD_GetSerialNumberString");
        return -1;
    }

    return 0;
}

int HID_API_EXPORT_CALL HID_API_CALL hid_get_indexed_string(hid_device *dev, int string_index, wchar_t *string, size_t maxlen)
{
    BOOL res;

    res = HidD_GetIndexedString(dev->device_handle, string_index, string, sizeof(wchar_t) * maxlen);
    if (!res) {
        register_error(dev, "HidD_GetIndexedString");
        return -1;
    }

    return 0;
}


HID_API_EXPORT const wchar_t * HID_API_CALL  hid_error(hid_device *dev)
{
    return (wchar_t*)dev->last_error_str;
}




#define P32
#ifdef S11
  unsigned short VendorID = 0xa0a0;
    unsigned short ProductID = 0x0001;
#endif

#ifdef P32
  unsigned short VendorID = 0x04d8;
    unsigned short ProductID = 0x3f;
#endif


#ifdef PICPGM
  unsigned short VendorID = 0x04d8;
  unsigned short ProductID = 0x0033;
#endif


#if 0
int __cdecl main(int argc, char* argv[])
{
    int res;
    unsigned char buf[65];

    UNREFERENCED_PARAMETER(argc);
    UNREFERENCED_PARAMETER(argv);

    
    memset(buf,0x00,sizeof(buf));
    buf[0] = 0;
    buf[1] = 0x81;


    
    int handle = open(VendorID, ProductID, L"12345");
    if (handle < 0)
        printf("unable to open device\n");


    
    buf[1] = 0x80;
    res = write(handle, buf, 65);
    if (res < 0)
        printf("Unable to write()\n");

    
    buf[1] = 0x81;
    write(handle, buf, 65);
    if (res < 0)
        printf("Unable to write() (2)\n");

    
    read(handle, buf, 65);
    if (res < 0)
        printf("Unable to read()\n");

    
    for (int i = 0; i < 4; i++)
        printf("buf[%d]: %d\n", i, buf[i]);

    return 0;
}
#endif

#ifdef __cplusplus
} 
#endif


#else









#include <IOKit/hid/IOHIDManager.h>
#include <IOKit/hid/IOHIDKeys.h>
#include <CoreFoundation/CoreFoundation.h>
#include <wchar.h>
#include <locale.h>
#include <pthread.h>
#include <sys/time.h>
#include <unistd.h>

#include "hidapi.h"


typedef int pthread_barrierattr_t;
typedef struct pthread_barrier {
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    int count;
    int trip_count;
} pthread_barrier_t;

IOHIDDeviceCallback g_device_arrival_cbk = NULL; //设备到达消息回调地址
IOHIDDeviceCallback g_device_removal_cbk = NULL; //设备移除消息回调地址

static int pthread_barrier_init(pthread_barrier_t *barrier, const pthread_barrierattr_t *attr, unsigned int count)
{
	if(count == 0) {
		errno = EINVAL;
		return -1;
	}

	if(pthread_mutex_init(&barrier->mutex, 0) < 0) {
		return -1;
	}
	if(pthread_cond_init(&barrier->cond, 0) < 0) {
		pthread_mutex_destroy(&barrier->mutex);
		return -1;
	}
	barrier->trip_count = count;
	barrier->count = 0;

	return 0;
}

static int pthread_barrier_destroy(pthread_barrier_t *barrier)
{
	pthread_cond_destroy(&barrier->cond);
	pthread_mutex_destroy(&barrier->mutex);
	return 0;
}

static int pthread_barrier_wait(pthread_barrier_t *barrier)
{
	pthread_mutex_lock(&barrier->mutex);
	++(barrier->count);
	if(barrier->count >= barrier->trip_count)
	{
		barrier->count = 0;
		pthread_cond_broadcast(&barrier->cond);
		pthread_mutex_unlock(&barrier->mutex);
		return 1;
	}
	else
	{
//        struct timeval now;
//        struct timespec outtime;

//        gettimeofday(&now, NULL);
//        outtime.tv_sec = now.tv_sec + 1;
//        outtime.tv_nsec = now.tv_usec * 1000;

        pthread_cond_wait(&barrier->cond, &(barrier->mutex));
		pthread_mutex_unlock(&barrier->mutex);
		return 0;
	}
}

static int return_data(hid_device *dev, unsigned char *data, size_t length);


struct input_report {
	uint8_t *data;
	size_t len;
	struct input_report *next;
};

struct hid_device_ {
	IOHIDDeviceRef device_handle;
	int blocking;
	int uses_numbered_reports;
	int disconnected;
	CFStringRef run_loop_mode;
	CFRunLoopRef run_loop;
	CFRunLoopSourceRef source;
	uint8_t *input_report_buf;
	CFIndex max_input_report_len;
	struct input_report *input_reports;

	pthread_t thread;
	pthread_mutex_t mutex; 
	pthread_cond_t condition;
	pthread_barrier_t barrier; 
	pthread_barrier_t shutdown_barrier; 
	int shutdown_thread;
};

static hid_device *new_hid_device(void)
{
    hid_device *dev = (hid_device*)calloc(1, sizeof(hid_device));
    if(dev <= 0)
        return NULL;

    memset(dev, 0, sizeof(hid_device));

	dev->device_handle = NULL;
	dev->blocking = 1;
	dev->uses_numbered_reports = 0;
	dev->disconnected = 0;
	dev->run_loop_mode = NULL;
	dev->run_loop = NULL;
	dev->source = NULL;
	dev->input_report_buf = NULL;
	dev->input_reports = NULL;
	dev->shutdown_thread = 0;
    dev->thread = 0;
    dev->max_input_report_len = 0;

	
	pthread_mutex_init(&dev->mutex, NULL);
	pthread_cond_init(&dev->condition, NULL);
	pthread_barrier_init(&dev->barrier, NULL, 2);
	pthread_barrier_init(&dev->shutdown_barrier, NULL, 2);

	return dev;
}

static void free_hid_device(hid_device *dev)
{
	if (!dev)
		return;

	
	struct input_report *rpt = dev->input_reports;
	while (rpt) {
		struct input_report *next = rpt->next;
        if(rpt->data)
        {
            free(rpt->data);
            rpt->data = NULL;
            rpt->len = 0;
        }        

        if(rpt)
        {
            free(rpt);
            rpt = NULL;
        }
		rpt = next;
	}

	
	if (dev->run_loop_mode)
    {
        CFRelease(dev->run_loop_mode);
        dev->run_loop_mode = NULL;
    }
	if (dev->source)
    {
        CFRelease(dev->source);
        dev->source = NULL;
    }
    if(dev->input_report_buf)
    {
        free(dev->input_report_buf);
        dev->input_report_buf = NULL;
    }

	
	pthread_barrier_destroy(&dev->shutdown_barrier);
	pthread_barrier_destroy(&dev->barrier);
	pthread_cond_destroy(&dev->condition);
	pthread_mutex_destroy(&dev->mutex);

	
    if(dev)
    {
        free(dev);
        dev = NULL;
    }
}

static IOHIDManagerRef hid_mgr = 0x0;


#if 0
static void register_error(hid_device *device, const char *op)
{

}
#endif


static int32_t get_int_property(IOHIDDeviceRef device, CFStringRef key)
{
    CFTypeRef ref = NULL;
    int32_t value = 0;

	ref = IOHIDDeviceGetProperty(device, key);
	if (ref) {
		if (CFGetTypeID(ref) == CFNumberGetTypeID()) {
			CFNumberGetValue((CFNumberRef) ref, kCFNumberSInt32Type, &value);
			return value;
		}
	}
	return 0;
}


HID_API_EXPORT unsigned short get_vendor_id(IOHIDDeviceRef device)
{
	return get_int_property(device, CFSTR(kIOHIDVendorIDKey));
}


HID_API_EXPORT unsigned short get_product_id(IOHIDDeviceRef device)
{
	return get_int_property(device, CFSTR(kIOHIDProductIDKey));
}

static int32_t get_location_id(IOHIDDeviceRef device)
{
	return get_int_property(device, CFSTR(kIOHIDLocationIDKey));
}

static int32_t get_max_report_length(IOHIDDeviceRef device)
{
	return get_int_property(device, CFSTR(kIOHIDMaxInputReportSizeKey));
}

static int get_string_property(IOHIDDeviceRef device, CFStringRef prop, wchar_t *buf, size_t len)
{
    CFStringRef str = NULL;

	if (!len)
		return 0;

    str = (CFStringRef)IOHIDDeviceGetProperty(device, prop);

	buf[0] = 0;

	if (str) {
		CFIndex str_len = CFStringGetLength(str);
        CFRange range = {0, 0};
        CFIndex used_buf_len = 0;
        CFIndex chars_copied = 0;

        len--;

		range.location = 0;
		range.length = ((size_t)str_len > len)? len: (size_t)str_len;
		chars_copied = CFStringGetBytes(str,
			range,
			kCFStringEncodingUTF32LE,
			(char)'?',
			FALSE,
			(UInt8*)buf,
			len * sizeof(wchar_t),
			&used_buf_len);

		if (chars_copied == len)
			buf[len] = 0; 
		else
			buf[chars_copied] = 0;        

		return 0;
	}
	else
		return -1;

}

static int get_string_property_utf8(IOHIDDeviceRef device, CFStringRef prop, char *buf, size_t len)
{
    CFStringRef str = NULL;
	if (!len)
		return 0;

    str = (CFStringRef)IOHIDDeviceGetProperty(device, prop);

	buf[0] = 0;

	if (str) {
		len--;

		CFIndex str_len = CFStringGetLength(str);
        CFRange range = {0, 0};
		range.location = 0;
		range.length = str_len;
        CFIndex used_buf_len = 0;
        CFIndex chars_copied = 0;
		chars_copied = CFStringGetBytes(str,
			range,
			kCFStringEncodingUTF8,
			(char)'?',
			FALSE,
			(UInt8*)buf,
			len,
			&used_buf_len);

		if (used_buf_len == len)
			buf[len] = 0; 
		else
			buf[used_buf_len] = 0;        

		return used_buf_len;
	}
	else
		return 0;
}


static int get_serial_number(IOHIDDeviceRef device, wchar_t *buf, size_t len)
{
	return get_string_property(device, CFSTR(kIOHIDSerialNumberKey), buf, len);
}

static int get_manufacturer_string(IOHIDDeviceRef device, wchar_t *buf, size_t len)
{
	return get_string_property(device, CFSTR(kIOHIDManufacturerKey), buf, len);
}

static int get_product_string(IOHIDDeviceRef device, wchar_t *buf, size_t len)
{
	return get_string_property(device, CFSTR(kIOHIDProductKey), buf, len);
}



static wchar_t *dup_wcs(const wchar_t *s)
{
	size_t len = wcslen(s);
    wchar_t *ret = (wchar_t*)malloc((len+1)*sizeof(wchar_t));
	wcscpy(ret, s);
    ret[len] = 0;
	return ret;
}


static int make_path(IOHIDDeviceRef device, char *buf, size_t len)
{
    int res = 0;
    unsigned short vid = 0, pid = 0;
    char transport[32] = {0};
    int32_t location = 0;

	buf[0] = '\0';

	res = get_string_property_utf8(
		device, CFSTR(kIOHIDTransportKey),
		transport, sizeof(transport));

	if (!res)
		return -1;

	location = get_location_id(device);
	vid = get_vendor_id(device);
	pid = get_product_id(device);

	res = snprintf(buf, len, "%s_%04hx_%04hx_%x",
                       transport, vid, pid, location);


    buf[len-1] = '\0';
	return res+1;
}


static int init_hid_manager()
{
	
	hid_mgr = IOHIDManagerCreate(kCFAllocatorDefault, kIOHIDOptionsTypeNone);
    if (hid_mgr)
    {
//        CFMutableDictionaryRef dict = CFDictionaryCreateMutable(kCFAllocatorDefault, 2,
//                                                &kCFTypeDictionaryKeyCallBacks,
//                                                &kCFTypeDictionaryValueCallBacks);
        IOHIDManagerSetDeviceMatching(hid_mgr, NULL);

#if 1
        IOHIDManagerRegisterDeviceMatchingCallback(hid_mgr, g_device_arrival_cbk, NULL);
        IOHIDManagerRegisterDeviceRemovalCallback(hid_mgr, g_device_removal_cbk, NULL);
#endif

        IOHIDManagerScheduleWithRunLoop(hid_mgr, CFRunLoopGetMain(), kCFRunLoopDefaultMode);

//        IOReturn ret = IOHIDManagerOpen(hid_mgr, kIOHIDOptionsTypeNone);
//        if(ret == kIOReturnSuccess)
//            return 0;
	}

	return -1;
}


int HID_API_EXPORT hid_init(void)
{
	if (!hid_mgr) {
        return init_hid_manager();
	}

	
	return 0;
}

int HID_API_EXPORT hid_exit(void)
{
	if (hid_mgr) {
		
		IOHIDManagerClose(hid_mgr, kIOHIDOptionsTypeNone);
		CFRelease(hid_mgr);
		hid_mgr = NULL;
	}

	return 0;
}

static void process_pending_events(void) {
    SInt32 res = 0;
	do {
		res = CFRunLoopRunInMode(kCFRunLoopDefaultMode, 0.001, FALSE);
	} while(res != kCFRunLoopRunFinished && res != kCFRunLoopRunTimedOut);
}

struct hid_device_info  HID_API_EXPORT *hid_enumerate(unsigned short vendor_id, unsigned short product_id)
{
	struct hid_device_info *root = NULL; 
	struct hid_device_info *cur_dev = NULL;
    CFIndex num_devices = 0;
    int i = 0;

	
	if (hid_init() < 0)
		return NULL;

	
	process_pending_events();

	

//    CFStringRef keys[2];
//    CFNumberRef values[2];

//    CFNumberRef vendorID = CFNumberCreate( kCFAllocatorDefault, kCFNumberIntType, &vendor_id );
//    CFNumberRef productID = CFNumberCreate( kCFAllocatorDefault, kCFNumberIntType, &product_id );

//    keys[0] = CFSTR( kIOHIDVendorIDKey );
//    values[0] = vendorID;
//    keys[1] = CFSTR( kIOHIDProductIDKey );
//    values[1] = productID;

//    CFDictionaryRef dict = CFDictionaryCreate( kCFAllocatorDefault, (const void **) &keys, (const void **) &values, 1, NULL, NULL);

    IOHIDManagerSetDeviceMatching(hid_mgr, NULL);
	CFSetRef device_set = IOHIDManagerCopyDevices(hid_mgr);

	
	num_devices = CFSetGetCount(device_set);
    IOHIDDeviceRef *device_array = (IOHIDDeviceRef *)calloc(num_devices, sizeof(IOHIDDeviceRef));
    memset(device_array, 0, num_devices * sizeof(IOHIDDeviceRef));
	CFSetGetValues(device_set, (const void **) device_array);

	
	for (i = 0; i < num_devices; i++) {
        unsigned short dev_vid = 0;
        unsigned short dev_pid = 0;
		#define BUF_LEN 256
        wchar_t buf[BUF_LEN] = {0};
        char cbuf[BUF_LEN] = {0};

		IOHIDDeviceRef dev = device_array[i];

        if (!dev) {
            continue;
        }
		dev_vid = get_vendor_id(dev);
		dev_pid = get_product_id(dev);

		
        if ((vendor_id == 0x0 || vendor_id == dev_vid) &&
            (product_id == 0x0 || product_id == dev_pid)) {
            struct hid_device_info *tmp = NULL;
            size_t len = 0;

			
            tmp = (hid_device_info*)malloc(sizeof(struct hid_device_info));
			if (cur_dev) {
				cur_dev->next = tmp;
			}
			else {
				root = tmp;
			}
			cur_dev = tmp;

			
			cur_dev->usage_page = get_int_property(dev, CFSTR(kIOHIDPrimaryUsagePageKey));
			cur_dev->usage = get_int_property(dev, CFSTR(kIOHIDPrimaryUsageKey));

			
			cur_dev->next = NULL;
			len = make_path(dev, cbuf, sizeof(cbuf));
			cur_dev->path = strdup(cbuf);

			
			get_serial_number(dev, buf, BUF_LEN);
			cur_dev->serial_number = dup_wcs(buf);

			
			get_manufacturer_string(dev, buf, BUF_LEN);
			cur_dev->manufacturer_string = dup_wcs(buf);
			get_product_string(dev, buf, BUF_LEN);
			cur_dev->product_string = dup_wcs(buf);

			
			cur_dev->vendor_id = dev_vid;
			cur_dev->product_id = dev_pid;

			
			cur_dev->release_number = get_int_property(dev, CFSTR(kIOHIDVersionNumberKey));

			
			cur_dev->interface_number = -1;
		}
	}

//    CFRelease(dict);
//    CFRelease(vendorID);
//    CFRelease(productID);

    if(device_array)
    {
        free(device_array);
        device_array = NULL;
    }

    if(device_set)
    {
        CFRelease(device_set);
        device_set = NULL;
    }

	return root;
}

void  HID_API_EXPORT hid_free_enumeration(struct hid_device_info *devs)
{
	
	struct hid_device_info *d = devs;
	while (d) {
		struct hid_device_info *next = d->next;
        if(d->path)
        {
            free(d->path);
            d->path = NULL;
        }
        if(d->serial_number)
        {
            free(d->serial_number);
            d->serial_number = NULL;
        }
        if(d->manufacturer_string)
        {
            free(d->manufacturer_string);
            d->manufacturer_string = NULL;
        }
        if(d->product_string)
        {
            free(d->product_string);
            d->product_string = NULL;
        }
        if(d)
        {
            free(d);
            d = NULL;
        }
		d = next;
	}
}

hid_device * HID_API_EXPORT hid_open(unsigned short vendor_id, unsigned short product_id, const wchar_t *serial_number)
{
	
    struct hid_device_info *devs = NULL, *cur_dev = NULL;
	const char *path_to_open = NULL;
	hid_device * handle = NULL;

	devs = hid_enumerate(vendor_id, product_id);
	cur_dev = devs;
	while (cur_dev) {
		if (cur_dev->vendor_id == vendor_id &&
		    cur_dev->product_id == product_id) {
			if (serial_number) {
				if (wcscmp(serial_number, cur_dev->serial_number) == 0) {
					path_to_open = cur_dev->path;
					break;
				}
			}
			else {
				path_to_open = cur_dev->path;
				break;
			}
		}
		cur_dev = cur_dev->next;
	}

	if (path_to_open) {
		
		handle = hid_open_path(path_to_open);
	}

    if(devs)
    {
        hid_free_enumeration(devs);
        devs = NULL;
    }
	return handle;
}

static void hid_device_removal_callback(void *context, IOReturn result,
                                        void *sender)
{
	
    hid_device *d = (hid_device*)context;
    d->disconnected = 1;
	CFRunLoopStop(d->run_loop);

    //if(g_device_removal_cbk)
    //    g_device_removal_cbk(context, result, sender, d->device_handle);
}


static void hid_report_callback(void *context, IOReturn result, void *sender,
                         IOHIDReportType report_type, uint32_t report_id,
                         uint8_t *report, CFIndex report_length)
{
    struct input_report *rpt = NULL;
    hid_device *dev = (hid_device*)context;

	
    rpt = (input_report*)calloc(1, sizeof(struct input_report));
    memset(rpt, 0, sizeof(struct input_report));

    rpt->data = (uint8_t*)calloc(1, report_length);
    memset(rpt->data, 0, report_length);
	memcpy(rpt->data, report, report_length);
	rpt->len = report_length;
	rpt->next = NULL;    

	
	pthread_mutex_lock(&dev->mutex);

	
	if (dev->input_reports == NULL) {
		
		dev->input_reports = rpt;
	}
	else {
		
		struct input_report *cur = dev->input_reports;
		int num_queued = 0;
		while (cur->next != NULL) {
			cur = cur->next;
			num_queued++;
		}
		cur->next = rpt;

		
		if (num_queued > 30) {
			return_data(dev, NULL, 0);
		}
	}

	
	pthread_cond_signal(&dev->condition);

	
	pthread_mutex_unlock(&dev->mutex);

    //if(g_device_arrival_cbk)
    //    g_device_arrival_cbk(context, result, sender, report_type, report_id, report, report_length);
}


static void perform_signal_callback(void *context)
{
    hid_device *dev = (hid_device*)context;
	CFRunLoopStop(dev->run_loop); 
}

static void *read_thread(void *param)
{
    hid_device *dev = (hid_device*)param;
    SInt32 code = 0;

	
	IOHIDDeviceScheduleWithRunLoop(dev->device_handle, CFRunLoopGetCurrent(), dev->run_loop_mode);

	
	CFRunLoopSourceContext ctx;
	memset(&ctx, 0, sizeof(ctx));
	ctx.version = 0;
	ctx.info = dev;
	ctx.perform = &perform_signal_callback;
	dev->source = CFRunLoopSourceCreate(kCFAllocatorDefault, 0, &ctx);
	CFRunLoopAddSource(CFRunLoopGetCurrent(), dev->source, dev->run_loop_mode);

	
	dev->run_loop = CFRunLoopGetCurrent();

	
	pthread_barrier_wait(&dev->barrier);

	
	while (!dev->shutdown_thread && !dev->disconnected) {
        code = CFRunLoopRunInMode(dev->run_loop_mode, 1000, FALSE);
		
		if (code == kCFRunLoopRunFinished) {
			dev->disconnected = 1;
			break;
		}


		
		if (code != kCFRunLoopRunTimedOut &&
		    code != kCFRunLoopRunHandledSource) {
			
			dev->shutdown_thread = 1;
			break;
		}
	}

	
	pthread_mutex_lock(&dev->mutex);
	pthread_cond_broadcast(&dev->condition);
	pthread_mutex_unlock(&dev->mutex);

	
	pthread_barrier_wait(&dev->shutdown_barrier);

	return NULL;
}

hid_device * HID_API_EXPORT hid_open_path(const char *path)
{
    int i = 0;
	hid_device *dev = NULL;
    CFIndex num_devices = 0;

	dev = new_hid_device();

	
	if (hid_init() < 0)
		return NULL;

	
	process_pending_events();

    CFSetRef device_set = IOHIDManagerCopyDevices(hid_mgr);

	num_devices = CFSetGetCount(device_set);
    IOHIDDeviceRef *device_array = (IOHIDDeviceRef*)calloc(num_devices, sizeof(IOHIDDeviceRef));
    memset(device_array, 0, num_devices * sizeof(IOHIDDeviceRef));
	CFSetGetValues(device_set, (const void **) device_array);
	for (i = 0; i < num_devices; i++) {
        char cbuf[BUF_LEN] = {0};
        size_t len = 0;
        IOHIDDeviceRef os_dev = device_array[i];
        len = make_path(os_dev, cbuf, sizeof(cbuf));
		if (!strcmp(cbuf, path)) {
			
			IOReturn ret = IOHIDDeviceOpen(os_dev, kIOHIDOptionsTypeSeizeDevice);
			if (ret == kIOReturnSuccess) {
                char str[32] = {0};
                if(device_array)
                {
                    free(device_array);
                    device_array = NULL;
                }
				CFRetain(os_dev);

                if(device_set)
                {
                    CFRelease(device_set);
                    device_set = NULL;
                }
				dev->device_handle = os_dev;

				
				dev->max_input_report_len = (CFIndex) get_max_report_length(os_dev);
                dev->input_report_buf = (uint8_t*)calloc(dev->max_input_report_len, sizeof(uint8_t));
                memset(dev->input_report_buf, 0, dev->max_input_report_len * sizeof(uint8_t));

				
				sprintf(str, "HIDAPI_%p", os_dev);
				dev->run_loop_mode =
					CFStringCreateWithCString(NULL, str, kCFStringEncodingASCII);

				
				IOHIDDeviceRegisterInputReportCallback(
					os_dev, dev->input_report_buf, dev->max_input_report_len,
					&hid_report_callback, dev);
				IOHIDDeviceRegisterRemovalCallback(dev->device_handle, hid_device_removal_callback, dev);

				
				pthread_create(&dev->thread, NULL, read_thread, dev);

				
				pthread_barrier_wait(&dev->barrier);

				return dev;
			}
			else {
				goto return_error;
			}
		}
	}

return_error:
    if(device_array)
    {
        free(device_array);
        device_array = NULL;
    }

    if(device_set)
    {
        CFRelease(device_set);
        device_set = NULL;
    }

    if(dev)
    {
        free_hid_device(dev);
        dev = NULL;
    }

	return NULL;
}

static int set_report(hid_device *dev, IOHIDReportType type, const unsigned char *data, size_t length)
{
    const unsigned char *data_to_send = NULL;
    size_t length_to_send = 0;
    IOReturn res = 0;

	
	if (dev->disconnected)
		return -1;

	if (data[0] == 0x0) {
		
		data_to_send = data+1;
		length_to_send = length-1;
	}
	else {
		
		data_to_send = data;
		length_to_send = length;
	}

	if (!dev->disconnected) {
		res = IOHIDDeviceSetReport(dev->device_handle,
					   type,
					   data[0], 
					   data_to_send, length_to_send);

		if (res == kIOReturnSuccess) {
			return length;
		}
		else
			return -1;
	}

	return -1;
}

int HID_API_EXPORT hid_write(hid_device *dev, const unsigned char *data, size_t length)
{
	return set_report(dev, kIOHIDReportTypeOutput, data, length);
}


static int return_data(hid_device *dev, unsigned char *data, size_t length)
{
	
    struct input_report *rpt = dev->input_reports;
    size_t len = (length < rpt->len)? length: rpt->len;

    memcpy(data, rpt->data, len);
    dev->input_reports = rpt->next;

    if(rpt->data)
    {
        free(rpt->data);
        rpt->data = NULL;        
    }

    if(rpt)
    {
        free(rpt);
        rpt = NULL;
    }

	return len;
}

static int cond_wait(const hid_device *dev, pthread_cond_t *cond, pthread_mutex_t *mutex)
{
    while (!dev->input_reports) {
		int res = pthread_cond_wait(cond, mutex);        
		if (res != 0)
			return res;

		

		if (dev->shutdown_thread || dev->disconnected)
			return -1;
	}

	return 0;
}

static int cond_timedwait(const hid_device *dev, pthread_cond_t *cond, pthread_mutex_t *mutex, const struct timespec *abstime)
{
	while (!dev->input_reports) {
		int res = pthread_cond_timedwait(cond, mutex, abstime);
		if (res != 0)
			return res;

		

		if (dev->shutdown_thread || dev->disconnected)
			return -1;
	}

	return 0;

}

int HID_API_EXPORT hid_read_timeout(hid_device *dev, unsigned char *data, size_t length, int milliseconds)
{
	int bytes_read = -1;

	
	pthread_mutex_lock(&dev->mutex);

	
	if (dev->input_reports) {
		        
		bytes_read = return_data(dev, data, length);
		goto ret;
    }

	
	if (dev->disconnected) {
		bytes_read = -1;
		goto ret;
	}

	if (dev->shutdown_thread) {
		
		bytes_read = -1;
		goto ret;
	}

	    
    if (milliseconds == -1) {

		
		int res;
		res = cond_wait(dev, &dev->condition, &dev->mutex);
		if (res == 0)
        {            
            bytes_read = return_data(dev, data, length);
        }
        else
        {            
			
			bytes_read = -1;
		}
	}
    else if (milliseconds > 0) {

		
        int res = 0;
        struct timespec ts = {0, 0};
        struct timeval tv = {0, 0};
		gettimeofday(&tv, NULL);
		TIMEVAL_TO_TIMESPEC(&tv, &ts);
		ts.tv_sec += milliseconds / 1000;
		ts.tv_nsec += (milliseconds % 1000) * 1000000;
		if (ts.tv_nsec >= 1000000000L) {
			ts.tv_sec++;
			ts.tv_nsec -= 1000000000L;
		}

		res = cond_timedwait(dev, &dev->condition, &dev->mutex, &ts);
		if (res == 0)
			bytes_read = return_data(dev, data, length);
		else if (res == ETIMEDOUT)
			bytes_read = 0;
		else
			bytes_read = -1;
	}
    else {
		
		bytes_read = 0;
    }

ret:
	
	pthread_mutex_unlock(&dev->mutex);    
	return bytes_read;
}

int HID_API_EXPORT hid_read(hid_device *dev, unsigned char *data, size_t length)
{
	return hid_read_timeout(dev, data, length, (dev->blocking)? -1: 0);
}

int HID_API_EXPORT hid_set_nonblocking(hid_device *dev, int nonblock)
{
	
	dev->blocking = !nonblock;

	return 0;
}

int HID_API_EXPORT hid_send_feature_report(hid_device *dev, const unsigned char *data, size_t length)
{
	return set_report(dev, kIOHIDReportTypeFeature, data, length);
}

int HID_API_EXPORT hid_get_feature_report(hid_device *dev, unsigned char *data, size_t length)
{
	CFIndex len = length;
    IOReturn res = 0;

	
	if (dev->disconnected)
		return -1;

	res = IOHIDDeviceGetReport(dev->device_handle,
	                           kIOHIDReportTypeFeature,
	                           data[0], 
	                           data, &len);
	if (res == kIOReturnSuccess)
		return len;
	else
		return -1;
}


void HID_API_EXPORT hid_close(hid_device *dev)
{
	if (!dev)
		return;

	
	if (!dev->disconnected) {
		IOHIDDeviceRegisterInputReportCallback(
			dev->device_handle, dev->input_report_buf, dev->max_input_report_len,
			NULL, dev);
		IOHIDDeviceRegisterRemovalCallback(dev->device_handle, NULL, dev);
		IOHIDDeviceUnscheduleFromRunLoop(dev->device_handle, dev->run_loop, dev->run_loop_mode);
		IOHIDDeviceScheduleWithRunLoop(dev->device_handle, CFRunLoopGetMain(), kCFRunLoopDefaultMode);
	}

	
	dev->shutdown_thread = 1;

	
	CFRunLoopSourceSignal(dev->source);
	CFRunLoopWakeUp(dev->run_loop);

	
	pthread_barrier_wait(&dev->shutdown_barrier);

	
	pthread_join(dev->thread, NULL);

	
	if (!dev->disconnected) {
		IOHIDDeviceClose(dev->device_handle, kIOHIDOptionsTypeSeizeDevice);
	}

	
	pthread_mutex_lock(&dev->mutex);
	while (dev->input_reports) {
		return_data(dev, NULL, 0);
	}
	pthread_mutex_unlock(&dev->mutex);
    if(dev->device_handle)
    {
        CFRelease(dev->device_handle);
        dev->device_handle = NULL;
    }

    free_hid_device(dev);
}

int HID_API_EXPORT_CALL hid_get_manufacturer_string(hid_device *dev, wchar_t *string, size_t maxlen)
{
	return get_manufacturer_string(dev->device_handle, string, maxlen);
}

int HID_API_EXPORT_CALL hid_get_product_string(hid_device *dev, wchar_t *string, size_t maxlen)
{
	return get_product_string(dev->device_handle, string, maxlen);
}

int HID_API_EXPORT_CALL hid_get_serial_number_string(hid_device *dev, wchar_t *string, size_t maxlen)
{
	return get_serial_number(dev->device_handle, string, maxlen);
}

int HID_API_EXPORT_CALL hid_get_indexed_string(hid_device *dev, int string_index, wchar_t *string, size_t maxlen)
{
	

	return 0;
}


HID_API_EXPORT const wchar_t * HID_API_CALL  hid_error(hid_device *dev)
{
	

	return NULL;
}


HID_API_EXPORT void hid_removalcbk_set(IOHIDDeviceCallback pCbk)
{
    g_device_removal_cbk = pCbk;

#if 0

    IOHIDManagerRegisterDeviceRemovalCallback(hid_mgr, pCbk, NULL);
#endif
}


HID_API_EXPORT void hid_arrivalcbk_set(IOHIDDeviceCallback pCbk)
{
    g_device_arrival_cbk = pCbk;

#if 0
    IOHIDManagerRegisterDeviceMatchingCallback(hid_mgr, pCbk, NULL);
#endif
}


#if 0
static int32_t get_usage(IOHIDDeviceRef device)
{
    int32_t res = 0;
	res = get_int_property(device, CFSTR(kIOHIDDeviceUsageKey));
	if (!res)
		res = get_int_property(device, CFSTR(kIOHIDPrimaryUsageKey));
	return res;
}

static int32_t get_usage_page(IOHIDDeviceRef device)
{
    int32_t res = 0;
	res = get_int_property(device, CFSTR(kIOHIDDeviceUsagePageKey));
	if (!res)
		res = get_int_property(device, CFSTR(kIOHIDPrimaryUsagePageKey));
	return res;
}

static int get_transport(IOHIDDeviceRef device, wchar_t *buf, size_t len)
{
	return get_string_property(device, CFSTR(kIOHIDTransportKey), buf, len);
}


int main(void)
{
    IOHIDManagerRef mgr = NULL;
    int i = 0;

	mgr = IOHIDManagerCreate(kCFAllocatorDefault, kIOHIDOptionsTypeNone);
	IOHIDManagerSetDeviceMatching(mgr, NULL);
	IOHIDManagerOpen(mgr, kIOHIDOptionsTypeNone);

	CFSetRef device_set = IOHIDManagerCopyDevices(mgr);

	CFIndex num_devices = CFSetGetCount(device_set);
	IOHIDDeviceRef *device_array = calloc(num_devices, sizeof(IOHIDDeviceRef));
    memset(device_array, 0, num_devices * sizeof(IOHIDDeviceRef));
	CFSetGetValues(device_set, (const void **) device_array);

	for (i = 0; i < num_devices; i++) {
		IOHIDDeviceRef dev = device_array[i];
		printf("Device: %p\n", dev);
		printf("  %04hx %04hx\n", get_vendor_id(dev), get_product_id(dev));

		wchar_t serial[256], buf[256];
		char cbuf[256];
		get_serial_number(dev, serial, 256);


		printf("  Serial: %ls\n", serial);
		printf("  Loc: %ld\n", get_location_id(dev));
		get_transport(dev, buf, 256);
		printf("  Trans: %ls\n", buf);
		make_path(dev, cbuf, 256);
		printf("  Path: %s\n", cbuf);

	}

	return 0;
}
#endif

#endif
