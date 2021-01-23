#include"img.hpp"

#include<stdio.h>

#if __has_include("libpng/png.h")
    #include<libpng/png.h>
#elif __has_include("libpng16/png.h")
    #include<libpng16/png.h>
#endif

#include<oXu/utils/logger.hpp>

#define PNG_SIG_BYTES_CNT 8

namespace oxu
{
    static void customPngReadFn(png_structp l_pngStruct, unsigned char* buffer, std::size_t count)
    {
        FILE *file = reinterpret_cast<FILE*>(png_get_io_ptr(l_pngStruct));
        fread(buffer, sizeof(unsigned char), count, file);
    }

    bool Image::load(const char* path)
    {
        FILE *file = fopen(path, "rb");
        if(!file)
        {
            OXU_LOG_ERR("[libpng] Can't open {}", path);
            return false;
        }

        /* read the header and fseek ahead of it */
        unsigned char header[PNG_SIG_BYTES_CNT] = {0};
        fread(header, sizeof(unsigned char), PNG_SIG_BYTES_CNT, file);
        fseek(file, PNG_SIG_BYTES_CNT * sizeof(unsigned char), SEEK_SET);

        if(png_sig_cmp(header, 0, PNG_SIG_BYTES_CNT))
        {
            fclose(file);
            OXU_LOG_ERR("[libpng] Bad signature for {}", path);
            return false;
        }

        png_structp l_pngStruct = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
        if(!l_pngStruct)
        {
            fclose(file);
            OXU_LOG_ERR("[libpng] Couldn't create png_struct for {}", path);
            return false;
        }

        png_infop l_pngInfo = png_create_info_struct(l_pngStruct);
        if(!l_pngInfo)
        {
            fclose(file);
            png_destroy_read_struct(&l_pngStruct, nullptr, nullptr);
            OXU_LOG_ERR("[libpng] Couldn't create png_info for {}", path);
            return false;
        }

        if(setjmp(png_jmpbuf(l_pngStruct)))
        {
            fclose(file);
            png_destroy_read_struct(&l_pngStruct, &l_pngInfo, nullptr);
            OXU_LOG_ERR("[libpng] Couldn't setjump pointer for {}", path);
            return false;
        }

        png_init_io(l_pngStruct, file);

        png_set_sig_bytes(l_pngStruct, PNG_SIG_BYTES_CNT);
        png_set_read_fn(l_pngStruct, file, customPngReadFn);
        png_read_info(l_pngStruct, l_pngInfo);

        png_get_IHDR(l_pngStruct, l_pngInfo, &m_width, &m_height, NULL, &m_colorType, NULL, NULL, NULL);
        png_set_interlace_handling(l_pngStruct);
        png_set_strip_16(l_pngStruct);
        png_set_packing(l_pngStruct);

        switch(m_colorType)
        {
        case PNG_COLOR_TYPE_PALETTE:
            png_set_palette_to_rgb(l_pngStruct);
            png_set_bgr(l_pngStruct);
            m_BPP = 24;
            if(png_get_valid(l_pngStruct, l_pngInfo, PNG_INFO_tRNS))
            {
                png_set_tRNS_to_alpha(l_pngStruct);
            }
            else
            {
                png_set_filler(l_pngStruct, 0xFF, PNG_FILLER_AFTER);
            }
            break;

        case PNG_COLOR_TYPE_GRAY:
            png_set_expand_gray_1_2_4_to_8(l_pngStruct);
            png_set_gray_to_rgb(l_pngStruct);
            png_set_bgr(l_pngStruct);
            break;

        case PNG_COLOR_TYPE_GRAY_ALPHA:
            png_set_gray_to_rgb(l_pngStruct);
            break;

        case PNG_COLOR_TYPE_RGB:
            png_set_bgr(l_pngStruct);
            png_set_filler(l_pngStruct, 0xFF, PNG_FILLER_AFTER);
            m_BPP = 24;
            break;

        case PNG_COLOR_TYPE_RGBA:
            m_BPP = 32;
            break;

        default:
            OXU_LOG_ERR("[libpng] Unsupported png color type {}", m_colorType);
            return false;
        }

        png_read_update_info(l_pngStruct, l_pngInfo);

        m_buffer.clear();
        m_buffer.resize(m_width * m_height * 4);
        if(m_buffer.capacity() > m_buffer.size())
        {
            m_buffer.shrink_to_fit();
        }
        
        unsigned char **lpp_rows = new unsigned char*[m_height];

        for(size_t i = 0; i < m_height; ++i)
        {
            lpp_rows[m_height - 1 - i] = &m_buffer[0] + (i * m_width * 4);
        }

        png_read_image(l_pngStruct, &lpp_rows[0]);

        delete[] lpp_rows;
        png_destroy_read_struct(&l_pngStruct, &l_pngInfo, nullptr);
        fclose(file);

        return true;
    }
}