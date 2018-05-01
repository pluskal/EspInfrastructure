#include <secrets.h>
#include <infrastructure.h>

Infrastructure infrastructure;

void setup()
{
    infrastructure.initialize(115200, new wlanNet_t{ssid, password}, "esp32");
}

void loop()
{
    infrastructure.loop();
}
