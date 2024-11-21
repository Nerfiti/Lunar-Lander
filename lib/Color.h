#pragma once

#include <stdint.h>

class Color final
{
    public:
        Color(uint32_t color);
        Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);

        Color &set_red  (uint8_t red);
        Color &set_green(uint8_t green);
        Color &set_blue (uint8_t blue);
        Color &set_alpha(uint8_t alpha);

        Color &set_color(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);

        uint8_t get_red  () const;
        uint8_t get_green() const;
        uint8_t get_blue () const;
        uint8_t get_alpha() const;

        uint32_t get_color() const;

        operator uint32_t() const;

        Color blend(Color other) const;

    private:
        uint32_t color_;

        static constexpr uint8_t Blue_offset_  = 0 * 8;
        static constexpr uint8_t Green_offset_ = 1 * 8;
        static constexpr uint8_t Red_offset_   = 2 * 8;
        static constexpr uint8_t Alpha_offset_ = 3 * 8;

        static constexpr uint32_t Byte_mask_  = 0xFF;
        static constexpr uint32_t Red_mask_   = Byte_mask_ << Red_offset_;
        static constexpr uint32_t Green_mask_ = Byte_mask_ << Green_offset_;
        static constexpr uint32_t Blue_mask_  = Byte_mask_ << Blue_offset_;
        static constexpr uint32_t Alpha_mask_ = Byte_mask_ << Alpha_offset_;

    public:
        static constexpr uint32_t Black  = (Byte_mask_ << Alpha_offset_);
        static constexpr uint32_t Red    = (Byte_mask_ << Red_offset_  ) | Black;
        static constexpr uint32_t Green  = (Byte_mask_ << Green_offset_) | Black;
        static constexpr uint32_t Blue   = (Byte_mask_ << Blue_offset_ ) | Black;
        static constexpr uint32_t White  = Red | Green | Blue;
        static constexpr uint32_t Yellow = Red | Green;
        static constexpr uint32_t Purple = Red | Blue;
        static constexpr uint32_t Cyan   = Green | Blue;
};