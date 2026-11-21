using System;
using System.Runtime.InteropServices;
using System.Runtime.CompilerServices;

namespace Sokol
{
    public static unsafe partial class NanoVG
    {
        /// <summary>
        /// NanoVG color value (RGBA floats).
        /// Mirrors the NVGcolor union in nanovg.h — the rgba[] array and the
        /// individual r/g/b/a fields are laid out at the same memory address.
        /// </summary>
        [StructLayout(LayoutKind.Sequential)]
        public struct NVGcolor
        {
            public float r;
            public float g;
            public float b;
            public float a;

            public NVGcolor(float r, float g, float b, float a = 1.0f)
            {
                this.r = r;
                this.g = g;
                this.b = b;
                this.a = a;
            }

            /// <summary>Access rgba components by index (0=r, 1=g, 2=b, 3=a).</summary>
            public unsafe float this[int index]
            {
                get => new ReadOnlySpan<float>(Unsafe.AsPointer(ref r), 4)[index];
                set => new Span<float>(Unsafe.AsPointer(ref r), 4)[index] = value;
            }

            public override string ToString() => $"NVGcolor(r={r}, g={g}, b={b}, a={a})";
        }

        // ─── Raw structs for unsafe byte* text APIs ──────────────────────────────
        // The generated NVGtextRow / NVGglyphPosition have string-marshaled fields
        // which are unusable when accessed through an unsafe pointer (the managed
        // string reference is in the same memory slot as the native char*, which
        // would corrupt the GC).  These raw variants mirror the native layout.

        /// <summary>
        /// Unsafe-pointer-safe version of NVGtextRow.
        /// Use with the byte* overloads of nvgTextBreakLines / nvgText etc.
        /// </summary>
        [StructLayout(LayoutKind.Sequential)]
        public struct NVGtextRowRaw
        {
            public byte* start;  // pointer into the UTF-8 input buffer (row start)
            public byte* end;    // pointer one past the last byte of this row
            public byte* next;   // pointer to the start of the next row (or end-of-string)
            public float width;
            public float minx;
            public float maxx;
        }

        /// <summary>
        /// Unsafe-pointer-safe version of NVGglyphPosition.
        /// Use with the byte* overload of nvgTextGlyphPositions.
        /// </summary>
        [StructLayout(LayoutKind.Sequential)]
        public struct NVGglyphPositionRaw
        {
            public byte* str;   // pointer to the glyph in the UTF-8 input buffer
            public float x;
            public float minx;
            public float maxx;
        }

        // ─── Raw byte* overloads — same native entry points as the string versions ─

#if __IOS__
        [DllImport("@rpath/sokol.framework/sokol", EntryPoint = "nvgText", CallingConvention = CallingConvention.Cdecl)]
#else
        [DllImport("sokol", EntryPoint = "nvgText", CallingConvention = CallingConvention.Cdecl)]
#endif
        public static extern float nvgText(IntPtr ctx, float x, float y, byte* str, byte* end);

#if __IOS__
        [DllImport("@rpath/sokol.framework/sokol", EntryPoint = "nvgTextBox", CallingConvention = CallingConvention.Cdecl)]
#else
        [DllImport("sokol", EntryPoint = "nvgTextBox", CallingConvention = CallingConvention.Cdecl)]
#endif
        public static extern void nvgTextBox(IntPtr ctx, float x, float y, float breakRowWidth, byte* str, byte* end);

#if __IOS__
        [DllImport("@rpath/sokol.framework/sokol", EntryPoint = "nvgTextBounds", CallingConvention = CallingConvention.Cdecl)]
#else
        [DllImport("sokol", EntryPoint = "nvgTextBounds", CallingConvention = CallingConvention.Cdecl)]
#endif
        public static extern float nvgTextBounds(IntPtr ctx, float x, float y, byte* str, byte* end, float* bounds);

#if __IOS__
        [DllImport("@rpath/sokol.framework/sokol", EntryPoint = "nvgTextBoxBounds", CallingConvention = CallingConvention.Cdecl)]
#else
        [DllImport("sokol", EntryPoint = "nvgTextBoxBounds", CallingConvention = CallingConvention.Cdecl)]
#endif
        public static extern void nvgTextBoxBounds(IntPtr ctx, float x, float y, float breakRowWidth, byte* str, byte* end, float* bounds);

#if __IOS__
        [DllImport("@rpath/sokol.framework/sokol", EntryPoint = "nvgTextGlyphPositions", CallingConvention = CallingConvention.Cdecl)]
#else
        [DllImport("sokol", EntryPoint = "nvgTextGlyphPositions", CallingConvention = CallingConvention.Cdecl)]
#endif
        public static extern int nvgTextGlyphPositions(IntPtr ctx, float x, float y, byte* str, byte* end, NVGglyphPositionRaw* positions, int maxPositions);

#if __IOS__
        [DllImport("@rpath/sokol.framework/sokol", EntryPoint = "nvgTextBreakLines", CallingConvention = CallingConvention.Cdecl)]
#else
        [DllImport("sokol", EntryPoint = "nvgTextBreakLines", CallingConvention = CallingConvention.Cdecl)]
#endif
        public static extern int nvgTextBreakLines(IntPtr ctx, byte* str, byte* end, float breakRowWidth, NVGtextRowRaw* rows, int maxRows);
    }
}
