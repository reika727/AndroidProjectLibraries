#include <jni.h>
#include <android/native_window.h>
#include <android/native_window_jni.h>
#include <android/surface_texture.h>
#include <android/surface_texture_jni.h>
#include <algorithm>

namespace image_utility {
    enum class YUV {
        Y, U, V
    };
    enum class RGB {
        R, G, B
    };

    class image_accessor_YUV_420_888 {
    private:
        jint width, height;
        uint8_t *y_ptr, *u_ptr, *v_ptr;
        jint y_row_stride, uv_row_stride, uv_pixel_stride;
    public:
        image_accessor_YUV_420_888(JNIEnv *const env, const jobject &image);

        inline jint get_width() const noexcept
        {
            return width;
        }

        inline jint get_height() const noexcept
        {
            return height;
        }

        inline uint8_t operator()(int x, int y, YUV c) const noexcept
        {
            return x < 0 || y < 0 || x >= width || y >= height ? static_cast<uint8_t >(0) :
                   c == YUV::Y ? y_ptr[y * y_row_stride + x] :
                   (c == YUV::U ? u_ptr : v_ptr)[(y >> 1) * uv_row_stride + (x >> 1) * uv_pixel_stride];
        }
    };

    class surface_texture_accessor_R8G8B8X8 {
    private:
        ASurfaceTexture *surface_texture;
        ANativeWindow *window;
        ANativeWindow_Buffer window_buffer;
        int32_t width, height;
    public:
        surface_texture_accessor_R8G8B8X8(JNIEnv *const env, const jobject &surface_texture);

        ~surface_texture_accessor_R8G8B8X8();

        inline int32_t get_width() const noexcept
        {
            return width;
        }

        inline int32_t get_height() const noexcept
        {
            return height;
        }

        inline uint8_t &operator()(int x, int y, RGB c) const noexcept
        {
            return reinterpret_cast<uint8_t *>(window_buffer.bits)[(y * window_buffer.stride + x) * 4 + static_cast<int>(c)];
        }
    };

    class coordinate_transformer {
    private:
        int image_height;
        int padding_x, padding_y;
        bool is_landscape;
        struct coordinate {
            int x, y;
        };
    public:
        coordinate_transformer(const image_accessor_YUV_420_888 &ia, const surface_texture_accessor_R8G8B8X8 &sa, bool is_landscape);

        inline coordinate operator()(int x, int y) const noexcept
        {
            return {
                    (is_landscape ? x : image_height - 1 - y) + padding_x,
                    (is_landscape ? y : x) + padding_y
            };
        }
    };

    class YUV_to_RGB {
    private:
        struct _RGB {
            int R, G, B;
        };
    public:
        inline static _RGB convert(int Y, int U, int V) noexcept
        {
            /*
              Kr = 0.2126, Kb = 0.0722, Kg = 1-Kr-Kb (ITU-R BT.709)
              M =
              {
                       Kr           Kg          Kb
                  -Kr/2(Kr+Kg) -Kg/2(Kr+Kg)     1/2
                      1/2      -Kg/2(Kg+Kb) -Kb/2(Kg+Kb)
              }
              M^-1 =
              {
                  1       0          2(1-Kr)
                  1 -2(1-Kb)Kb/Kg -2(1-Kr)Kr/Kg
                  1    2(1-Kb)          0
              }
              =
              {
                  1        0            3937/2500
                  1 -1674679/8940000 -4185031/8940000
                  1    4639/2500            0
              }
            */
            return {
                    std::clamp(Y + 3937 * V / 2500, 0, 255),
                    std::clamp(Y - (1674679 * U + 4185031 * V) / 8940000, 0, 255),
                    std::clamp(Y + 4639 * U / 2500, 0, 255)
            };
        }
    };
}