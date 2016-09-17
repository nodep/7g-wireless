#pragma once

#include "usbdrv.h"

void vusb_init(void);

bool vusb_poll(void);			// returns true if the idle duration has expired
void vusb_reset_idle(void);		// resets the idle duration
