#include <wiringPi.h>
#include "Onkyo_send_blocking/OnkyoRI.h"

int main() {
   wiringPiSetup();

 OnkyoRI ori(22);
 ori.send(0x1AE);
}
