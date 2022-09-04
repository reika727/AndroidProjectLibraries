#include"image_utility.hpp"
#include<stdexcept>

image_utility::image_accessor_YUV_420_888::image_accessor_YUV_420_888(JNIEnv *const env, const jobject &image)
{
    jclass class_image = env->GetObjectClass(image);
    jobjectArray plane_array = static_cast<jobjectArray>(
            env->CallObjectMethod(image, env->GetMethodID(class_image, "getPlanes", "()[Landroid/media/Image$Plane;"))
    );
    jobject y_plane = env->GetObjectArrayElement(plane_array, 0);
    jobject u_plane = env->GetObjectArrayElement(plane_array, 1);
    jobject v_plane = env->GetObjectArrayElement(plane_array, 2);
    jclass class_plane = env->GetObjectClass(y_plane);
    jmethodID get_buffer = env->GetMethodID(class_plane, "getBuffer", "()Ljava/nio/ByteBuffer;");
    jmethodID get_row_stride = env->GetMethodID(class_plane, "getRowStride", "()I");
    width = env->CallIntMethod(image, env->GetMethodID(class_image, "getWidth", "()I"));
    height = env->CallIntMethod(image, env->GetMethodID(class_image, "getHeight", "()I"));
    y_ptr = reinterpret_cast<uint8_t *>(env->GetDirectBufferAddress(env->CallObjectMethod(y_plane, get_buffer)));
    u_ptr = reinterpret_cast<uint8_t *>(env->GetDirectBufferAddress(env->CallObjectMethod(u_plane, get_buffer)));
    v_ptr = reinterpret_cast<uint8_t *>(env->GetDirectBufferAddress(env->CallObjectMethod(v_plane, get_buffer)));
    y_row_stride = env->CallIntMethod(y_plane, get_row_stride);
    uv_row_stride = env->CallIntMethod(u_plane, get_row_stride);
    uv_pixel_stride = env->CallIntMethod(u_plane, env->GetMethodID(class_plane, "getPixelStride", "()I"));
    env->DeleteLocalRef(class_plane);
    env->DeleteLocalRef(v_plane);
    env->DeleteLocalRef(u_plane);
    env->DeleteLocalRef(y_plane);
    env->DeleteLocalRef(plane_array);
    env->DeleteLocalRef(class_image);
}

image_utility::surface_texture_accessor_R8G8B8X8::surface_texture_accessor_R8G8B8X8(JNIEnv *const env, const jobject &_surface_texture)
        : surface_texture(ASurfaceTexture_fromSurfaceTexture(env, _surface_texture)),
          window(ASurfaceTexture_acquireANativeWindow(surface_texture)),
          width(ANativeWindow_getWidth(window)), height(ANativeWindow_getHeight(window))
{
    ANativeWindow_setBuffersGeometry(window, 0, 0, AHARDWAREBUFFER_FORMAT_R8G8B8X8_UNORM);
    if (ANativeWindow_lock(window, &window_buffer, NULL)) {
        ANativeWindow_release(window);
        ASurfaceTexture_release(surface_texture);
        throw std::runtime_error("lock failed.");
    }
}

image_utility::surface_texture_accessor_R8G8B8X8::~surface_texture_accessor_R8G8B8X8()
{
    ANativeWindow_unlockAndPost(window);
    ANativeWindow_release(window);
    ASurfaceTexture_release(surface_texture);
}

image_utility::coordinate_transformer::coordinate_transformer(
        const image_utility::image_accessor_YUV_420_888 &ia,
        const image_utility::surface_texture_accessor_R8G8B8X8 &sa,
        bool is_landscape
) : image_height(ia.get_height()), padding_x(0), padding_y(0), is_landscape(is_landscape)
{
    if (is_landscape) {
        if (ia.get_width() * sa.get_height() < image_height * sa.get_width())
            padding_x = (sa.get_width() - ia.get_width()) / 2;
        else
            padding_y = (sa.get_height() - image_height) / 2;
    } else {
        if (image_height * sa.get_height() < ia.get_width() * sa.get_width())
            padding_x = (sa.get_width() - image_height) / 2;
        else
            padding_y = (sa.get_height() - ia.get_width()) / 2;
    }
}