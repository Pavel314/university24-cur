using System.Numerics;
using System.Runtime.CompilerServices;
using static System.Runtime.CompilerServices.MethodImplOptions;
using System.Runtime.Intrinsics;
using System.Buffers.Binary;
using System.Collections.Immutable;
using System.Diagnostics;

namespace t7_comp_graph
{
    using Impl = MethodImplAttribute;

    public readonly record struct Vector
    {
        public static readonly Vector Zero = new Vector(0,0);
        private readonly Vector128<double> v;
        public double X => v.GetElement(0);
        public double Y => v.GetElement(1);
        [Impl(AggressiveInlining)] public Vector(double x, double y) => v = Vector128.Create(x, y);
        [Impl(AggressiveInlining)] public static Vector FromAngle(double ang) { var (s, c) = Math.SinCos(ang); return new(c, s); }
        [Impl(AggressiveInlining)] private Vector(Vector128<double> v) => this.v = v;
        [Impl(AggressiveInlining)] public static double Dot(Vector v1, Vector v2) => Vector128.Dot(v1.v, v2.v);
        [Impl(AggressiveInlining)] public double LengthSquared() => Dot(this, this);
        [Impl(AggressiveInlining)] public double Length() => Math.Sqrt(LengthSquared());
        [Impl(AggressiveInlining)] public static double Cross(Vector v1, Vector v2) => v1.X * v2.Y - v1.Y * v2.X;
        [Impl(AggressiveInlining)] public static Vector Lerp(Vector v1, Vector v2, double t) => new(Vector128.Lerp(v1.v, v2.v, Vector128.Create(t)));
        [Impl(AggressiveInlining)] public Vector SafeDiv(double s, Vector dflt) => Math.Abs(s) < 1e-12 ? dflt : this / s;
        [Impl(AggressiveInlining)] public Vector Norm() => SafeDiv(Length(), new Vector(0, 0));
        [Impl(AggressiveInlining)] public Vector SafeNorm(Vector dflt) => SafeDiv(Length(), dflt);
        [Impl(AggressiveInlining)] public static Vector operator +(Vector v1, Vector v2) => new(v1.v + v2.v);
        [Impl(AggressiveInlining)] public static Vector operator -(Vector v1, Vector v2) => new(v1.v - v2.v);
        [Impl(AggressiveInlining)] public static Vector operator *(Vector v, double s) => new(v.v * s);
        [Impl(AggressiveInlining)] public static Vector operator *(double s, Vector v) => v * s;
        [Impl(AggressiveInlining)] public static double operator *(Vector v1, Vector v2) => Dot(v1, v2);
        [Impl(AggressiveInlining)] public static Vector operator /(Vector v, double s) => new(v.v / s);
        [Impl(AggressiveInlining)] public static Vector operator -(Vector v) => new(-v.v);

        [Impl(AggressiveInlining)] public static implicit operator Vector((double X, double Y) t) => new(t.X, t.Y);
        //TODO hack, should be deleted
        [Impl(AggressiveInlining)] public static implicit operator System.Numerics.Vector2(Vector v) => new((float)v.X, (float)v.Y);
        [Impl(AggressiveInlining)] public static implicit operator Vector(System.Numerics.Vector2 v) => new(v.X, v.Y);

        [Impl(AggressiveInlining)] public static Vector Min(Vector v1, Vector v2) => new(Vector128.Min(v1.v, v2.v));
        [Impl(AggressiveInlining)] public static Vector Max(Vector v1, Vector v2) => new(Vector128.Max(v1.v, v2.v));

        [Impl(AggressiveInlining)]
        public Vector Rotated(double ang){var (s, c) = Math.SinCos(ang); return new(X * c - Y * s, X * s + Y * c);}
    }

    public readonly record struct Transform
    {
        private readonly double _cos;
        private readonly double _sin;
        public readonly Vector Pos;
        public readonly double Ang;
        public static readonly Transform Empty = new((0, 0), 0);
        [Impl(AggressiveInlining)]
        public Transform(Vector pos, double ang = 0)
        {
            Pos = pos;
            Ang = ang;
            (_sin, _cos) = Math.SinCos(ang);
        }
        [Impl(AggressiveInlining)]
        private Transform(Vector pos, double ang, double cos, double sin)
        {
            Pos = pos;
            Ang = ang;
            (_sin, _cos) = (sin, cos);
        }
        [Impl(AggressiveInlining)] public Transform Replace(Vector? pos = null, double? ang = null) => ang.HasValue ? new Transform(pos ?? Pos, ang.Value) : new Transform(pos ?? Pos, Ang, _cos, _sin);
        [Impl(AggressiveInlining)] public readonly Vector AbsApplyDir(Vector v) => new(Math.Abs(v.X * _cos) + Math.Abs(v.Y * _sin), Math.Abs(v.X * _sin) + Math.Abs(v.Y * _cos));
        [Impl(AggressiveInlining)] public readonly Vector ApplyDir(Vector v) => new(v.X * _cos - v.Y * _sin, v.X * _sin + v.Y * _cos);
        [Impl(AggressiveInlining)] public readonly Vector Apply(Vector v) => ApplyDir(v) + Pos;
        [Impl(AggressiveInlining)] public readonly Vector UnapplyDir(Vector v) => new(v.X * _cos + v.Y * _sin, -v.X * _sin + v.Y * _cos);
        [Impl(AggressiveInlining)] public readonly Vector Unapply(Vector v) => UnapplyDir(v - Pos);
        [Impl(AggressiveInlining)] public readonly Transform Moved(Vector delta) => new(Pos + delta, Ang, _cos, _sin);
        [Impl(AggressiveInlining)] public readonly Transform Rotated(double delta) => new(Pos.Rotated(delta), Ang + delta);
        [Impl(AggressiveInlining)] public readonly Transform RotatedLocal(double delta) => new(Pos, Ang + delta);
    }

    public struct BoundBox(double minX, double minY, double maxX, double maxY)
    {
        public double MinX = minX, MinY = minY, MaxX = maxX, MaxY = maxY;

        public readonly double X => MinX;
        public readonly double Y => MinY;
        public readonly double Width => MaxX - MinX;
        public readonly double Height => MaxY - MinY;
        public Vector Min{ readonly get => new(MinX, MinY); set { MinX = value.X; MinY = value.Y; }}
        public Vector Max { readonly get => new(MaxX, MaxY); set { MaxX = value.X; MaxY = value.Y; } }
        public readonly Vector Pos => new(X, Y);
        public readonly Vector Size => new(Width, Height);


        public static BoundBox FromMinMax(double minX, double minY, double maxX, double maxY) => new(minX, minY, maxX, maxY);
        public static BoundBox FromMinMax(double min, double max) => new(min, min, max, max);
        public static BoundBox FromMinMax(Vector min, Vector max) => FromMinMax(min.X, min.Y, max.X, max.Y);
        public static BoundBox FromUnsortMinMax(Vector a, Vector b) => FromMinMax(Vector.Min(a,b), Vector.Max(a,b));

        public static BoundBox FromCenter(double w, double h) => new(-w * 0.5, -h * 0.5, w * 0.5, h * 0.5);
        public static BoundBox FromCenter(double w, double h, Vector center) => FromCenter(w, h).Moved(center);
        public static BoundBox FromXywh(double x, double y, double w, double h) => new(x, y, x + w, y + h);
        public static BoundBox FromXywh(Vector pos, Vector size) => FromXywh(pos.X, pos.Y, size.X, size.Y);
        public static BoundBox Empty() => new(double.MaxValue, double.MaxValue, double.MinValue, double.MinValue);

        public void Extend(Vector v)
        {
            Min = Vector.Min(Min, v);
            Max = Vector.Max(Max, v);
        }

        public void Extend(BoundBox o)
        {
            Min = Vector.Min(Min, o.Min);
            Max = Vector.Max(Max, o.Max);
        }

        public readonly BoundBox Expand(double left, double top, double right, double bottom) => new(MinX - left, MinY - bottom, MaxX + right, MaxY + top);
        public readonly BoundBox Expand(double hor, double ver) => Expand(hor, ver, hor, ver);
        public readonly BoundBox Expand(double pad) => Expand(pad, pad);
        public readonly bool Contains(Vector v) => v.X >= MinX && v.X <= MaxX && v.Y >= MinY && v.Y <= MaxY;

        public static bool StrictEquals(BoundBox a, BoundBox b) =>
           a.MinX == b.MinX && a.MinY == b.MinY &&
           a.MaxX == b.MaxX && a.MaxY == b.MaxY;
        
        public readonly BoundBox Moved(Vector delta) => new(MinX + delta.X, MinY + delta.Y, MaxX + delta.X, MaxY + delta.Y);
        public readonly BoundBox ApplyTransform(Transform tr)
        {
            Vector h = HalfExtents;
            Vector wide = tr.AbsApplyDir(h);
            Vector center = tr.Apply(Center);
            return FromMinMax(center - wide, center + wide);
        }
        public readonly Vector Center => new((MinX + MaxX) * 0.5, (MinY + MaxY) * 0.5);
        public readonly Vector HalfExtents => new((MaxX - MinX) * 0.5, (MaxY - MinY) * 0.5);
    }

    public class Camera
    {
        private double zoom;
        public Transform Tr;
        public double MinZoom;
        public double MaxZoom;
        public double Zoom { get => zoom; set => zoom = Math.Clamp(value, MinZoom, MaxZoom); }

        public Vector Pos { get => Tr.Pos; set => Tr = Tr.Replace(pos:value); }
        public double Ang { get => Tr.Ang; set => Tr = Tr.Replace(ang:value); }

        public Camera(Transform tr, double zoom = 1, double minZoom = double.NegativeInfinity, double maxZoom = double.PositiveInfinity)
        {
            Tr = tr;
            (MinZoom, MaxZoom) = (minZoom, maxZoom);
            Zoom = zoom;
        }

        public void Moved(Vector delta) => Tr = Tr.Moved(delta);
        public void Rotated(double delta) => Tr = Tr.RotatedLocal(delta);

        public void ZoomAt(Vector focus, double factor)
        {
            var oldZoom = zoom;
            Zoom *= factor;
            Pos = focus + (Pos - focus) / (Zoom/oldZoom);
        }
    }

    public class Viewport(Camera cam, BoundBox screen)
    {
        public Camera Cam = cam;
        public BoundBox Screen = screen;
        private const int ScreenYDir = -1;
        public BoundBox GetWorldBBox() => BoundBox.FromCenter(Screen.Width / Cam.Zoom, Screen.Height / Cam.Zoom).ApplyTransform(Cam.Tr);
        public void Resize(BoundBox screen) => Screen = screen;

        public Vector ToScreenDir(Vector worldDir)
        {
            Vector local = Cam.Tr.UnapplyDir(worldDir) * Cam.Zoom;
            return new Vector(local.X, local.Y * ScreenYDir);
        }
        public double ToScreen(double world) => world * Cam.Zoom;
        public Vector ToScreen(Vector world) => Screen.Center + ToScreenDir(world - Cam.Pos);
        public BoundBox ToScreen(BoundBox world)
        {
            Vector wide = Cam.Tr.AbsApplyDir(world.Size) * Cam.Zoom;
            return BoundBox.FromCenter(wide.X, wide.Y, ToScreen(world.Center));
        }

        public Vector ToWorldDir(Vector screenDir)
        {
            Vector local = new(screenDir.X, screenDir.Y * ScreenYDir);
            return Cam.Tr.ApplyDir(local / Cam.Zoom);
        }
        public Vector ToWorld(Vector screen) => Cam.Pos + ToWorldDir(screen - Screen.Center);
        public double ToWorld(double screen) => screen / Cam.Zoom;
    }

    public readonly record struct RgbaColor(uint Rgba)
    {
        public static RgbaColor None => default;
        public bool IsVisible => A > 0;
        public RgbaColor(byte r, byte g, byte b, byte a = 255) : this((uint)((r << 24) | (g << 16) | (b << 8) | (a << 0))) { }
        public static RgbaColor FromArgb(uint argb) => new(BitOperations.RotateLeft(argb, 8));
        public RgbaColor Replace(byte? r = null, byte? g = null, byte? b = null, byte? a = null) => new(r ?? R, g ?? G, b ?? B, a ?? A);
        public byte R => (byte)((Rgba >> 24) & 0xFF);
        public byte G => (byte)((Rgba >> 16) & 0xFF);
        public byte B => (byte)((Rgba >> 8) & 0xFF);
        public byte A => (byte)((Rgba >> 0) & 0xFF);
        public uint Argb => BitOperations.RotateRight(Rgba, 8);
        public uint Abgr => BinaryPrimitives.ReverseEndianness(Rgba);
        public uint ImGui => Abgr;

        public Vector3 ToVector3() => new Vector3(R, G, B) / 255f;
        public static RgbaColor FromVector3(Vector3 color, byte alpha = 255)
        {
            static byte ToByte(float v) => (byte)Math.Round(Math.Clamp(v, 0f, 1f) * 255f);
            return new RgbaColor(ToByte(color.X), ToByte(color.Y), ToByte(color.Z), alpha);
        }
    }

    public readonly record struct Option<T>(bool Enabled, T Value)
    {
        public static implicit operator Option<T>(T value) => new(true, value);
        public static implicit operator bool(Option<T> arg) => arg.Enabled;
        public bool IsEnabled(out T value){value = Value; return Enabled;}
        public static T operator |(Option<T> arg, T defaultValue) => arg.Enabled ? arg.Value : defaultValue;
    }

    public static class EnumUtils<TEnum> where TEnum : struct, Enum
    {
        public static readonly string[] Names = Enum.GetNames<TEnum>();

        static EnumUtils()
        {
            if (Enum.GetUnderlyingType(typeof(TEnum)) != typeof(int))
                throw new NotSupportedException($"The enum {typeof(TEnum).Name} must have an underlying type of int.");
        }
        public static string? GetName(TEnum value) => Enum.GetName(value);
        public static int ToInt(TEnum selected) => Unsafe.As<TEnum, int>(ref selected);
        public static TEnum FromInt(int selected) => Unsafe.As<int, TEnum>(ref selected);
    }

    public readonly record struct Point(int X, int Y)
    {
        public static readonly Point Zero = new(0, 0);
        public static Point RoundLerp(Point a, Point b, double t) => new((int)Math.Round(a.X + (b.X - a.X) * t), (int)Math.Round(a.Y + (b.Y - a.Y) * t));
        [Impl(AggressiveInlining)] public static Point operator +(Point a, Point b) => new(a.X + b.X, a.Y + b.Y);
        [Impl(AggressiveInlining)] public static Point operator -(Point a, Point b) => new(a.X - b.X, a.Y - b.Y);
        [Impl(AggressiveInlining)] public static Point operator *(Point p, int s) => new(p.X * s, p.Y * s);
        [Impl(AggressiveInlining)] public static Point operator *(int s, Point p) => p * s;
        [Impl(AggressiveInlining)] public static Point operator /(Point p, int s) => new(p.X / s, p.Y / s);
        [Impl(AggressiveInlining)] public static Point operator -(Point p) => new(-p.X, -p.Y);
        [Impl(AggressiveInlining)] public Vector ToVec() => (X, Y);
        [Impl(AggressiveInlining)] public double Length() => double.Hypot(X,Y);
        [Impl(AggressiveInlining)] public static implicit operator Point((int X, int Y) t) => new(t.X, t.Y);

        [Impl(AggressiveInlining)] public Point MirrorX(int axisX) => new(2 * axisX - X, Y);
        [Impl(AggressiveInlining)] public Point MirrorY(int axisY) => new(X, 2 * axisY - Y);
        [Impl(AggressiveInlining)] public Point Mirror(Point center) => new(2 * center.X - X, 2 * center.Y - Y);

        public static Point FromRound(Vector pt) => new((int)Math.Round(pt.X), (int)Math.Round(pt.Y));
    }


    public readonly record struct CellRegion(Point Start, Point End)
    {
        public Point Dir => End - Start;
        public int SignWidth => End.X - Start.X;
        public int SignHeight => End.Y - Start.Y;
        public int Width => Math.Abs(SignWidth);
        public int Height => Math.Abs(SignHeight);

        public BoundBox ToBoundBox() => BoundBox.FromUnsortMinMax(Start.ToVec(), End.ToVec()).Expand(0.5);
        public Point Center => new((Start.X + End.X) / 2, (Start.Y + End.Y) / 2);

        public enum FitMode { Expand, Shrink }
        public static CellRegion FromVectors(Vector start, Vector end, FitMode mode)
        {
            var dir = mode == FitMode.Shrink ? end - start : start - end;
            static int Round(double v, double d) => d > 0 ? (int)Math.Ceiling(v + 0.5) : d < 0 ? (int)Math.Floor(v - 0.5) : (int)Math.Round(v);
            return new(
                new(Round(start.X, dir.X), Round(start.Y, dir.Y)),
                new(Round(end.X, -dir.X), Round(end.Y, -dir.Y))
            );
        }
    }

    public readonly record struct AlphaPixel(Point Pos, double Alpha)
    {
        public static implicit operator AlphaPixel((Point Pos, double Alpha) t) => new(t.Pos, t.Alpha);
        public static implicit operator AlphaPixel((int X, int Y, double Alpha) t) => new(new Point(t.X, t.Y), t.Alpha);
    }
    public readonly record struct Pen(RgbaColor Color)
    {
        public bool IsVisible => Color.IsVisible;
        public RgbaColor Apply(AlphaPixel pix)
        {
            var alpha = (byte)Math.Clamp(Math.Round(Color.A * pix.Alpha), 0, 255);
            return new RgbaColor(Color.R, Color.G, Color.B, alpha);
        }
    }

    public readonly record struct TransformedShape(Transform Tr, Pen Pen, Shape Shape)
    {
        public BoundBox Bounds => Shape.Bounds.ApplyTransform(Tr);
    }

    public class Statistics
    {
        public int MaxVerts;
        public int MaxIdx;
        public int VertsCount;
        public int IdxCount;
        public float Fps;

        public void CaptureBufferSize(int maxVerts, int maxIdx)
        {
            MaxVerts = maxVerts;
            MaxIdx = maxIdx;
        }

        public void CaptureFrame(int vertsCount, int idxCount, float fps)
        {
            VertsCount = vertsCount;
            IdxCount = idxCount;
            Fps = fps;
        }
    }

    public readonly record struct DebugRasterStep(int PixelCount, string Desc)
    {
        public static implicit operator DebugRasterStep((int PixelCount, string Desc) t) => new(t.PixelCount, t.Desc);
    }
    public readonly record struct DebugRasterResult(ImmutableArray<AlphaPixel> Pixels, ImmutableArray<DebugRasterStep>? Steps)
    {
        public static implicit operator DebugRasterResult(ImmutableArray<AlphaPixel> pixels) => new(pixels, null);
        public static DebugRasterResult FromSingle(AlphaPixel p, bool genDbg) =>
            new([p], genDbg?[new DebugRasterStep(0, $"Single point: ({p.Pos.X} {p.Pos.Y}) a={p.Alpha:F2}")]:null);
    }

    public readonly record struct DebugRasterBuilder(ImmutableArray<AlphaPixel>.Builder Pixels, ImmutableArray<DebugRasterStep>.Builder? Steps)
    {
        public static DebugRasterBuilder GetBuilder(bool genDebug, int capPx = -1, int capSteps = -1)
        {
            Debug.Assert(genDebug || capSteps < 0, "capSteps should not be specified when genDebug is false.");
            Debug.Assert(capPx >= -1 && capSteps >= -1);
            var pxBld = capPx >= 0 ? ImmutableArray.CreateBuilder<AlphaPixel>(capPx) : ImmutableArray.CreateBuilder<AlphaPixel>();
            ImmutableArray<DebugRasterStep>.Builder? stepBld = null;
            if (genDebug) stepBld = capSteps >= 0 ? ImmutableArray.CreateBuilder<DebugRasterStep>(capSteps) : ImmutableArray.CreateBuilder<DebugRasterStep>();
            return new DebugRasterBuilder(pxBld, stepBld);
        }
        public DebugRasterResult ToResult() => new(Pixels.DrainToImmutable(), Steps?.DrainToImmutable());
    }

    public class FreqTimer
    {
        private bool _enabled;
        private double _freq;
        public bool Enabled
        {
            get => _enabled;
            set { if (!_enabled && value) Reset(); _enabled = value; }
        }
        public double Freq
        {
            get => _freq;
            set { if (value <= 0) throw new ArgumentException("Frequency must be positive"); _freq = value; }
        }
        public double ElapsedTime { get; private set; }
        public double Period => 1.0 / Freq;
        public FreqTimer(double freq, bool enabled = true)
        {
            Enabled = enabled;
            Freq = freq;
            ElapsedTime = 0.0;
        }
        public bool Update(double dt, out double elapsed)
        {
            elapsed = 0;
            if (!Enabled || dt <= 0) return false;
            ElapsedTime += dt;
            elapsed = ElapsedTime;
            bool updated = ElapsedTime >= Period;
            if (updated)
                ElapsedTime %= Period;
            return updated;
        }
        public bool Update(double dt) => Update(dt, out double _);
        public void Reset() => ElapsedTime = 0;
    }
}