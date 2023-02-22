#include <thread>
#include "gps.hpp"

int main()
{
    GPS gps = GPS();
    if (gps.init("/dev/ttyACM0", Serial::eB9600))
    {
        /* Execute GPS thread */
        gps.start();
    }
}