#include "osystem.h"

#include <plog/Log.h>

using namespace std;

int main (int argc_, char *argv_[])
{
    osystem_t osystem;
    osystem.init (argc_, argv_);

    while (osystem.run ())
    {
    }

    return 0;
}
