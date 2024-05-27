#include "texture.h"

using namespace std;

class texture_t::private_t
{
public:
    ~private_t ()
    {
        if (bgfx::isValid (handle))
            bgfx::destroy (handle);
    };

    private_t ()
        : size ()
        , handle (BGFX_INVALID_HANDLE)
        , palette (nullopt)
    {
    }
    private_t (glm::tvec2<uint16_t> const &size_)
        : size (size_)
        , handle (BGFX_INVALID_HANDLE)
        , palette (nullopt)
    {
    }

    glm::tvec2<uint16_t> size;
    bgfx::TextureHandle handle;
    optional<texture_t> palette;
};

texture_t::~texture_t () = default;
texture_t::texture_t ()
    : m_d (nullptr)
{
}

texture_t::texture_t (glm::tvec2<uint16_t> const &size_)
    : m_d (spimpl::make_impl<private_t> (size_))
{
    init ();
}

texture_t::texture_t (preset_t preset_)
    : m_d (spimpl::make_impl<private_t> ())
{
    switch (preset_)
    {
    case FULLSCREEN:
        m_d->size = {320, 200};
        break;
    case PALETTE:
        m_d->size = {3, 256};
    };

    init ();
}

glm::tvec2<uint16_t> const &texture_t::size () const
{
    return m_d->size;
}

bgfx::TextureHandle const &texture_t::handle () const
{
    return m_d->handle;
}

optional<texture_t> const &texture_t::palette () const
{
    return m_d->palette;
}

void texture_t::update (std::vector<std::byte> const &data_,
                        uint16_t offset_,
                        uint16_t range_)
{
    bgfx::updateTexture2D (m_d->handle,
                           0,
                           0,
                           0,
                           0,
                           m_d->size.x,
                           m_d->size.y,
                           bgfx::copy (data_.data () + offset_,
                                       (range_ == 0 ? data_.size () - offset_ : range_)));
}

void texture_t::fill (uint8_t color_, glm::tvec4<uint16_t> rect_)
{
    vector<byte> const color (rect_.z * rect_.w, byte (color_));
    bgfx::updateTexture2D (m_d->handle,
                           0,
                           0,
                           rect_.x,
                           rect_.t,
                           m_d->size.x,
                           m_d->size.y,
                           bgfx::copy (color.data (), color.size ()));
}

void texture_t::fill (uint8_t color_)
{
    vector<byte> const color (m_d->size.x * m_d->size.y, byte (color_));
    bgfx::updateTexture2D (m_d->handle,
                           0,
                           0,
                           0,
                           0,
                           m_d->size.x,
                           m_d->size.y,
                           bgfx::copy (color.data (), color.size ()));
}

void texture_t::setPalette (texture_t const &palette_)
{
    m_d->palette = palette_;
}

bool texture_t::isValid () const
{
    return (bool)m_d;
}

bool texture_t::hasPalette () const
{
    return m_d->palette.has_value ();
}

void texture_t::init ()
{
    m_d->handle = bgfx::createTexture2D (
        m_d->size.x, m_d->size.y, false, 1, bgfx::TextureFormat::R8U);
}
