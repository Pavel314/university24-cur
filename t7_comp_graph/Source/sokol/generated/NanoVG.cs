// machine generated, do not edit
using System;
using System.Runtime.InteropServices;
using M = System.Runtime.InteropServices.MarshalAsAttribute;
using U = System.Runtime.InteropServices.UnmanagedType;

using static Sokol.SG;

namespace Sokol
{
public static unsafe partial class NanoVG
{
[StructLayout(LayoutKind.Sequential)]
public struct NVGpaint
{
    #pragma warning disable 169
    public struct xformCollection
    {
        public ref float this[int index] => ref MemoryMarshal.CreateSpan(ref _item0, 6)[index];
        private float _item0;
        private float _item1;
        private float _item2;
        private float _item3;
        private float _item4;
        private float _item5;
    }
    #pragma warning restore 169
    public xformCollection xform;
    #pragma warning disable 169
    public struct extentCollection
    {
        public ref float this[int index] => ref MemoryMarshal.CreateSpan(ref _item0, 2)[index];
        private float _item0;
        private float _item1;
    }
    #pragma warning restore 169
    public extentCollection extent;
    public float radius;
    public float feather;
    public NVGcolor innerColor;
    public NVGcolor outerColor;
    public int image;
}
public enum NVGwinding
{
    NVG_CCW = 1,
    NVG_CW = 2,
}
public enum NVGsolidity
{
    NVG_SOLID = 1,
    NVG_HOLE = 2,
}
public enum NVGlineCap
{
    NVG_BUTT,
    NVG_ROUND,
    NVG_SQUARE,
    NVG_BEVEL,
    NVG_MITER,
}
public enum NVGalign
{
    NVG_ALIGN_LEFT = 1,
    NVG_ALIGN_CENTER = 2,
    NVG_ALIGN_RIGHT = 4,
    NVG_ALIGN_TOP = 8,
    NVG_ALIGN_MIDDLE = 16,
    NVG_ALIGN_BOTTOM = 32,
    NVG_ALIGN_BASELINE = 64,
}
public enum NVGblendFactor
{
    NVG_ZERO = 1,
    NVG_ONE = 2,
    NVG_SRC_COLOR = 4,
    NVG_ONE_MINUS_SRC_COLOR = 8,
    NVG_DST_COLOR = 16,
    NVG_ONE_MINUS_DST_COLOR = 32,
    NVG_SRC_ALPHA = 64,
    NVG_ONE_MINUS_SRC_ALPHA = 128,
    NVG_DST_ALPHA = 256,
    NVG_ONE_MINUS_DST_ALPHA = 512,
    NVG_SRC_ALPHA_SATURATE = 1024,
}
public enum NVGcompositeOperation
{
    NVG_SOURCE_OVER,
    NVG_SOURCE_IN,
    NVG_SOURCE_OUT,
    NVG_ATOP,
    NVG_DESTINATION_OVER,
    NVG_DESTINATION_IN,
    NVG_DESTINATION_OUT,
    NVG_DESTINATION_ATOP,
    NVG_LIGHTER,
    NVG_COPY,
    NVG_XOR,
}
[StructLayout(LayoutKind.Sequential)]
public struct NVGcompositeOperationState
{
    public int srcRGB;
    public int dstRGB;
    public int srcAlpha;
    public int dstAlpha;
}
[StructLayout(LayoutKind.Sequential)]
public struct NVGglyphPosition
{
#if WEB
    private IntPtr _str;
    public string str { get => Marshal.PtrToStringAnsi(_str);  set { if (_str != IntPtr.Zero) { Marshal.FreeHGlobal(_str); _str = IntPtr.Zero; } if (value != null) { _str = Marshal.StringToHGlobalAnsi(value); } } }
#else
    [M(U.LPUTF8Str)] public string str;
#endif
    public float x;
    public float minx;
    public float maxx;
}
[StructLayout(LayoutKind.Sequential)]
public struct NVGtextRow
{
#if WEB
    private IntPtr _start;
    public string start { get => Marshal.PtrToStringAnsi(_start);  set { if (_start != IntPtr.Zero) { Marshal.FreeHGlobal(_start); _start = IntPtr.Zero; } if (value != null) { _start = Marshal.StringToHGlobalAnsi(value); } } }
#else
    [M(U.LPUTF8Str)] public string start;
#endif
#if WEB
    private IntPtr _end;
    public string end { get => Marshal.PtrToStringAnsi(_end);  set { if (_end != IntPtr.Zero) { Marshal.FreeHGlobal(_end); _end = IntPtr.Zero; } if (value != null) { _end = Marshal.StringToHGlobalAnsi(value); } } }
#else
    [M(U.LPUTF8Str)] public string end;
#endif
#if WEB
    private IntPtr _next;
    public string next { get => Marshal.PtrToStringAnsi(_next);  set { if (_next != IntPtr.Zero) { Marshal.FreeHGlobal(_next); _next = IntPtr.Zero; } if (value != null) { _next = Marshal.StringToHGlobalAnsi(value); } } }
#else
    [M(U.LPUTF8Str)] public string next;
#endif
    public float width;
    public float minx;
    public float maxx;
}
public enum NVGimageFlags
{
    NVG_IMAGE_GENERATE_MIPMAPS = 1,
    NVG_IMAGE_REPEATX = 2,
    NVG_IMAGE_REPEATY = 4,
    NVG_IMAGE_FLIPY = 8,
    NVG_IMAGE_PREMULTIPLIED = 16,
    NVG_IMAGE_NEAREST = 32,
}
#if __IOS__
[DllImport("@rpath/sokol.framework/sokol", EntryPoint = "nvgBeginFrame", CallingConvention = CallingConvention.Cdecl)]
#else
[DllImport("sokol", EntryPoint = "nvgBeginFrame", CallingConvention = CallingConvention.Cdecl)]
#endif
public static extern void nvgBeginFrame(IntPtr ctx, float windowWidth, float windowHeight, float devicePixelRatio);

#if __IOS__
[DllImport("@rpath/sokol.framework/sokol", EntryPoint = "nvgCancelFrame", CallingConvention = CallingConvention.Cdecl)]
#else
[DllImport("sokol", EntryPoint = "nvgCancelFrame", CallingConvention = CallingConvention.Cdecl)]
#endif
public static extern void nvgCancelFrame(IntPtr ctx);

#if __IOS__
[DllImport("@rpath/sokol.framework/sokol", EntryPoint = "nvgEndFrame", CallingConvention = CallingConvention.Cdecl)]
#else
[DllImport("sokol", EntryPoint = "nvgEndFrame", CallingConvention = CallingConvention.Cdecl)]
#endif
public static extern void nvgEndFrame(IntPtr ctx);

#if __IOS__
[DllImport("@rpath/sokol.framework/sokol", EntryPoint = "nvgGlobalCompositeOperation", CallingConvention = CallingConvention.Cdecl)]
#else
[DllImport("sokol", EntryPoint = "nvgGlobalCompositeOperation", CallingConvention = CallingConvention.Cdecl)]
#endif
public static extern void nvgGlobalCompositeOperation(IntPtr ctx, int op);

#if __IOS__
[DllImport("@rpath/sokol.framework/sokol", EntryPoint = "nvgGlobalCompositeBlendFunc", CallingConvention = CallingConvention.Cdecl)]
#else
[DllImport("sokol", EntryPoint = "nvgGlobalCompositeBlendFunc", CallingConvention = CallingConvention.Cdecl)]
#endif
public static extern void nvgGlobalCompositeBlendFunc(IntPtr ctx, int sfactor, int dfactor);

#if __IOS__
[DllImport("@rpath/sokol.framework/sokol", EntryPoint = "nvgGlobalCompositeBlendFuncSeparate", CallingConvention = CallingConvention.Cdecl)]
#else
[DllImport("sokol", EntryPoint = "nvgGlobalCompositeBlendFuncSeparate", CallingConvention = CallingConvention.Cdecl)]
#endif
public static extern void nvgGlobalCompositeBlendFuncSeparate(IntPtr ctx, int srcRGB, int dstRGB, int srcAlpha, int dstAlpha);

#if WEB
public static NVGcolor nvgRGB(byte r, byte g, byte b)
{
    NVGcolor result = default;
    nvgRGB_internal(ref result, r, g, b);
    return result;
}
#else
#if __IOS__
[DllImport("@rpath/sokol.framework/sokol", EntryPoint = "nvgRGB", CallingConvention = CallingConvention.Cdecl)]
#else
[DllImport("sokol", EntryPoint = "nvgRGB", CallingConvention = CallingConvention.Cdecl)]
#endif
public static extern NVGcolor nvgRGB(byte r, byte g, byte b);
#endif

#if WEB
public static NVGcolor nvgRGBf(float r, float g, float b)
{
    NVGcolor result = default;
    nvgRGBf_internal(ref result, r, g, b);
    return result;
}
#else
#if __IOS__
[DllImport("@rpath/sokol.framework/sokol", EntryPoint = "nvgRGBf", CallingConvention = CallingConvention.Cdecl)]
#else
[DllImport("sokol", EntryPoint = "nvgRGBf", CallingConvention = CallingConvention.Cdecl)]
#endif
public static extern NVGcolor nvgRGBf(float r, float g, float b);
#endif

#if WEB
public static NVGcolor nvgRGBA(byte r, byte g, byte b, byte a)
{
    NVGcolor result = default;
    nvgRGBA_internal(ref result, r, g, b, a);
    return result;
}
#else
#if __IOS__
[DllImport("@rpath/sokol.framework/sokol", EntryPoint = "nvgRGBA", CallingConvention = CallingConvention.Cdecl)]
#else
[DllImport("sokol", EntryPoint = "nvgRGBA", CallingConvention = CallingConvention.Cdecl)]
#endif
public static extern NVGcolor nvgRGBA(byte r, byte g, byte b, byte a);
#endif

#if WEB
public static NVGcolor nvgRGBAf(float r, float g, float b, float a)
{
    NVGcolor result = default;
    nvgRGBAf_internal(ref result, r, g, b, a);
    return result;
}
#else
#if __IOS__
[DllImport("@rpath/sokol.framework/sokol", EntryPoint = "nvgRGBAf", CallingConvention = CallingConvention.Cdecl)]
#else
[DllImport("sokol", EntryPoint = "nvgRGBAf", CallingConvention = CallingConvention.Cdecl)]
#endif
public static extern NVGcolor nvgRGBAf(float r, float g, float b, float a);
#endif

#if WEB
public static NVGcolor nvgLerpRGBA(NVGcolor c0, NVGcolor c1, float u)
{
    NVGcolor result = default;
    nvgLerpRGBA_internal(ref result, c0, c1, u);
    return result;
}
#else
#if __IOS__
[DllImport("@rpath/sokol.framework/sokol", EntryPoint = "nvgLerpRGBA", CallingConvention = CallingConvention.Cdecl)]
#else
[DllImport("sokol", EntryPoint = "nvgLerpRGBA", CallingConvention = CallingConvention.Cdecl)]
#endif
public static extern NVGcolor nvgLerpRGBA(NVGcolor c0, NVGcolor c1, float u);
#endif

#if WEB
public static NVGcolor nvgTransRGBA(NVGcolor c0, byte a)
{
    NVGcolor result = default;
    nvgTransRGBA_internal(ref result, c0, a);
    return result;
}
#else
#if __IOS__
[DllImport("@rpath/sokol.framework/sokol", EntryPoint = "nvgTransRGBA", CallingConvention = CallingConvention.Cdecl)]
#else
[DllImport("sokol", EntryPoint = "nvgTransRGBA", CallingConvention = CallingConvention.Cdecl)]
#endif
public static extern NVGcolor nvgTransRGBA(NVGcolor c0, byte a);
#endif

#if WEB
public static NVGcolor nvgTransRGBAf(NVGcolor c0, float a)
{
    NVGcolor result = default;
    nvgTransRGBAf_internal(ref result, c0, a);
    return result;
}
#else
#if __IOS__
[DllImport("@rpath/sokol.framework/sokol", EntryPoint = "nvgTransRGBAf", CallingConvention = CallingConvention.Cdecl)]
#else
[DllImport("sokol", EntryPoint = "nvgTransRGBAf", CallingConvention = CallingConvention.Cdecl)]
#endif
public static extern NVGcolor nvgTransRGBAf(NVGcolor c0, float a);
#endif

#if WEB
public static NVGcolor nvgHSL(float h, float s, float l)
{
    NVGcolor result = default;
    nvgHSL_internal(ref result, h, s, l);
    return result;
}
#else
#if __IOS__
[DllImport("@rpath/sokol.framework/sokol", EntryPoint = "nvgHSL", CallingConvention = CallingConvention.Cdecl)]
#else
[DllImport("sokol", EntryPoint = "nvgHSL", CallingConvention = CallingConvention.Cdecl)]
#endif
public static extern NVGcolor nvgHSL(float h, float s, float l);
#endif

#if WEB
public static NVGcolor nvgHSLA(float h, float s, float l, byte a)
{
    NVGcolor result = default;
    nvgHSLA_internal(ref result, h, s, l, a);
    return result;
}
#else
#if __IOS__
[DllImport("@rpath/sokol.framework/sokol", EntryPoint = "nvgHSLA", CallingConvention = CallingConvention.Cdecl)]
#else
[DllImport("sokol", EntryPoint = "nvgHSLA", CallingConvention = CallingConvention.Cdecl)]
#endif
public static extern NVGcolor nvgHSLA(float h, float s, float l, byte a);
#endif

#if __IOS__
[DllImport("@rpath/sokol.framework/sokol", EntryPoint = "nvgSave", CallingConvention = CallingConvention.Cdecl)]
#else
[DllImport("sokol", EntryPoint = "nvgSave", CallingConvention = CallingConvention.Cdecl)]
#endif
public static extern void nvgSave(IntPtr ctx);

#if __IOS__
[DllImport("@rpath/sokol.framework/sokol", EntryPoint = "nvgRestore", CallingConvention = CallingConvention.Cdecl)]
#else
[DllImport("sokol", EntryPoint = "nvgRestore", CallingConvention = CallingConvention.Cdecl)]
#endif
public static extern void nvgRestore(IntPtr ctx);

#if __IOS__
[DllImport("@rpath/sokol.framework/sokol", EntryPoint = "nvgReset", CallingConvention = CallingConvention.Cdecl)]
#else
[DllImport("sokol", EntryPoint = "nvgReset", CallingConvention = CallingConvention.Cdecl)]
#endif
public static extern void nvgReset(IntPtr ctx);

#if __IOS__
[DllImport("@rpath/sokol.framework/sokol", EntryPoint = "nvgShapeAntiAlias", CallingConvention = CallingConvention.Cdecl)]
#else
[DllImport("sokol", EntryPoint = "nvgShapeAntiAlias", CallingConvention = CallingConvention.Cdecl)]
#endif
public static extern void nvgShapeAntiAlias(IntPtr ctx, int enabled);

#if __IOS__
[DllImport("@rpath/sokol.framework/sokol", EntryPoint = "nvgStrokeColor", CallingConvention = CallingConvention.Cdecl)]
#else
[DllImport("sokol", EntryPoint = "nvgStrokeColor", CallingConvention = CallingConvention.Cdecl)]
#endif
public static extern void nvgStrokeColor(IntPtr ctx, NVGcolor color);

#if __IOS__
[DllImport("@rpath/sokol.framework/sokol", EntryPoint = "nvgStrokePaint", CallingConvention = CallingConvention.Cdecl)]
#else
[DllImport("sokol", EntryPoint = "nvgStrokePaint", CallingConvention = CallingConvention.Cdecl)]
#endif
public static extern void nvgStrokePaint(IntPtr ctx, NVGpaint paint);

#if __IOS__
[DllImport("@rpath/sokol.framework/sokol", EntryPoint = "nvgFillColor", CallingConvention = CallingConvention.Cdecl)]
#else
[DllImport("sokol", EntryPoint = "nvgFillColor", CallingConvention = CallingConvention.Cdecl)]
#endif
public static extern void nvgFillColor(IntPtr ctx, NVGcolor color);

#if __IOS__
[DllImport("@rpath/sokol.framework/sokol", EntryPoint = "nvgFillPaint", CallingConvention = CallingConvention.Cdecl)]
#else
[DllImport("sokol", EntryPoint = "nvgFillPaint", CallingConvention = CallingConvention.Cdecl)]
#endif
public static extern void nvgFillPaint(IntPtr ctx, NVGpaint paint);

#if __IOS__
[DllImport("@rpath/sokol.framework/sokol", EntryPoint = "nvgMiterLimit", CallingConvention = CallingConvention.Cdecl)]
#else
[DllImport("sokol", EntryPoint = "nvgMiterLimit", CallingConvention = CallingConvention.Cdecl)]
#endif
public static extern void nvgMiterLimit(IntPtr ctx, float limit);

#if __IOS__
[DllImport("@rpath/sokol.framework/sokol", EntryPoint = "nvgStrokeWidth", CallingConvention = CallingConvention.Cdecl)]
#else
[DllImport("sokol", EntryPoint = "nvgStrokeWidth", CallingConvention = CallingConvention.Cdecl)]
#endif
public static extern void nvgStrokeWidth(IntPtr ctx, float size);

#if __IOS__
[DllImport("@rpath/sokol.framework/sokol", EntryPoint = "nvgLineCap", CallingConvention = CallingConvention.Cdecl)]
#else
[DllImport("sokol", EntryPoint = "nvgLineCap", CallingConvention = CallingConvention.Cdecl)]
#endif
public static extern void nvgLineCap(IntPtr ctx, int cap);

#if __IOS__
[DllImport("@rpath/sokol.framework/sokol", EntryPoint = "nvgLineJoin", CallingConvention = CallingConvention.Cdecl)]
#else
[DllImport("sokol", EntryPoint = "nvgLineJoin", CallingConvention = CallingConvention.Cdecl)]
#endif
public static extern void nvgLineJoin(IntPtr ctx, int join);

#if __IOS__
[DllImport("@rpath/sokol.framework/sokol", EntryPoint = "nvgGlobalAlpha", CallingConvention = CallingConvention.Cdecl)]
#else
[DllImport("sokol", EntryPoint = "nvgGlobalAlpha", CallingConvention = CallingConvention.Cdecl)]
#endif
public static extern void nvgGlobalAlpha(IntPtr ctx, float alpha);

#if __IOS__
[DllImport("@rpath/sokol.framework/sokol", EntryPoint = "nvgResetTransform", CallingConvention = CallingConvention.Cdecl)]
#else
[DllImport("sokol", EntryPoint = "nvgResetTransform", CallingConvention = CallingConvention.Cdecl)]
#endif
public static extern void nvgResetTransform(IntPtr ctx);

#if __IOS__
[DllImport("@rpath/sokol.framework/sokol", EntryPoint = "nvgTransform", CallingConvention = CallingConvention.Cdecl)]
#else
[DllImport("sokol", EntryPoint = "nvgTransform", CallingConvention = CallingConvention.Cdecl)]
#endif
public static extern void nvgTransform(IntPtr ctx, float a, float b, float c, float d, float e, float f);

#if __IOS__
[DllImport("@rpath/sokol.framework/sokol", EntryPoint = "nvgTranslate", CallingConvention = CallingConvention.Cdecl)]
#else
[DllImport("sokol", EntryPoint = "nvgTranslate", CallingConvention = CallingConvention.Cdecl)]
#endif
public static extern void nvgTranslate(IntPtr ctx, float x, float y);

#if __IOS__
[DllImport("@rpath/sokol.framework/sokol", EntryPoint = "nvgRotate", CallingConvention = CallingConvention.Cdecl)]
#else
[DllImport("sokol", EntryPoint = "nvgRotate", CallingConvention = CallingConvention.Cdecl)]
#endif
public static extern void nvgRotate(IntPtr ctx, float angle);

#if __IOS__
[DllImport("@rpath/sokol.framework/sokol", EntryPoint = "nvgSkewX", CallingConvention = CallingConvention.Cdecl)]
#else
[DllImport("sokol", EntryPoint = "nvgSkewX", CallingConvention = CallingConvention.Cdecl)]
#endif
public static extern void nvgSkewX(IntPtr ctx, float angle);

#if __IOS__
[DllImport("@rpath/sokol.framework/sokol", EntryPoint = "nvgSkewY", CallingConvention = CallingConvention.Cdecl)]
#else
[DllImport("sokol", EntryPoint = "nvgSkewY", CallingConvention = CallingConvention.Cdecl)]
#endif
public static extern void nvgSkewY(IntPtr ctx, float angle);

#if __IOS__
[DllImport("@rpath/sokol.framework/sokol", EntryPoint = "nvgScale", CallingConvention = CallingConvention.Cdecl)]
#else
[DllImport("sokol", EntryPoint = "nvgScale", CallingConvention = CallingConvention.Cdecl)]
#endif
public static extern void nvgScale(IntPtr ctx, float x, float y);

#if __IOS__
[DllImport("@rpath/sokol.framework/sokol", EntryPoint = "nvgCurrentTransform", CallingConvention = CallingConvention.Cdecl)]
#else
[DllImport("sokol", EntryPoint = "nvgCurrentTransform", CallingConvention = CallingConvention.Cdecl)]
#endif
public static extern void nvgCurrentTransform(IntPtr ctx, ref float xform);

#if __IOS__
[DllImport("@rpath/sokol.framework/sokol", EntryPoint = "nvgTransformIdentity", CallingConvention = CallingConvention.Cdecl)]
#else
[DllImport("sokol", EntryPoint = "nvgTransformIdentity", CallingConvention = CallingConvention.Cdecl)]
#endif
public static extern void nvgTransformIdentity(ref float dst);

#if __IOS__
[DllImport("@rpath/sokol.framework/sokol", EntryPoint = "nvgTransformTranslate", CallingConvention = CallingConvention.Cdecl)]
#else
[DllImport("sokol", EntryPoint = "nvgTransformTranslate", CallingConvention = CallingConvention.Cdecl)]
#endif
public static extern void nvgTransformTranslate(ref float dst, float tx, float ty);

#if __IOS__
[DllImport("@rpath/sokol.framework/sokol", EntryPoint = "nvgTransformScale", CallingConvention = CallingConvention.Cdecl)]
#else
[DllImport("sokol", EntryPoint = "nvgTransformScale", CallingConvention = CallingConvention.Cdecl)]
#endif
public static extern void nvgTransformScale(ref float dst, float sx, float sy);

#if __IOS__
[DllImport("@rpath/sokol.framework/sokol", EntryPoint = "nvgTransformRotate", CallingConvention = CallingConvention.Cdecl)]
#else
[DllImport("sokol", EntryPoint = "nvgTransformRotate", CallingConvention = CallingConvention.Cdecl)]
#endif
public static extern void nvgTransformRotate(ref float dst, float a);

#if __IOS__
[DllImport("@rpath/sokol.framework/sokol", EntryPoint = "nvgTransformSkewX", CallingConvention = CallingConvention.Cdecl)]
#else
[DllImport("sokol", EntryPoint = "nvgTransformSkewX", CallingConvention = CallingConvention.Cdecl)]
#endif
public static extern void nvgTransformSkewX(ref float dst, float a);

#if __IOS__
[DllImport("@rpath/sokol.framework/sokol", EntryPoint = "nvgTransformSkewY", CallingConvention = CallingConvention.Cdecl)]
#else
[DllImport("sokol", EntryPoint = "nvgTransformSkewY", CallingConvention = CallingConvention.Cdecl)]
#endif
public static extern void nvgTransformSkewY(ref float dst, float a);

#if __IOS__
[DllImport("@rpath/sokol.framework/sokol", EntryPoint = "nvgTransformMultiply", CallingConvention = CallingConvention.Cdecl)]
#else
[DllImport("sokol", EntryPoint = "nvgTransformMultiply", CallingConvention = CallingConvention.Cdecl)]
#endif
public static extern void nvgTransformMultiply(ref float dst, in float src);

#if __IOS__
[DllImport("@rpath/sokol.framework/sokol", EntryPoint = "nvgTransformPremultiply", CallingConvention = CallingConvention.Cdecl)]
#else
[DllImport("sokol", EntryPoint = "nvgTransformPremultiply", CallingConvention = CallingConvention.Cdecl)]
#endif
public static extern void nvgTransformPremultiply(ref float dst, in float src);

#if __IOS__
[DllImport("@rpath/sokol.framework/sokol", EntryPoint = "nvgTransformInverse", CallingConvention = CallingConvention.Cdecl)]
#else
[DllImport("sokol", EntryPoint = "nvgTransformInverse", CallingConvention = CallingConvention.Cdecl)]
#endif
public static extern int nvgTransformInverse(ref float dst, in float src);

#if __IOS__
[DllImport("@rpath/sokol.framework/sokol", EntryPoint = "nvgTransformPoint", CallingConvention = CallingConvention.Cdecl)]
#else
[DllImport("sokol", EntryPoint = "nvgTransformPoint", CallingConvention = CallingConvention.Cdecl)]
#endif
public static extern void nvgTransformPoint(ref float dstx, ref float dsty, in float xform, float srcx, float srcy);

#if __IOS__
[DllImport("@rpath/sokol.framework/sokol", EntryPoint = "nvgDegToRad", CallingConvention = CallingConvention.Cdecl)]
#else
[DllImport("sokol", EntryPoint = "nvgDegToRad", CallingConvention = CallingConvention.Cdecl)]
#endif
public static extern float nvgDegToRad(float deg);

#if __IOS__
[DllImport("@rpath/sokol.framework/sokol", EntryPoint = "nvgRadToDeg", CallingConvention = CallingConvention.Cdecl)]
#else
[DllImport("sokol", EntryPoint = "nvgRadToDeg", CallingConvention = CallingConvention.Cdecl)]
#endif
public static extern float nvgRadToDeg(float rad);

#if __IOS__
[DllImport("@rpath/sokol.framework/sokol", EntryPoint = "nvgCreateImage", CallingConvention = CallingConvention.Cdecl)]
#else
[DllImport("sokol", EntryPoint = "nvgCreateImage", CallingConvention = CallingConvention.Cdecl)]
#endif
public static extern int nvgCreateImage(IntPtr ctx, [M(U.LPUTF8Str)] string filename, int imageFlags);

#if __IOS__
[DllImport("@rpath/sokol.framework/sokol", EntryPoint = "nvgCreateImageMem", CallingConvention = CallingConvention.Cdecl)]
#else
[DllImport("sokol", EntryPoint = "nvgCreateImageMem", CallingConvention = CallingConvention.Cdecl)]
#endif
public static extern int nvgCreateImageMem(IntPtr ctx, int imageFlags, byte* data, int ndata);

#if __IOS__
[DllImport("@rpath/sokol.framework/sokol", EntryPoint = "nvgCreateImageRGBA", CallingConvention = CallingConvention.Cdecl)]
#else
[DllImport("sokol", EntryPoint = "nvgCreateImageRGBA", CallingConvention = CallingConvention.Cdecl)]
#endif
public static extern int nvgCreateImageRGBA(IntPtr ctx, int w, int h, int imageFlags, in byte data);

#if __IOS__
[DllImport("@rpath/sokol.framework/sokol", EntryPoint = "nvgUpdateImage", CallingConvention = CallingConvention.Cdecl)]
#else
[DllImport("sokol", EntryPoint = "nvgUpdateImage", CallingConvention = CallingConvention.Cdecl)]
#endif
public static extern void nvgUpdateImage(IntPtr ctx, int image, in byte data);

#if __IOS__
[DllImport("@rpath/sokol.framework/sokol", EntryPoint = "nvgImageSize", CallingConvention = CallingConvention.Cdecl)]
#else
[DllImport("sokol", EntryPoint = "nvgImageSize", CallingConvention = CallingConvention.Cdecl)]
#endif
public static extern void nvgImageSize(IntPtr ctx, int image, ref int w, ref int h);

#if __IOS__
[DllImport("@rpath/sokol.framework/sokol", EntryPoint = "nvgDeleteImage", CallingConvention = CallingConvention.Cdecl)]
#else
[DllImport("sokol", EntryPoint = "nvgDeleteImage", CallingConvention = CallingConvention.Cdecl)]
#endif
public static extern void nvgDeleteImage(IntPtr ctx, int image);

#if WEB
public static NVGpaint nvgLinearGradient(IntPtr ctx, float sx, float sy, float ex, float ey, NVGcolor icol, NVGcolor ocol)
{
    NVGpaint result = default;
    nvgLinearGradient_internal(ref result, ctx, sx, sy, ex, ey, icol, ocol);
    return result;
}
#else
#if __IOS__
[DllImport("@rpath/sokol.framework/sokol", EntryPoint = "nvgLinearGradient", CallingConvention = CallingConvention.Cdecl)]
#else
[DllImport("sokol", EntryPoint = "nvgLinearGradient", CallingConvention = CallingConvention.Cdecl)]
#endif
public static extern NVGpaint nvgLinearGradient(IntPtr ctx, float sx, float sy, float ex, float ey, NVGcolor icol, NVGcolor ocol);
#endif

#if WEB
public static NVGpaint nvgBoxGradient(IntPtr ctx, float x, float y, float w, float h, float r, float f, NVGcolor icol, NVGcolor ocol)
{
    NVGpaint result = default;
    nvgBoxGradient_internal(ref result, ctx, x, y, w, h, r, f, icol, ocol);
    return result;
}
#else
#if __IOS__
[DllImport("@rpath/sokol.framework/sokol", EntryPoint = "nvgBoxGradient", CallingConvention = CallingConvention.Cdecl)]
#else
[DllImport("sokol", EntryPoint = "nvgBoxGradient", CallingConvention = CallingConvention.Cdecl)]
#endif
public static extern NVGpaint nvgBoxGradient(IntPtr ctx, float x, float y, float w, float h, float r, float f, NVGcolor icol, NVGcolor ocol);
#endif

#if WEB
public static NVGpaint nvgRadialGradient(IntPtr ctx, float cx, float cy, float inr, float outr, NVGcolor icol, NVGcolor ocol)
{
    NVGpaint result = default;
    nvgRadialGradient_internal(ref result, ctx, cx, cy, inr, outr, icol, ocol);
    return result;
}
#else
#if __IOS__
[DllImport("@rpath/sokol.framework/sokol", EntryPoint = "nvgRadialGradient", CallingConvention = CallingConvention.Cdecl)]
#else
[DllImport("sokol", EntryPoint = "nvgRadialGradient", CallingConvention = CallingConvention.Cdecl)]
#endif
public static extern NVGpaint nvgRadialGradient(IntPtr ctx, float cx, float cy, float inr, float outr, NVGcolor icol, NVGcolor ocol);
#endif

#if WEB
public static NVGpaint nvgImagePattern(IntPtr ctx, float ox, float oy, float ex, float ey, float angle, int image, float alpha)
{
    NVGpaint result = default;
    nvgImagePattern_internal(ref result, ctx, ox, oy, ex, ey, angle, image, alpha);
    return result;
}
#else
#if __IOS__
[DllImport("@rpath/sokol.framework/sokol", EntryPoint = "nvgImagePattern", CallingConvention = CallingConvention.Cdecl)]
#else
[DllImport("sokol", EntryPoint = "nvgImagePattern", CallingConvention = CallingConvention.Cdecl)]
#endif
public static extern NVGpaint nvgImagePattern(IntPtr ctx, float ox, float oy, float ex, float ey, float angle, int image, float alpha);
#endif

#if __IOS__
[DllImport("@rpath/sokol.framework/sokol", EntryPoint = "nvgScissor", CallingConvention = CallingConvention.Cdecl)]
#else
[DllImport("sokol", EntryPoint = "nvgScissor", CallingConvention = CallingConvention.Cdecl)]
#endif
public static extern void nvgScissor(IntPtr ctx, float x, float y, float w, float h);

#if __IOS__
[DllImport("@rpath/sokol.framework/sokol", EntryPoint = "nvgIntersectScissor", CallingConvention = CallingConvention.Cdecl)]
#else
[DllImport("sokol", EntryPoint = "nvgIntersectScissor", CallingConvention = CallingConvention.Cdecl)]
#endif
public static extern void nvgIntersectScissor(IntPtr ctx, float x, float y, float w, float h);

#if __IOS__
[DllImport("@rpath/sokol.framework/sokol", EntryPoint = "nvgResetScissor", CallingConvention = CallingConvention.Cdecl)]
#else
[DllImport("sokol", EntryPoint = "nvgResetScissor", CallingConvention = CallingConvention.Cdecl)]
#endif
public static extern void nvgResetScissor(IntPtr ctx);

#if __IOS__
[DllImport("@rpath/sokol.framework/sokol", EntryPoint = "nvgBeginPath", CallingConvention = CallingConvention.Cdecl)]
#else
[DllImport("sokol", EntryPoint = "nvgBeginPath", CallingConvention = CallingConvention.Cdecl)]
#endif
public static extern void nvgBeginPath(IntPtr ctx);

#if __IOS__
[DllImport("@rpath/sokol.framework/sokol", EntryPoint = "nvgMoveTo", CallingConvention = CallingConvention.Cdecl)]
#else
[DllImport("sokol", EntryPoint = "nvgMoveTo", CallingConvention = CallingConvention.Cdecl)]
#endif
public static extern void nvgMoveTo(IntPtr ctx, float x, float y);

#if __IOS__
[DllImport("@rpath/sokol.framework/sokol", EntryPoint = "nvgLineTo", CallingConvention = CallingConvention.Cdecl)]
#else
[DllImport("sokol", EntryPoint = "nvgLineTo", CallingConvention = CallingConvention.Cdecl)]
#endif
public static extern void nvgLineTo(IntPtr ctx, float x, float y);

#if __IOS__
[DllImport("@rpath/sokol.framework/sokol", EntryPoint = "nvgBezierTo", CallingConvention = CallingConvention.Cdecl)]
#else
[DllImport("sokol", EntryPoint = "nvgBezierTo", CallingConvention = CallingConvention.Cdecl)]
#endif
public static extern void nvgBezierTo(IntPtr ctx, float c1x, float c1y, float c2x, float c2y, float x, float y);

#if __IOS__
[DllImport("@rpath/sokol.framework/sokol", EntryPoint = "nvgQuadTo", CallingConvention = CallingConvention.Cdecl)]
#else
[DllImport("sokol", EntryPoint = "nvgQuadTo", CallingConvention = CallingConvention.Cdecl)]
#endif
public static extern void nvgQuadTo(IntPtr ctx, float cx, float cy, float x, float y);

#if __IOS__
[DllImport("@rpath/sokol.framework/sokol", EntryPoint = "nvgArcTo", CallingConvention = CallingConvention.Cdecl)]
#else
[DllImport("sokol", EntryPoint = "nvgArcTo", CallingConvention = CallingConvention.Cdecl)]
#endif
public static extern void nvgArcTo(IntPtr ctx, float x1, float y1, float x2, float y2, float radius);

#if __IOS__
[DllImport("@rpath/sokol.framework/sokol", EntryPoint = "nvgClosePath", CallingConvention = CallingConvention.Cdecl)]
#else
[DllImport("sokol", EntryPoint = "nvgClosePath", CallingConvention = CallingConvention.Cdecl)]
#endif
public static extern void nvgClosePath(IntPtr ctx);

#if __IOS__
[DllImport("@rpath/sokol.framework/sokol", EntryPoint = "nvgPathWinding", CallingConvention = CallingConvention.Cdecl)]
#else
[DllImport("sokol", EntryPoint = "nvgPathWinding", CallingConvention = CallingConvention.Cdecl)]
#endif
public static extern void nvgPathWinding(IntPtr ctx, int dir);

#if __IOS__
[DllImport("@rpath/sokol.framework/sokol", EntryPoint = "nvgArc", CallingConvention = CallingConvention.Cdecl)]
#else
[DllImport("sokol", EntryPoint = "nvgArc", CallingConvention = CallingConvention.Cdecl)]
#endif
public static extern void nvgArc(IntPtr ctx, float cx, float cy, float r, float a0, float a1, int dir);

#if __IOS__
[DllImport("@rpath/sokol.framework/sokol", EntryPoint = "nvgRect", CallingConvention = CallingConvention.Cdecl)]
#else
[DllImport("sokol", EntryPoint = "nvgRect", CallingConvention = CallingConvention.Cdecl)]
#endif
public static extern void nvgRect(IntPtr ctx, float x, float y, float w, float h);

#if __IOS__
[DllImport("@rpath/sokol.framework/sokol", EntryPoint = "nvgRoundedRect", CallingConvention = CallingConvention.Cdecl)]
#else
[DllImport("sokol", EntryPoint = "nvgRoundedRect", CallingConvention = CallingConvention.Cdecl)]
#endif
public static extern void nvgRoundedRect(IntPtr ctx, float x, float y, float w, float h, float r);

#if __IOS__
[DllImport("@rpath/sokol.framework/sokol", EntryPoint = "nvgRoundedRectVarying", CallingConvention = CallingConvention.Cdecl)]
#else
[DllImport("sokol", EntryPoint = "nvgRoundedRectVarying", CallingConvention = CallingConvention.Cdecl)]
#endif
public static extern void nvgRoundedRectVarying(IntPtr ctx, float x, float y, float w, float h, float radTopLeft, float radTopRight, float radBottomRight, float radBottomLeft);

#if __IOS__
[DllImport("@rpath/sokol.framework/sokol", EntryPoint = "nvgEllipse", CallingConvention = CallingConvention.Cdecl)]
#else
[DllImport("sokol", EntryPoint = "nvgEllipse", CallingConvention = CallingConvention.Cdecl)]
#endif
public static extern void nvgEllipse(IntPtr ctx, float cx, float cy, float rx, float ry);

#if __IOS__
[DllImport("@rpath/sokol.framework/sokol", EntryPoint = "nvgCircle", CallingConvention = CallingConvention.Cdecl)]
#else
[DllImport("sokol", EntryPoint = "nvgCircle", CallingConvention = CallingConvention.Cdecl)]
#endif
public static extern void nvgCircle(IntPtr ctx, float cx, float cy, float r);

#if __IOS__
[DllImport("@rpath/sokol.framework/sokol", EntryPoint = "nvgFill", CallingConvention = CallingConvention.Cdecl)]
#else
[DllImport("sokol", EntryPoint = "nvgFill", CallingConvention = CallingConvention.Cdecl)]
#endif
public static extern void nvgFill(IntPtr ctx);

#if __IOS__
[DllImport("@rpath/sokol.framework/sokol", EntryPoint = "nvgStroke", CallingConvention = CallingConvention.Cdecl)]
#else
[DllImport("sokol", EntryPoint = "nvgStroke", CallingConvention = CallingConvention.Cdecl)]
#endif
public static extern void nvgStroke(IntPtr ctx);

#if __IOS__
[DllImport("@rpath/sokol.framework/sokol", EntryPoint = "nvgCreateFont", CallingConvention = CallingConvention.Cdecl)]
#else
[DllImport("sokol", EntryPoint = "nvgCreateFont", CallingConvention = CallingConvention.Cdecl)]
#endif
public static extern int nvgCreateFont(IntPtr ctx, [M(U.LPUTF8Str)] string name, [M(U.LPUTF8Str)] string filename);

#if __IOS__
[DllImport("@rpath/sokol.framework/sokol", EntryPoint = "nvgCreateFontAtIndex", CallingConvention = CallingConvention.Cdecl)]
#else
[DllImport("sokol", EntryPoint = "nvgCreateFontAtIndex", CallingConvention = CallingConvention.Cdecl)]
#endif
public static extern int nvgCreateFontAtIndex(IntPtr ctx, [M(U.LPUTF8Str)] string name, [M(U.LPUTF8Str)] string filename, int fontIndex);

#if __IOS__
[DllImport("@rpath/sokol.framework/sokol", EntryPoint = "nvgCreateFontMem", CallingConvention = CallingConvention.Cdecl)]
#else
[DllImport("sokol", EntryPoint = "nvgCreateFontMem", CallingConvention = CallingConvention.Cdecl)]
#endif
public static extern int nvgCreateFontMem(IntPtr ctx, [M(U.LPUTF8Str)] string name, byte* data, int ndata, int freeData);

#if __IOS__
[DllImport("@rpath/sokol.framework/sokol", EntryPoint = "nvgCreateFontMemAtIndex", CallingConvention = CallingConvention.Cdecl)]
#else
[DllImport("sokol", EntryPoint = "nvgCreateFontMemAtIndex", CallingConvention = CallingConvention.Cdecl)]
#endif
public static extern int nvgCreateFontMemAtIndex(IntPtr ctx, [M(U.LPUTF8Str)] string name, byte* data, int ndata, int freeData, int fontIndex);

#if __IOS__
[DllImport("@rpath/sokol.framework/sokol", EntryPoint = "nvgFindFont", CallingConvention = CallingConvention.Cdecl)]
#else
[DllImport("sokol", EntryPoint = "nvgFindFont", CallingConvention = CallingConvention.Cdecl)]
#endif
public static extern int nvgFindFont(IntPtr ctx, [M(U.LPUTF8Str)] string name);

#if __IOS__
[DllImport("@rpath/sokol.framework/sokol", EntryPoint = "nvgAddFallbackFontId", CallingConvention = CallingConvention.Cdecl)]
#else
[DllImport("sokol", EntryPoint = "nvgAddFallbackFontId", CallingConvention = CallingConvention.Cdecl)]
#endif
public static extern int nvgAddFallbackFontId(IntPtr ctx, int baseFont, int fallbackFont);

#if __IOS__
[DllImport("@rpath/sokol.framework/sokol", EntryPoint = "nvgAddFallbackFont", CallingConvention = CallingConvention.Cdecl)]
#else
[DllImport("sokol", EntryPoint = "nvgAddFallbackFont", CallingConvention = CallingConvention.Cdecl)]
#endif
public static extern int nvgAddFallbackFont(IntPtr ctx, [M(U.LPUTF8Str)] string baseFont, [M(U.LPUTF8Str)] string fallbackFont);

#if __IOS__
[DllImport("@rpath/sokol.framework/sokol", EntryPoint = "nvgResetFallbackFontsId", CallingConvention = CallingConvention.Cdecl)]
#else
[DllImport("sokol", EntryPoint = "nvgResetFallbackFontsId", CallingConvention = CallingConvention.Cdecl)]
#endif
public static extern void nvgResetFallbackFontsId(IntPtr ctx, int baseFont);

#if __IOS__
[DllImport("@rpath/sokol.framework/sokol", EntryPoint = "nvgResetFallbackFonts", CallingConvention = CallingConvention.Cdecl)]
#else
[DllImport("sokol", EntryPoint = "nvgResetFallbackFonts", CallingConvention = CallingConvention.Cdecl)]
#endif
public static extern void nvgResetFallbackFonts(IntPtr ctx, [M(U.LPUTF8Str)] string baseFont);

#if __IOS__
[DllImport("@rpath/sokol.framework/sokol", EntryPoint = "nvgFontSize", CallingConvention = CallingConvention.Cdecl)]
#else
[DllImport("sokol", EntryPoint = "nvgFontSize", CallingConvention = CallingConvention.Cdecl)]
#endif
public static extern void nvgFontSize(IntPtr ctx, float size);

#if __IOS__
[DllImport("@rpath/sokol.framework/sokol", EntryPoint = "nvgFontBlur", CallingConvention = CallingConvention.Cdecl)]
#else
[DllImport("sokol", EntryPoint = "nvgFontBlur", CallingConvention = CallingConvention.Cdecl)]
#endif
public static extern void nvgFontBlur(IntPtr ctx, float blur);

#if __IOS__
[DllImport("@rpath/sokol.framework/sokol", EntryPoint = "nvgTextLetterSpacing", CallingConvention = CallingConvention.Cdecl)]
#else
[DllImport("sokol", EntryPoint = "nvgTextLetterSpacing", CallingConvention = CallingConvention.Cdecl)]
#endif
public static extern void nvgTextLetterSpacing(IntPtr ctx, float spacing);

#if __IOS__
[DllImport("@rpath/sokol.framework/sokol", EntryPoint = "nvgTextLineHeight", CallingConvention = CallingConvention.Cdecl)]
#else
[DllImport("sokol", EntryPoint = "nvgTextLineHeight", CallingConvention = CallingConvention.Cdecl)]
#endif
public static extern void nvgTextLineHeight(IntPtr ctx, float lineHeight);

#if __IOS__
[DllImport("@rpath/sokol.framework/sokol", EntryPoint = "nvgTextAlign", CallingConvention = CallingConvention.Cdecl)]
#else
[DllImport("sokol", EntryPoint = "nvgTextAlign", CallingConvention = CallingConvention.Cdecl)]
#endif
public static extern void nvgTextAlign(IntPtr ctx, int align);

#if __IOS__
[DllImport("@rpath/sokol.framework/sokol", EntryPoint = "nvgFontFaceId", CallingConvention = CallingConvention.Cdecl)]
#else
[DllImport("sokol", EntryPoint = "nvgFontFaceId", CallingConvention = CallingConvention.Cdecl)]
#endif
public static extern void nvgFontFaceId(IntPtr ctx, int font);

#if __IOS__
[DllImport("@rpath/sokol.framework/sokol", EntryPoint = "nvgFontFace", CallingConvention = CallingConvention.Cdecl)]
#else
[DllImport("sokol", EntryPoint = "nvgFontFace", CallingConvention = CallingConvention.Cdecl)]
#endif
public static extern void nvgFontFace(IntPtr ctx, [M(U.LPUTF8Str)] string font);

#if __IOS__
[DllImport("@rpath/sokol.framework/sokol", EntryPoint = "nvgText", CallingConvention = CallingConvention.Cdecl)]
#else
[DllImport("sokol", EntryPoint = "nvgText", CallingConvention = CallingConvention.Cdecl)]
#endif
public static extern float nvgText(IntPtr ctx, float x, float y, [M(U.LPUTF8Str)] string _string, [M(U.LPUTF8Str)] string end);

#if __IOS__
[DllImport("@rpath/sokol.framework/sokol", EntryPoint = "nvgTextBox", CallingConvention = CallingConvention.Cdecl)]
#else
[DllImport("sokol", EntryPoint = "nvgTextBox", CallingConvention = CallingConvention.Cdecl)]
#endif
public static extern void nvgTextBox(IntPtr ctx, float x, float y, float breakRowWidth, [M(U.LPUTF8Str)] string _string, [M(U.LPUTF8Str)] string end);

#if __IOS__
[DllImport("@rpath/sokol.framework/sokol", EntryPoint = "nvgTextBounds", CallingConvention = CallingConvention.Cdecl)]
#else
[DllImport("sokol", EntryPoint = "nvgTextBounds", CallingConvention = CallingConvention.Cdecl)]
#endif
public static extern float nvgTextBounds(IntPtr ctx, float x, float y, [M(U.LPUTF8Str)] string _string, [M(U.LPUTF8Str)] string end, ref float bounds);

#if __IOS__
[DllImport("@rpath/sokol.framework/sokol", EntryPoint = "nvgTextBoxBounds", CallingConvention = CallingConvention.Cdecl)]
#else
[DllImport("sokol", EntryPoint = "nvgTextBoxBounds", CallingConvention = CallingConvention.Cdecl)]
#endif
public static extern void nvgTextBoxBounds(IntPtr ctx, float x, float y, float breakRowWidth, [M(U.LPUTF8Str)] string _string, [M(U.LPUTF8Str)] string end, ref float bounds);

#if __IOS__
[DllImport("@rpath/sokol.framework/sokol", EntryPoint = "nvgTextGlyphPositions", CallingConvention = CallingConvention.Cdecl)]
#else
[DllImport("sokol", EntryPoint = "nvgTextGlyphPositions", CallingConvention = CallingConvention.Cdecl)]
#endif
public static extern int nvgTextGlyphPositions(IntPtr ctx, float x, float y, [M(U.LPUTF8Str)] string _string, [M(U.LPUTF8Str)] string end, NVGglyphPosition* positions, int maxPositions);

#if __IOS__
[DllImport("@rpath/sokol.framework/sokol", EntryPoint = "nvgTextMetrics", CallingConvention = CallingConvention.Cdecl)]
#else
[DllImport("sokol", EntryPoint = "nvgTextMetrics", CallingConvention = CallingConvention.Cdecl)]
#endif
public static extern void nvgTextMetrics(IntPtr ctx, ref float ascender, ref float descender, ref float lineh);

#if __IOS__
[DllImport("@rpath/sokol.framework/sokol", EntryPoint = "nvgTextBreakLines", CallingConvention = CallingConvention.Cdecl)]
#else
[DllImport("sokol", EntryPoint = "nvgTextBreakLines", CallingConvention = CallingConvention.Cdecl)]
#endif
public static extern int nvgTextBreakLines(IntPtr ctx, [M(U.LPUTF8Str)] string _string, [M(U.LPUTF8Str)] string end, float breakRowWidth, NVGtextRow* rows, int maxRows);

public enum NVGtexture
{
    NVG_TEXTURE_ALPHA = 1,
    NVG_TEXTURE_RGBA = 2,
}
#if __IOS__
[DllImport("@rpath/sokol.framework/sokol", EntryPoint = "nvgDebugDumpPathCache", CallingConvention = CallingConvention.Cdecl)]
#else
[DllImport("sokol", EntryPoint = "nvgDebugDumpPathCache", CallingConvention = CallingConvention.Cdecl)]
#endif
public static extern void nvgDebugDumpPathCache(IntPtr ctx);

public const int NVG_ANTIALIAS = 1;
public const int NVG_STENCIL_STROKES = 2;
public const int NVG_DEBUG = 4;
[StructLayout(LayoutKind.Sequential)]
public struct snvg_allocator_t
{
    public delegate* unmanaged<nuint, void*, void*> alloc_fn;
    public delegate* unmanaged<void*, void*, void> free_fn;
    public void* user_data;
}
[StructLayout(LayoutKind.Sequential)]
public struct snvg_desc_t
{
    public int max_vertices;
    public snvg_allocator_t allocator;
}
#if __IOS__
[DllImport("@rpath/sokol.framework/sokol", EntryPoint = "nvgCreateSokol", CallingConvention = CallingConvention.Cdecl)]
#else
[DllImport("sokol", EntryPoint = "nvgCreateSokol", CallingConvention = CallingConvention.Cdecl)]
#endif
public static extern IntPtr nvgCreateSokol(int flags);

#if __IOS__
[DllImport("@rpath/sokol.framework/sokol", EntryPoint = "nvgCreateSokolWithDesc", CallingConvention = CallingConvention.Cdecl)]
#else
[DllImport("sokol", EntryPoint = "nvgCreateSokolWithDesc", CallingConvention = CallingConvention.Cdecl)]
#endif
public static extern IntPtr nvgCreateSokolWithDesc(int flags, in snvg_desc_t desc);

#if __IOS__
[DllImport("@rpath/sokol.framework/sokol", EntryPoint = "nvgDeleteSokol", CallingConvention = CallingConvention.Cdecl)]
#else
[DllImport("sokol", EntryPoint = "nvgDeleteSokol", CallingConvention = CallingConvention.Cdecl)]
#endif
public static extern void nvgDeleteSokol(IntPtr ctx);

#if __IOS__
[DllImport("@rpath/sokol.framework/sokol", EntryPoint = "nvgRGB_internal", CallingConvention = CallingConvention.Cdecl)]
#else
[DllImport("sokol", EntryPoint = "nvgRGB_internal", CallingConvention = CallingConvention.Cdecl)]
#endif
public static extern void nvgRGB_internal(ref NVGcolor result, byte r, byte g, byte b);

#if __IOS__
[DllImport("@rpath/sokol.framework/sokol", EntryPoint = "nvgRGBf_internal", CallingConvention = CallingConvention.Cdecl)]
#else
[DllImport("sokol", EntryPoint = "nvgRGBf_internal", CallingConvention = CallingConvention.Cdecl)]
#endif
public static extern void nvgRGBf_internal(ref NVGcolor result, float r, float g, float b);

#if __IOS__
[DllImport("@rpath/sokol.framework/sokol", EntryPoint = "nvgRGBA_internal", CallingConvention = CallingConvention.Cdecl)]
#else
[DllImport("sokol", EntryPoint = "nvgRGBA_internal", CallingConvention = CallingConvention.Cdecl)]
#endif
public static extern void nvgRGBA_internal(ref NVGcolor result, byte r, byte g, byte b, byte a);

#if __IOS__
[DllImport("@rpath/sokol.framework/sokol", EntryPoint = "nvgRGBAf_internal", CallingConvention = CallingConvention.Cdecl)]
#else
[DllImport("sokol", EntryPoint = "nvgRGBAf_internal", CallingConvention = CallingConvention.Cdecl)]
#endif
public static extern void nvgRGBAf_internal(ref NVGcolor result, float r, float g, float b, float a);

#if __IOS__
[DllImport("@rpath/sokol.framework/sokol", EntryPoint = "nvgLerpRGBA_internal", CallingConvention = CallingConvention.Cdecl)]
#else
[DllImport("sokol", EntryPoint = "nvgLerpRGBA_internal", CallingConvention = CallingConvention.Cdecl)]
#endif
public static extern void nvgLerpRGBA_internal(ref NVGcolor result, NVGcolor c0, NVGcolor c1, float u);

#if __IOS__
[DllImport("@rpath/sokol.framework/sokol", EntryPoint = "nvgTransRGBA_internal", CallingConvention = CallingConvention.Cdecl)]
#else
[DllImport("sokol", EntryPoint = "nvgTransRGBA_internal", CallingConvention = CallingConvention.Cdecl)]
#endif
public static extern void nvgTransRGBA_internal(ref NVGcolor result, NVGcolor c0, byte a);

#if __IOS__
[DllImport("@rpath/sokol.framework/sokol", EntryPoint = "nvgTransRGBAf_internal", CallingConvention = CallingConvention.Cdecl)]
#else
[DllImport("sokol", EntryPoint = "nvgTransRGBAf_internal", CallingConvention = CallingConvention.Cdecl)]
#endif
public static extern void nvgTransRGBAf_internal(ref NVGcolor result, NVGcolor c0, float a);

#if __IOS__
[DllImport("@rpath/sokol.framework/sokol", EntryPoint = "nvgHSL_internal", CallingConvention = CallingConvention.Cdecl)]
#else
[DllImport("sokol", EntryPoint = "nvgHSL_internal", CallingConvention = CallingConvention.Cdecl)]
#endif
public static extern void nvgHSL_internal(ref NVGcolor result, float h, float s, float l);

#if __IOS__
[DllImport("@rpath/sokol.framework/sokol", EntryPoint = "nvgHSLA_internal", CallingConvention = CallingConvention.Cdecl)]
#else
[DllImport("sokol", EntryPoint = "nvgHSLA_internal", CallingConvention = CallingConvention.Cdecl)]
#endif
public static extern void nvgHSLA_internal(ref NVGcolor result, float h, float s, float l, byte a);

#if __IOS__
[DllImport("@rpath/sokol.framework/sokol", EntryPoint = "nvgLinearGradient_internal", CallingConvention = CallingConvention.Cdecl)]
#else
[DllImport("sokol", EntryPoint = "nvgLinearGradient_internal", CallingConvention = CallingConvention.Cdecl)]
#endif
public static extern void nvgLinearGradient_internal(ref NVGpaint result, IntPtr ctx, float sx, float sy, float ex, float ey, NVGcolor icol, NVGcolor ocol);

#if __IOS__
[DllImport("@rpath/sokol.framework/sokol", EntryPoint = "nvgBoxGradient_internal", CallingConvention = CallingConvention.Cdecl)]
#else
[DllImport("sokol", EntryPoint = "nvgBoxGradient_internal", CallingConvention = CallingConvention.Cdecl)]
#endif
public static extern void nvgBoxGradient_internal(ref NVGpaint result, IntPtr ctx, float x, float y, float w, float h, float r, float f, NVGcolor icol, NVGcolor ocol);

#if __IOS__
[DllImport("@rpath/sokol.framework/sokol", EntryPoint = "nvgRadialGradient_internal", CallingConvention = CallingConvention.Cdecl)]
#else
[DllImport("sokol", EntryPoint = "nvgRadialGradient_internal", CallingConvention = CallingConvention.Cdecl)]
#endif
public static extern void nvgRadialGradient_internal(ref NVGpaint result, IntPtr ctx, float cx, float cy, float inr, float outr, NVGcolor icol, NVGcolor ocol);

#if __IOS__
[DllImport("@rpath/sokol.framework/sokol", EntryPoint = "nvgImagePattern_internal", CallingConvention = CallingConvention.Cdecl)]
#else
[DllImport("sokol", EntryPoint = "nvgImagePattern_internal", CallingConvention = CallingConvention.Cdecl)]
#endif
public static extern void nvgImagePattern_internal(ref NVGpaint result, IntPtr ctx, float ox, float oy, float ex, float ey, float angle, int image, float alpha);

}
}
