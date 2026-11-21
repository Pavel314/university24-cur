// machine generated, do not edit
using System;
using System.Runtime.InteropServices;
using M = System.Runtime.InteropServices.MarshalAsAttribute;
using U = System.Runtime.InteropServices.UnmanagedType;

using static Sokol.SG;

using static Sokol.SApp;

namespace Sokol
{
public static unsafe partial class SGImgui
{
[StructLayout(LayoutKind.Sequential)]
public struct sgimgui_allocator_t
{
    public delegate* unmanaged<nuint, void*, void*> alloc_fn;
    public delegate* unmanaged<void*, void*, void> free_fn;
    public void* user_data;
}
[StructLayout(LayoutKind.Sequential)]
public struct sgimgui_desc_t
{
    public sgimgui_allocator_t allocator;
}
#if __IOS__
[DllImport("@rpath/sokol.framework/sokol", EntryPoint = "sgimgui_setup", CallingConvention = CallingConvention.Cdecl)]
#else
[DllImport("sokol", EntryPoint = "sgimgui_setup", CallingConvention = CallingConvention.Cdecl)]
#endif
public static extern void sgimgui_setup(in sgimgui_desc_t desc);

#if __IOS__
[DllImport("@rpath/sokol.framework/sokol", EntryPoint = "sgimgui_shutdown", CallingConvention = CallingConvention.Cdecl)]
#else
[DllImport("sokol", EntryPoint = "sgimgui_shutdown", CallingConvention = CallingConvention.Cdecl)]
#endif
public static extern void sgimgui_shutdown();

#if __IOS__
[DllImport("@rpath/sokol.framework/sokol", EntryPoint = "sgimgui_draw", CallingConvention = CallingConvention.Cdecl)]
#else
[DllImport("sokol", EntryPoint = "sgimgui_draw", CallingConvention = CallingConvention.Cdecl)]
#endif
public static extern void sgimgui_draw();

#if __IOS__
[DllImport("@rpath/sokol.framework/sokol", EntryPoint = "sgimgui_draw_menu", CallingConvention = CallingConvention.Cdecl)]
#else
[DllImport("sokol", EntryPoint = "sgimgui_draw_menu", CallingConvention = CallingConvention.Cdecl)]
#endif
public static extern void sgimgui_draw_menu([M(U.LPUTF8Str)] string title);

#if __IOS__
[DllImport("@rpath/sokol.framework/sokol", EntryPoint = "sgimgui_draw_buffer_window_content", CallingConvention = CallingConvention.Cdecl)]
#else
[DllImport("sokol", EntryPoint = "sgimgui_draw_buffer_window_content", CallingConvention = CallingConvention.Cdecl)]
#endif
public static extern void sgimgui_draw_buffer_window_content();

#if __IOS__
[DllImport("@rpath/sokol.framework/sokol", EntryPoint = "sgimgui_draw_image_window_content", CallingConvention = CallingConvention.Cdecl)]
#else
[DllImport("sokol", EntryPoint = "sgimgui_draw_image_window_content", CallingConvention = CallingConvention.Cdecl)]
#endif
public static extern void sgimgui_draw_image_window_content();

#if __IOS__
[DllImport("@rpath/sokol.framework/sokol", EntryPoint = "sgimgui_draw_sampler_window_content", CallingConvention = CallingConvention.Cdecl)]
#else
[DllImport("sokol", EntryPoint = "sgimgui_draw_sampler_window_content", CallingConvention = CallingConvention.Cdecl)]
#endif
public static extern void sgimgui_draw_sampler_window_content();

#if __IOS__
[DllImport("@rpath/sokol.framework/sokol", EntryPoint = "sgimgui_draw_shader_window_content", CallingConvention = CallingConvention.Cdecl)]
#else
[DllImport("sokol", EntryPoint = "sgimgui_draw_shader_window_content", CallingConvention = CallingConvention.Cdecl)]
#endif
public static extern void sgimgui_draw_shader_window_content();

#if __IOS__
[DllImport("@rpath/sokol.framework/sokol", EntryPoint = "sgimgui_draw_pipeline_window_content", CallingConvention = CallingConvention.Cdecl)]
#else
[DllImport("sokol", EntryPoint = "sgimgui_draw_pipeline_window_content", CallingConvention = CallingConvention.Cdecl)]
#endif
public static extern void sgimgui_draw_pipeline_window_content();

#if __IOS__
[DllImport("@rpath/sokol.framework/sokol", EntryPoint = "sgimgui_draw_view_window_content", CallingConvention = CallingConvention.Cdecl)]
#else
[DllImport("sokol", EntryPoint = "sgimgui_draw_view_window_content", CallingConvention = CallingConvention.Cdecl)]
#endif
public static extern void sgimgui_draw_view_window_content();

#if __IOS__
[DllImport("@rpath/sokol.framework/sokol", EntryPoint = "sgimgui_draw_capture_window_content", CallingConvention = CallingConvention.Cdecl)]
#else
[DllImport("sokol", EntryPoint = "sgimgui_draw_capture_window_content", CallingConvention = CallingConvention.Cdecl)]
#endif
public static extern void sgimgui_draw_capture_window_content();

#if __IOS__
[DllImport("@rpath/sokol.framework/sokol", EntryPoint = "sgimgui_draw_capabilities_window_content", CallingConvention = CallingConvention.Cdecl)]
#else
[DllImport("sokol", EntryPoint = "sgimgui_draw_capabilities_window_content", CallingConvention = CallingConvention.Cdecl)]
#endif
public static extern void sgimgui_draw_capabilities_window_content();

#if __IOS__
[DllImport("@rpath/sokol.framework/sokol", EntryPoint = "sgimgui_draw_frame_stats_window_content", CallingConvention = CallingConvention.Cdecl)]
#else
[DllImport("sokol", EntryPoint = "sgimgui_draw_frame_stats_window_content", CallingConvention = CallingConvention.Cdecl)]
#endif
public static extern void sgimgui_draw_frame_stats_window_content();

#if __IOS__
[DllImport("@rpath/sokol.framework/sokol", EntryPoint = "sgimgui_draw_buffer_window", CallingConvention = CallingConvention.Cdecl)]
#else
[DllImport("sokol", EntryPoint = "sgimgui_draw_buffer_window", CallingConvention = CallingConvention.Cdecl)]
#endif
public static extern void sgimgui_draw_buffer_window([M(U.LPUTF8Str)] string title);

#if __IOS__
[DllImport("@rpath/sokol.framework/sokol", EntryPoint = "sgimgui_draw_image_window", CallingConvention = CallingConvention.Cdecl)]
#else
[DllImport("sokol", EntryPoint = "sgimgui_draw_image_window", CallingConvention = CallingConvention.Cdecl)]
#endif
public static extern void sgimgui_draw_image_window([M(U.LPUTF8Str)] string title);

#if __IOS__
[DllImport("@rpath/sokol.framework/sokol", EntryPoint = "sgimgui_draw_sampler_window", CallingConvention = CallingConvention.Cdecl)]
#else
[DllImport("sokol", EntryPoint = "sgimgui_draw_sampler_window", CallingConvention = CallingConvention.Cdecl)]
#endif
public static extern void sgimgui_draw_sampler_window([M(U.LPUTF8Str)] string title);

#if __IOS__
[DllImport("@rpath/sokol.framework/sokol", EntryPoint = "sgimgui_draw_shader_window", CallingConvention = CallingConvention.Cdecl)]
#else
[DllImport("sokol", EntryPoint = "sgimgui_draw_shader_window", CallingConvention = CallingConvention.Cdecl)]
#endif
public static extern void sgimgui_draw_shader_window([M(U.LPUTF8Str)] string title);

#if __IOS__
[DllImport("@rpath/sokol.framework/sokol", EntryPoint = "sgimgui_draw_pipeline_window", CallingConvention = CallingConvention.Cdecl)]
#else
[DllImport("sokol", EntryPoint = "sgimgui_draw_pipeline_window", CallingConvention = CallingConvention.Cdecl)]
#endif
public static extern void sgimgui_draw_pipeline_window([M(U.LPUTF8Str)] string title);

#if __IOS__
[DllImport("@rpath/sokol.framework/sokol", EntryPoint = "sgimgui_draw_view_window", CallingConvention = CallingConvention.Cdecl)]
#else
[DllImport("sokol", EntryPoint = "sgimgui_draw_view_window", CallingConvention = CallingConvention.Cdecl)]
#endif
public static extern void sgimgui_draw_view_window([M(U.LPUTF8Str)] string title);

#if __IOS__
[DllImport("@rpath/sokol.framework/sokol", EntryPoint = "sgimgui_draw_capture_window", CallingConvention = CallingConvention.Cdecl)]
#else
[DllImport("sokol", EntryPoint = "sgimgui_draw_capture_window", CallingConvention = CallingConvention.Cdecl)]
#endif
public static extern void sgimgui_draw_capture_window([M(U.LPUTF8Str)] string title);

#if __IOS__
[DllImport("@rpath/sokol.framework/sokol", EntryPoint = "sgimgui_draw_capabilities_window", CallingConvention = CallingConvention.Cdecl)]
#else
[DllImport("sokol", EntryPoint = "sgimgui_draw_capabilities_window", CallingConvention = CallingConvention.Cdecl)]
#endif
public static extern void sgimgui_draw_capabilities_window([M(U.LPUTF8Str)] string title);

#if __IOS__
[DllImport("@rpath/sokol.framework/sokol", EntryPoint = "sgimgui_draw_frame_stats_window", CallingConvention = CallingConvention.Cdecl)]
#else
[DllImport("sokol", EntryPoint = "sgimgui_draw_frame_stats_window", CallingConvention = CallingConvention.Cdecl)]
#endif
public static extern void sgimgui_draw_frame_stats_window([M(U.LPUTF8Str)] string title);

#if __IOS__
[DllImport("@rpath/sokol.framework/sokol", EntryPoint = "sgimgui_draw_buffer_menu_item", CallingConvention = CallingConvention.Cdecl)]
#else
[DllImport("sokol", EntryPoint = "sgimgui_draw_buffer_menu_item", CallingConvention = CallingConvention.Cdecl)]
#endif
public static extern void sgimgui_draw_buffer_menu_item([M(U.LPUTF8Str)] string label);

#if __IOS__
[DllImport("@rpath/sokol.framework/sokol", EntryPoint = "sgimgui_draw_image_menu_item", CallingConvention = CallingConvention.Cdecl)]
#else
[DllImport("sokol", EntryPoint = "sgimgui_draw_image_menu_item", CallingConvention = CallingConvention.Cdecl)]
#endif
public static extern void sgimgui_draw_image_menu_item([M(U.LPUTF8Str)] string label);

#if __IOS__
[DllImport("@rpath/sokol.framework/sokol", EntryPoint = "sgimgui_draw_sampler_menu_item", CallingConvention = CallingConvention.Cdecl)]
#else
[DllImport("sokol", EntryPoint = "sgimgui_draw_sampler_menu_item", CallingConvention = CallingConvention.Cdecl)]
#endif
public static extern void sgimgui_draw_sampler_menu_item([M(U.LPUTF8Str)] string label);

#if __IOS__
[DllImport("@rpath/sokol.framework/sokol", EntryPoint = "sgimgui_draw_shader_menu_item", CallingConvention = CallingConvention.Cdecl)]
#else
[DllImport("sokol", EntryPoint = "sgimgui_draw_shader_menu_item", CallingConvention = CallingConvention.Cdecl)]
#endif
public static extern void sgimgui_draw_shader_menu_item([M(U.LPUTF8Str)] string label);

#if __IOS__
[DllImport("@rpath/sokol.framework/sokol", EntryPoint = "sgimgui_draw_pipeline_menu_item", CallingConvention = CallingConvention.Cdecl)]
#else
[DllImport("sokol", EntryPoint = "sgimgui_draw_pipeline_menu_item", CallingConvention = CallingConvention.Cdecl)]
#endif
public static extern void sgimgui_draw_pipeline_menu_item([M(U.LPUTF8Str)] string label);

#if __IOS__
[DllImport("@rpath/sokol.framework/sokol", EntryPoint = "sgimgui_draw_view_menu_item", CallingConvention = CallingConvention.Cdecl)]
#else
[DllImport("sokol", EntryPoint = "sgimgui_draw_view_menu_item", CallingConvention = CallingConvention.Cdecl)]
#endif
public static extern void sgimgui_draw_view_menu_item([M(U.LPUTF8Str)] string label);

#if __IOS__
[DllImport("@rpath/sokol.framework/sokol", EntryPoint = "sgimgui_draw_capture_menu_item", CallingConvention = CallingConvention.Cdecl)]
#else
[DllImport("sokol", EntryPoint = "sgimgui_draw_capture_menu_item", CallingConvention = CallingConvention.Cdecl)]
#endif
public static extern void sgimgui_draw_capture_menu_item([M(U.LPUTF8Str)] string label);

#if __IOS__
[DllImport("@rpath/sokol.framework/sokol", EntryPoint = "sgimgui_draw_capabilities_menu_item", CallingConvention = CallingConvention.Cdecl)]
#else
[DllImport("sokol", EntryPoint = "sgimgui_draw_capabilities_menu_item", CallingConvention = CallingConvention.Cdecl)]
#endif
public static extern void sgimgui_draw_capabilities_menu_item([M(U.LPUTF8Str)] string label);

#if __IOS__
[DllImport("@rpath/sokol.framework/sokol", EntryPoint = "sgimgui_draw_frame_stats_menu_item", CallingConvention = CallingConvention.Cdecl)]
#else
[DllImport("sokol", EntryPoint = "sgimgui_draw_frame_stats_menu_item", CallingConvention = CallingConvention.Cdecl)]
#endif
public static extern void sgimgui_draw_frame_stats_menu_item([M(U.LPUTF8Str)] string label);

}
}
