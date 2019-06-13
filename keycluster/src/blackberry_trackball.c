#include "fsl_gpio.h"
#include "fsl_port.h"
#include "blackberry_trackball.h"

pointer_delta_t BlackBerryTrackball_PointerDelta;

void BlackberryTrackball_Init(void)
{
    CLOCK_EnableClock(BLACKBERRY_TRACKBALL_LEFT_CLOCK);
    PORT_SetPinMux(BLACKBERRY_TRACKBALL_LEFT_PORT, BLACKBERRY_TRACKBALL_LEFT_PIN, kPORT_MuxAsGpio);
    PORT_SetPinInterruptConfig(BLACKBERRY_TRACKBALL_LEFT_PORT, BLACKBERRY_TRACKBALL_LEFT_PIN, kPORT_InterruptEitherEdge);
    NVIC_EnableIRQ(BLACKBERRY_TRACKBALL_LEFT_IRQ);

    CLOCK_EnableClock(BLACKBERRY_TRACKBALL_RIGHT_CLOCK);
    PORT_SetPinMux(BLACKBERRY_TRACKBALL_RIGHT_PORT, BLACKBERRY_TRACKBALL_RIGHT_PIN, kPORT_MuxAsGpio);
    PORT_SetPinInterruptConfig(BLACKBERRY_TRACKBALL_RIGHT_PORT, BLACKBERRY_TRACKBALL_RIGHT_PIN, kPORT_InterruptEitherEdge);
    NVIC_EnableIRQ(BLACKBERRY_TRACKBALL_RIGHT_IRQ);

    CLOCK_EnableClock(BLACKBERRY_TRACKBALL_TOP_CLOCK);
    PORT_SetPinMux(BLACKBERRY_TRACKBALL_TOP_PORT, BLACKBERRY_TRACKBALL_TOP_PIN, kPORT_MuxAsGpio);
    PORT_SetPinInterruptConfig(BLACKBERRY_TRACKBALL_TOP_PORT, BLACKBERRY_TRACKBALL_TOP_PIN, kPORT_InterruptEitherEdge);
    NVIC_EnableIRQ(BLACKBERRY_TRACKBALL_TOP_IRQ);

    CLOCK_EnableClock(BLACKBERRY_TRACKBALL_BOTTOM_CLOCK);
    PORT_SetPinMux(BLACKBERRY_TRACKBALL_BOTTOM_PORT, BLACKBERRY_TRACKBALL_BOTTOM_PIN, kPORT_MuxAsGpio);
    PORT_SetPinInterruptConfig(BLACKBERRY_TRACKBALL_BOTTOM_PORT, BLACKBERRY_TRACKBALL_BOTTOM_PIN, kPORT_InterruptEitherEdge);
    NVIC_EnableIRQ(BLACKBERRY_TRACKBALL_BOTTOM_IRQ);
}

void handleTrackball(void)
{
    if (PORT_GetPinsInterruptFlags(BLACKBERRY_TRACKBALL_LEFT_PORT) & (1 << BLACKBERRY_TRACKBALL_LEFT_PIN)) {
        BlackBerryTrackball_PointerDelta.x++;
        GPIO_ClearPinsInterruptFlags(BLACKBERRY_TRACKBALL_LEFT_GPIO, 1U << BLACKBERRY_TRACKBALL_LEFT_PIN);
    }

    if (PORT_GetPinsInterruptFlags(BLACKBERRY_TRACKBALL_RIGHT_PORT) & (1 << BLACKBERRY_TRACKBALL_RIGHT_PIN)) {
        BlackBerryTrackball_PointerDelta.x--;
        GPIO_ClearPinsInterruptFlags(BLACKBERRY_TRACKBALL_RIGHT_GPIO, 1U << BLACKBERRY_TRACKBALL_RIGHT_PIN);
    }

    if (PORT_GetPinsInterruptFlags(BLACKBERRY_TRACKBALL_TOP_PORT) & (1 << BLACKBERRY_TRACKBALL_TOP_PIN)) {
        BlackBerryTrackball_PointerDelta.y++;
        GPIO_ClearPinsInterruptFlags(BLACKBERRY_TRACKBALL_TOP_GPIO, 1U << BLACKBERRY_TRACKBALL_TOP_PIN);
    }

    if (PORT_GetPinsInterruptFlags(BLACKBERRY_TRACKBALL_BOTTOM_PORT) & (1 << BLACKBERRY_TRACKBALL_BOTTOM_PIN)) {
        BlackBerryTrackball_PointerDelta.y--;
        GPIO_ClearPinsInterruptFlags(BLACKBERRY_TRACKBALL_BOTTOM_GPIO, 1U << BLACKBERRY_TRACKBALL_BOTTOM_PIN);
    }
}

void BLACKBERRY_TRACKBALL_IRQ_HANDLER1(void)
{
    handleTrackball();
}

void BLACKBERRY_TRACKBALL_IRQ_HANDLER2(void)
{
    handleTrackball();
}