#include "osystem.h"

using namespace std;

int main (int argc_, char *argv_[])
{
    try
    {
        osystem_t osystem;
        osystem.init (argc_, argv_);

        while (osystem.run ())
        {
        }
    }
    catch (exception const &e_)
    {
    }

    return 0;
}
