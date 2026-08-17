#pragma once

#define SCREEN_WIDTH             80
#define SCREEN_HEIGHT            160

#define PHANTOM_NUM              3
#define PHANTOM_PINS             {37, 36, 35}
#define PHANTOM_ID_IN1           0
#define PHANTOM_ID_IN2           1
#define PHANTOM_ID_IN3           2
#define PHANTOM_ACTIVATION_SPEED 1.0f

#define OUT_SPAN_VOLTS           (1.2258f)

#define BLOB_SIZE                1024
#define CMD_BUF_SIZE             (BLOB_SIZE + 20)
