// machine generated, do not edit
using System;
using System.Runtime.InteropServices;
using M = System.Runtime.InteropServices.MarshalAsAttribute;
using U = System.Runtime.InteropServices.UnmanagedType;

namespace Sokol
{
public static unsafe partial class NanoSVG
{
public enum NSVGpaintType
{
    NSVG_PAINT_UNDEF = -1,
    NSVG_PAINT_NONE = 0,
    NSVG_PAINT_COLOR = 1,
    NSVG_PAINT_LINEAR_GRADIENT = 2,
    NSVG_PAINT_RADIAL_GRADIENT = 3,
}
public enum NSVGspreadType
{
    NSVG_SPREAD_PAD = 0,
    NSVG_SPREAD_REFLECT = 1,
    NSVG_SPREAD_REPEAT = 2,
}
public enum NSVGlineJoin
{
    NSVG_JOIN_MITER = 0,
    NSVG_JOIN_ROUND = 1,
    NSVG_JOIN_BEVEL = 2,
}
public enum NSVGlineCap
{
    NSVG_CAP_BUTT = 0,
    NSVG_CAP_ROUND = 1,
    NSVG_CAP_SQUARE = 2,
}
public enum NSVGfillRule
{
    NSVG_FILLRULE_NONZERO = 0,
    NSVG_FILLRULE_EVENODD = 1,
}
public enum NSVGflags
{
    NSVG_FLAGS_VISIBLE = 1,
}
public enum NSVGpaintOrder
{
    NSVG_PAINT_FILL = 0,
    NSVG_PAINT_MARKERS = 1,
    NSVG_PAINT_STROKE = 2,
}
[StructLayout(LayoutKind.Sequential)]
public struct NSVGgradientStop
{
    public uint color;
    public float offset;
}
[StructLayout(LayoutKind.Sequential)]
public struct NSVGgradient
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
    public byte spread;
    public float fx;
    public float fy;
    public int nstops;
    #pragma warning disable 169
    public struct stopsCollection
    {
        public ref NSVGgradientStop this[int index] => ref MemoryMarshal.CreateSpan(ref _item0, 1)[index];
        private NSVGgradientStop _item0;
    }
    #pragma warning restore 169
    public stopsCollection stops;
}
[StructLayout(LayoutKind.Sequential)]
public struct NSVGpath
{
    public float* pts;
    public int npts;
    public byte closed;
    #pragma warning disable 169
    public struct boundsCollection
    {
        public ref float this[int index] => ref MemoryMarshal.CreateSpan(ref _item0, 4)[index];
        private float _item0;
        private float _item1;
        private float _item2;
        private float _item3;
    }
    #pragma warning restore 169
    public boundsCollection bounds;
    public NSVGpath* next;
}
[StructLayout(LayoutKind.Sequential)]
public struct NSVGshape
{
    #pragma warning disable 169
    public struct idCollection
    {
        public ref byte this[int index] => ref MemoryMarshal.CreateSpan(ref _item0, 64)[index];
        private byte _item0;
        private byte _item1;
        private byte _item2;
        private byte _item3;
        private byte _item4;
        private byte _item5;
        private byte _item6;
        private byte _item7;
        private byte _item8;
        private byte _item9;
        private byte _item10;
        private byte _item11;
        private byte _item12;
        private byte _item13;
        private byte _item14;
        private byte _item15;
        private byte _item16;
        private byte _item17;
        private byte _item18;
        private byte _item19;
        private byte _item20;
        private byte _item21;
        private byte _item22;
        private byte _item23;
        private byte _item24;
        private byte _item25;
        private byte _item26;
        private byte _item27;
        private byte _item28;
        private byte _item29;
        private byte _item30;
        private byte _item31;
        private byte _item32;
        private byte _item33;
        private byte _item34;
        private byte _item35;
        private byte _item36;
        private byte _item37;
        private byte _item38;
        private byte _item39;
        private byte _item40;
        private byte _item41;
        private byte _item42;
        private byte _item43;
        private byte _item44;
        private byte _item45;
        private byte _item46;
        private byte _item47;
        private byte _item48;
        private byte _item49;
        private byte _item50;
        private byte _item51;
        private byte _item52;
        private byte _item53;
        private byte _item54;
        private byte _item55;
        private byte _item56;
        private byte _item57;
        private byte _item58;
        private byte _item59;
        private byte _item60;
        private byte _item61;
        private byte _item62;
        private byte _item63;
    }
    #pragma warning restore 169
    public idCollection id;
    public NSVGpaint fill;
    public NSVGpaint stroke;
    public float opacity;
    public float strokeWidth;
    public float strokeDashOffset;
    #pragma warning disable 169
    public struct strokeDashArrayCollection
    {
        public ref float this[int index] => ref MemoryMarshal.CreateSpan(ref _item0, 8)[index];
        private float _item0;
        private float _item1;
        private float _item2;
        private float _item3;
        private float _item4;
        private float _item5;
        private float _item6;
        private float _item7;
    }
    #pragma warning restore 169
    public strokeDashArrayCollection strokeDashArray;
    public byte strokeDashCount;
    public byte strokeLineJoin;
    public byte strokeLineCap;
    public float miterLimit;
    public byte fillRule;
    public byte paintOrder;
    public byte flags;
    #pragma warning disable 169
    public struct boundsCollection
    {
        public ref float this[int index] => ref MemoryMarshal.CreateSpan(ref _item0, 4)[index];
        private float _item0;
        private float _item1;
        private float _item2;
        private float _item3;
    }
    #pragma warning restore 169
    public boundsCollection bounds;
    #pragma warning disable 169
    public struct fillGradientCollection
    {
        public ref byte this[int index] => ref MemoryMarshal.CreateSpan(ref _item0, 64)[index];
        private byte _item0;
        private byte _item1;
        private byte _item2;
        private byte _item3;
        private byte _item4;
        private byte _item5;
        private byte _item6;
        private byte _item7;
        private byte _item8;
        private byte _item9;
        private byte _item10;
        private byte _item11;
        private byte _item12;
        private byte _item13;
        private byte _item14;
        private byte _item15;
        private byte _item16;
        private byte _item17;
        private byte _item18;
        private byte _item19;
        private byte _item20;
        private byte _item21;
        private byte _item22;
        private byte _item23;
        private byte _item24;
        private byte _item25;
        private byte _item26;
        private byte _item27;
        private byte _item28;
        private byte _item29;
        private byte _item30;
        private byte _item31;
        private byte _item32;
        private byte _item33;
        private byte _item34;
        private byte _item35;
        private byte _item36;
        private byte _item37;
        private byte _item38;
        private byte _item39;
        private byte _item40;
        private byte _item41;
        private byte _item42;
        private byte _item43;
        private byte _item44;
        private byte _item45;
        private byte _item46;
        private byte _item47;
        private byte _item48;
        private byte _item49;
        private byte _item50;
        private byte _item51;
        private byte _item52;
        private byte _item53;
        private byte _item54;
        private byte _item55;
        private byte _item56;
        private byte _item57;
        private byte _item58;
        private byte _item59;
        private byte _item60;
        private byte _item61;
        private byte _item62;
        private byte _item63;
    }
    #pragma warning restore 169
    public fillGradientCollection fillGradient;
    #pragma warning disable 169
    public struct strokeGradientCollection
    {
        public ref byte this[int index] => ref MemoryMarshal.CreateSpan(ref _item0, 64)[index];
        private byte _item0;
        private byte _item1;
        private byte _item2;
        private byte _item3;
        private byte _item4;
        private byte _item5;
        private byte _item6;
        private byte _item7;
        private byte _item8;
        private byte _item9;
        private byte _item10;
        private byte _item11;
        private byte _item12;
        private byte _item13;
        private byte _item14;
        private byte _item15;
        private byte _item16;
        private byte _item17;
        private byte _item18;
        private byte _item19;
        private byte _item20;
        private byte _item21;
        private byte _item22;
        private byte _item23;
        private byte _item24;
        private byte _item25;
        private byte _item26;
        private byte _item27;
        private byte _item28;
        private byte _item29;
        private byte _item30;
        private byte _item31;
        private byte _item32;
        private byte _item33;
        private byte _item34;
        private byte _item35;
        private byte _item36;
        private byte _item37;
        private byte _item38;
        private byte _item39;
        private byte _item40;
        private byte _item41;
        private byte _item42;
        private byte _item43;
        private byte _item44;
        private byte _item45;
        private byte _item46;
        private byte _item47;
        private byte _item48;
        private byte _item49;
        private byte _item50;
        private byte _item51;
        private byte _item52;
        private byte _item53;
        private byte _item54;
        private byte _item55;
        private byte _item56;
        private byte _item57;
        private byte _item58;
        private byte _item59;
        private byte _item60;
        private byte _item61;
        private byte _item62;
        private byte _item63;
    }
    #pragma warning restore 169
    public strokeGradientCollection strokeGradient;
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
    public NSVGpath* paths;
    public NSVGshape* next;
}
[StructLayout(LayoutKind.Sequential)]
public struct NSVGimage
{
    public float width;
    public float height;
    public NSVGshape* shapes;
}
#if __IOS__
[DllImport("@rpath/sokol.framework/sokol", EntryPoint = "nsvgParse", CallingConvention = CallingConvention.Cdecl)]
#else
[DllImport("sokol", EntryPoint = "nsvgParse", CallingConvention = CallingConvention.Cdecl)]
#endif
public static extern NSVGimage* nsvgParse(IntPtr input, [M(U.LPUTF8Str)] string units, float dpi);

#if __IOS__
[DllImport("@rpath/sokol.framework/sokol", EntryPoint = "nsvgDuplicatePath", CallingConvention = CallingConvention.Cdecl)]
#else
[DllImport("sokol", EntryPoint = "nsvgDuplicatePath", CallingConvention = CallingConvention.Cdecl)]
#endif
public static extern NSVGpath* nsvgDuplicatePath(NSVGpath* p);

#if __IOS__
[DllImport("@rpath/sokol.framework/sokol", EntryPoint = "nsvgDelete", CallingConvention = CallingConvention.Cdecl)]
#else
[DllImport("sokol", EntryPoint = "nsvgDelete", CallingConvention = CallingConvention.Cdecl)]
#endif
public static extern void nsvgDelete(NSVGimage* image);

}
}
