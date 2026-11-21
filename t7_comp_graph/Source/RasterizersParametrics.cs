using System.Collections.Immutable;
using System.Diagnostics;

namespace t7_comp_graph
{
    public readonly record struct Matrix4x2(Vector4 Col1, Vector4 Col2)
    {
        public static Matrix4x2 operator *(Matrix4 m, Matrix4x2 g) => new(m * g.Col1, m * g.Col2);
        public static Vector operator *(Vector4 t, Matrix4x2 g) => new(t * g.Col1, t * g.Col2);
    }

    public class ParametricRasterizer(bool genDbg)
    {
        private readonly HashSet<Point> pixels = [];
        private readonly ImmutableArray<AlphaPixel>.Builder bld = ImmutableArray.CreateBuilder<AlphaPixel>();
        private readonly ImmutableArray<DebugRasterStep>.Builder? dbg = genDbg ? ImmutableArray.CreateBuilder<DebugRasterStep>():null;

        public Point? StartPos { get; private set; } = null;
        public bool GenDbg { get; private set; } = genDbg;

        public void Gap() => StartPos = null;
        public void MoveTo(Point pt) => StartPos = pt;


        public ParametricRasterizer AddSegment(Func<double, Point> func, int steps, double start = 0, double end = 1)
        {
            Debug.Assert(func is not null && steps > 0);
            if (!StartPos.HasValue)
                StartPos = func(start);

            dbg?.Add((bld.Count, $"New segment: steps={steps}, start=({StartPos.Value.X} {StartPos.Value.Y})"));
            var scale = (end - start) / steps;

            for (int step = 1; step <= steps; ++step)
            {
                var t = step == steps ? end : start + scale * step;
                var pt = func(t);

                int startIdx = bld.Count;
                Rasterizers.LineInPlace(new DebugRasterBuilder(bld, null), StartPos.Value, pt, LineAlgorithm.Bresen);

                int writeIdx = startIdx;
                for (int readIdx = startIdx; readIdx < bld.Count; ++readIdx)
                {
                    if (pixels.Add(bld[readIdx].Pos))
                        bld[writeIdx++] = bld[readIdx];
                }
                bld.Count = writeIdx;

                if (bld.Count > startIdx)
                    dbg?.Add((bld.Count, $"t={t:F2} -> ({pt.X}, {pt.Y})"));

                StartPos = pt;
            }
            return this;
        }

        public ParametricRasterizer AddCubicSpline(Matrix4 basis, ReadOnlySpan<Point> controlPoints, Func<Point, Point, Point, Point, Matrix4x2> controlsToGeometry, int step)
        {
            for (int i = 0; i <= controlPoints.Length - 4; i += step)
            {
                var (p0, p1, p2, p3) = (controlPoints[i], controlPoints[i + 1], controlPoints[i + 2], controlPoints[i + 3]);
                var appLen = (p1-p0).Length() + (p2 - p1).Length() + (p3 - p2).Length();
                var steps = Math.Clamp((int)Math.Ceiling(appLen / 10.0), 12, 512);

                var g = controlsToGeometry(p0, p1, p2, p3);
                var mg = basis * g;
                AddSegment(t => Point.FromRound(new Vector4(t * t * t, t * t, t, 1.0) * mg), steps: steps);
            }
            return this;
        }
        public DebugRasterResult Build() => new DebugRasterResult(bld.ToImmutable(), dbg?.ToImmutable());
    }



    public static partial class Rasterizers
    {
        public static DebugRasterResult CubicSpline(
            Matrix4 basis,
            ReadOnlySpan<Point> controlPoints,
            Func<Point, Point, Point, Point, Matrix4x2> controlsToGeometry,
            int step, bool genDebug
            ) => new ParametricRasterizer(genDebug).AddCubicSpline(basis, controlPoints, controlsToGeometry, step).Build();

        public static readonly Matrix4 BSplineBasis = (1.0 / 6.0) * new Matrix4(
            -1, 3, -3, 1,
             3, -6, 3, 0,
            -3, 0, 3, 0,
             1, 4, 1, 0
        );
        public static readonly Matrix4 BezierBasis = new(
            -1, 3, -3, 1,
             3, -6, 3, 0,
            -3, 3, 0, 0,
             1, 0, 0, 0
        );
        public static readonly Matrix4 HermiteBasis = new(
             2, -2, 1, 1,
            -3, 3, -2, -1,
             0, 0, 1, 0,
             1, 0, 0, 0
        );

        public static bool BSplineCheckPointsCount(int count) => count >= 4;
        public static bool BezierCheckPointsCount(int count) => count >= 4 && (count - 1) % 3 == 0;
        public static bool HermiteCheckPointsCount(int count) => count >= 4 && count % 2 == 0;
        public static DebugRasterResult BSpline(ReadOnlySpan<Point> controlPoints, bool genDebug)
        {
            Debug.Assert(BSplineCheckPointsCount(controlPoints.Length));
            static Matrix4x2 FromControls(Point p0, Point p1, Point p2, Point p3) =>
                new(new Vector4(p0.X, p1.X, p2.X, p3.X), new Vector4(p0.Y, p1.Y, p2.Y, p3.Y));
            return CubicSpline(BSplineBasis, controlPoints, FromControls, 1, genDebug);
        }
        public static DebugRasterResult Bezier(ReadOnlySpan<Point> controlPoints, bool genDebug)
        {
            Debug.Assert(BezierCheckPointsCount(controlPoints.Length));
            static Matrix4x2 FromControls(Point p0, Point p1, Point p2, Point p3) =>
                new(new Vector4(p0.X, p1.X, p2.X, p3.X), new Vector4(p0.Y, p1.Y, p2.Y, p3.Y));
            return CubicSpline(BezierBasis, controlPoints, FromControls, 3, genDebug);
        }
        public static DebugRasterResult Hermite(ReadOnlySpan<Point> controlPoints, bool genDebug)
        {
            Debug.Assert(HermiteCheckPointsCount(controlPoints.Length));
            static Matrix4x2 FromControls(Point p1, Point h1, Point p2, Point h2) =>
                new(new Vector4(p1.X, p2.X, h1.X - p1.X, h2.X - p2.X), new Vector4(p1.Y, p2.Y, h1.Y - p1.Y, h2.Y - p2.Y));
            return CubicSpline(HermiteBasis, controlPoints, FromControls, 2, genDebug);
        }
    }
}


//public ParametricRasterizer AddSegment(Func<double, Point> func, int steps, double start = 0, double end = 1)
//{
//    Debug.Assert(func is not null && steps > 0);
//    if (!StartPos.HasValue)
//        StartPos = func(start);

//    dbg?.Add((bld.Count, $"New segment: steps={steps}, start=({StartPos.Value.X} {StartPos.Value.Y})"));
//    var scale = (end - start) / steps;

//    for (int step = 1; step <= steps; ++step)
//    {
//        var t = step == steps ? end : start + scale * step;
//        var pt = func(t);

//        int before = bld.Count;
//        Rasterizers.LineBresenhamInPlace(bld, null, StartPos.Value, pt, pixels);
//        if (bld.Count > before)
//            dbg?.Add((bld.Count, $"t={t:F2} -> ({pt.X}, {pt.Y})"));
//        StartPos = pt;
//    }
//    return this;
//}