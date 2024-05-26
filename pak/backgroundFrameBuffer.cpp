#include "backgroundFrameBuffer.h"

#include "osystem.h"

#include <plog/Log.h>

using namespace std;

class backgroundFrameBuffer_t::private_t
{
public:
    ~private_t () {}

    private_t () {}
};

backgroundFrameBuffer_t::~backgroundFrameBuffer_t () = default;

backgroundFrameBuffer_t::backgroundFrameBuffer_t ()
    : frameBuffer_t (1)
    , m_d (make_unique<private_t> ())
{
}

void backgroundFrameBuffer_t::startFrame ()
{
}

void backgroundFrameBuffer_t::_shutdown ()
{
}
