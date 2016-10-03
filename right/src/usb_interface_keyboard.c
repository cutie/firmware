#include "fsl_port.h"
#include "include/board/board.h"
#include "usb_api.h"
#include "usb_composite_device.h"
#include "test_led.h"

static usb_device_endpoint_struct_t UsbKeyboardEndpoints[USB_KEYBOARD_ENDPOINT_COUNT] = {{
    USB_KEYBOARD_ENDPOINT_INDEX | (USB_IN << USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_SHIFT),
    USB_ENDPOINT_INTERRUPT,
    USB_KEYBOARD_INTERRUPT_IN_PACKET_SIZE,
}};

static usb_device_interface_struct_t UsbKeyboardInterface[] = {{
    USB_INTERFACE_ALTERNATE_SETTING_NONE,
    {USB_KEYBOARD_ENDPOINT_COUNT, UsbKeyboardEndpoints},
    NULL,
}};

static usb_device_interfaces_struct_t UsbKeyboardInterfaces[USB_KEYBOARD_INTERFACE_COUNT] = {{
    USB_CLASS_HID,
    USB_HID_SUBCLASS_BOOT,
    USB_HID_PROTOCOL_KEYBOARD,
    USB_KEYBOARD_INTERFACE_INDEX,
    UsbKeyboardInterface,
    sizeof(UsbKeyboardInterface) / sizeof(usb_device_interfaces_struct_t),
}};

static usb_device_interface_list_t UsbKeyboardInterfaceList[USB_DEVICE_CONFIGURATION_COUNT] = {{
    USB_KEYBOARD_INTERFACE_COUNT,
    UsbKeyboardInterfaces,
}};

usb_device_class_struct_t UsbKeyboardClass = {
    UsbKeyboardInterfaceList,
    kUSB_DeviceClassTypeHid,
    USB_DEVICE_CONFIGURATION_COUNT,
};

static usb_keyboard_report_t UsbKeyboardReport;

typedef struct {
    PORT_Type *port;
    GPIO_Type *gpio;
    uint32_t pin;
} pin_t;

#define KEYBOARD_MATRIX_COLS_NUM 7
#define KEYBOARD_MATRIX_ROWS_NUM 5

pin_t keyboardMatrixCols[] = {
    {PORTA, GPIOA, 5},
    {PORTB, GPIOB, 3},
    {PORTB, GPIOB, 16},
    {PORTB, GPIOB, 17},
    {PORTB, GPIOB, 18},
    {PORTA, GPIOA, 1},
    {PORTB, GPIOB, 0},
};

pin_t keyboardMatrixRows[] = {
    {PORTA, GPIOA, 12},
    {PORTA, GPIOA, 13},
    {PORTC, GPIOC, 0},
    {PORTB, GPIOB, 19},
    {PORTD, GPIOD, 6},
};

static usb_status_t UsbKeyboardAction(void)
{
    UsbKeyboardReport.modifiers = 0;
    UsbKeyboardReport.reserved = 0;

    for (uint8_t scancode_idx=0; scancode_idx<USB_KEYBOARD_MAX_KEYS; scancode_idx++) {
        UsbKeyboardReport.scancodes[scancode_idx] = 0;
    }

    // row 1: PA12
    // col 1: PA5

    PORT_SetPinConfig(keyboardMatrixCols[0].port, keyboardMatrixCols[0].pin, &(port_pin_config_t){.pullSelect=kPORT_PullDisable, .mux=kPORT_MuxAsGpio});
    GPIO_PinInit(keyboardMatrixCols[0].gpio, keyboardMatrixCols[0].pin, &(gpio_pin_config_t){.pinDirection=kGPIO_DigitalOutput, .outputLogic=1});
    GPIO_WritePinOutput(keyboardMatrixCols[0].gpio, keyboardMatrixCols[0].pin, 1);

    PORT_SetPinConfig(keyboardMatrixRows[0].port, keyboardMatrixRows[0].pin, &(port_pin_config_t){.pullSelect=kPORT_PullDown, .mux=kPORT_MuxAsGpio});
    GPIO_PinInit(keyboardMatrixRows[0].gpio, keyboardMatrixRows[0].pin, &(gpio_pin_config_t){.pinDirection=kGPIO_DigitalInput});
    if (GPIO_ReadPinInput(keyboardMatrixRows[0].gpio, keyboardMatrixRows[0].pin)) {
        GPIO_SetPinsOutput(TEST_LED_GPIO, 1 << TEST_LED_GPIO_PIN);
        UsbKeyboardReport.scancodes[0] = HID_KEYBOARD_SC_A;
    }

    return USB_DeviceHidSend(UsbCompositeDevice.keyboardHandle, USB_KEYBOARD_ENDPOINT_INDEX,
                             (uint8_t*)&UsbKeyboardReport, USB_KEYBOARD_REPORT_LENGTH);
}

usb_status_t UsbKeyboardCallback(class_handle_t handle, uint32_t event, void *param)
{
    usb_status_t error = kStatus_USB_Error;

    switch (event) {
        case kUSB_DeviceHidEventSendResponse:
            if (UsbCompositeDevice.attach) {
                return UsbKeyboardAction();
            }
            break;
        case kUSB_DeviceHidEventGetReport:
        case kUSB_DeviceHidEventSetReport:
        case kUSB_DeviceHidEventRequestReportBuffer:
            error = kStatus_USB_InvalidRequest;
            break;
        case kUSB_DeviceHidEventGetIdle:
        case kUSB_DeviceHidEventGetProtocol:
        case kUSB_DeviceHidEventSetIdle:
        case kUSB_DeviceHidEventSetProtocol:
            break;
        default:
            break;
    }

    return error;
}

usb_status_t UsbKeyboardSetConfiguration(class_handle_t handle, uint8_t configuration)
{
    if (USB_COMPOSITE_CONFIGURATION_INDEX == configuration) {
        return UsbKeyboardAction();
    }
    return kStatus_USB_Error;
}

usb_status_t UsbKeyboardSetInterface(class_handle_t handle, uint8_t interface, uint8_t alternateSetting)
{
    if (USB_KEYBOARD_INTERFACE_INDEX == interface) {
        return UsbKeyboardAction();
    }
    return kStatus_USB_Error;
}