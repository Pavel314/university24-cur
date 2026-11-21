using System;
using System.Collections.Generic;
using System.Collections.Immutable;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using t7_comp_graph;

namespace t7_comp_graph
{
    public enum LineAlgorithm { DDA, Bresen, Wu }
    public static partial class Rasterizers
    {
        public static DebugRasterResult Line(Point p1, Point p2, LineAlgorithm algorithm, bool genDebug)
        {
            return algorithm switch
            {
                LineAlgorithm.DDA => LineDDA(p1, p2, genDebug),
                LineAlgorithm.Bresen => LineBresenham(p1, p2, genDebug),
                LineAlgorithm.Wu => LineWu(p1, p2, genDebug),
                _ => throw new ArgumentOutOfRangeException(nameof(algorithm))
            };
        }

        public static void LineInPlace(DebugRasterBuilder raster, Point p1, Point p2, LineAlgorithm algorithm)
        {
            switch (algorithm)
            {
                case LineAlgorithm.DDA: LineDDAInPlace(raster, p1, p2); break;
                case LineAlgorithm.Bresen: LineBresenhamInPlace(raster, p1, p2); break;
                case LineAlgorithm.Wu: LineWuInPlace(raster, p1, p2); break;
                default: throw new ArgumentOutOfRangeException(nameof(algorithm));
            }
        }

        public static DebugRasterResult Line(ReadOnlySpan<Point> controlPoints, LineAlgorithm algorithm, bool genDebug)
        {
            Debug.Assert(controlPoints.Length == 2);
            return Line(controlPoints[0], controlPoints[1], algorithm, genDebug);
        }
    }

    public static partial class Rasterizers
    {
        public static void LineDDAInPlace(DebugRasterBuilder raster, Point p1, Point p2)
        {
            static AlphaPixel ToWrite(double x, double y) => ((int)Math.Floor(x), (int)Math.Floor(y), 1.0);
            static string fmt(AlphaPixel px, double x, double y) => $"x={x:F2}, y={y:F2} -> ({px.Pos.X}, {px.Pos.Y})";
            
            int length = Math.Max(Math.Abs(p2.X - p1.X), Math.Abs(p2.Y - p1.Y));
            double dx = p2.X - p1.X;
            double dy = p2.Y - p1.Y;
            var (xInc, yInc) = length > 0 ? (dx / length, dy / length) : (0, 0);
            double x = p1.X + 0.5;
            double y = p1.Y + 0.5;

            var (bld, dbg) = raster;
            dbg?.Add((bld.Count, $"xInc={xInc:F2}, yInc={yInc:F2}"));

            bld.Add(ToWrite(x,y));
            dbg?.Add((bld.Count, fmt(bld[^1], x, y)));
            for (int i = 0; i < length; i++)
            {
                x += xInc;
                y += yInc;
                bld.Add(ToWrite(x,y));
                dbg?.Add((bld.Count, fmt(bld[^1], x, y)));
            }
        }

        public static void LineBresenhamInPlace(DebugRasterBuilder raster, Point p1, Point p2)
        {
            static string fmt(AlphaPixel px, int e) => $"({px.Pos.X}, {px.Pos.Y}), e={e}";

            var (dx, dy) = (Math.Abs(p2.X - p1.X), Math.Abs(p2.Y - p1.Y));
            var (sx, sy) = (Math.Sign(p2.X - p1.X), Math.Sign(p2.Y - p1.Y));

            var swap = dy > dx;
            if (swap) (dx, dy) = (dy, dx);

            var (x, y) = (p1.X, p1.Y);
            var e = 2 * dy - dx;

            var (bld, dbg) = raster;
            dbg?.Add((bld.Count, $"dx={dx}, dy={dy}, dy>dx={swap}, init e={e}"));

            bld.Add((x, y, 1.0));
            dbg?.Add((bld.Count, fmt(bld[^1], e)));

            for (int i = 1; i <= dx; i++)
            {
                if (e >= 0)
                {
                    if (swap) x += sx; else y += sy;
                    e -= 2 * dx;
                }
                if (swap) y += sy; else x += sx;
                e += 2 * dy;
                bld.Add((x, y, 1.0));
                dbg?.Add((bld.Count, fmt(bld[^1], e)));
            }
        }


        public static void LineWuInPlace(DebugRasterBuilder raster, Point p1, Point p2)
        {
            static string Fmt1(double y, AlphaPixel a) => $"y={y:F2} ({a.Pos.X},{a.Pos.Y}) a={a.Alpha:F2}";
            static string Fmt2(double y, AlphaPixel a, AlphaPixel b) => $"y={y:F2} ({a.Pos.X},{a.Pos.Y}) ({b.Pos.X},{b.Pos.Y}) a={a.Alpha:F2} {b.Alpha:F2}";
            static Point Swap(Point p) => (p.Y, p.X);
            static AlphaPixel ToWrite(AlphaPixel p, bool swap) => swap ? new AlphaPixel(Swap(p.Pos), p.Alpha) : p;

            var (bld, dbg) = raster;
            if (p1 == p2)
            {
                bld.Add((p1, 1.0));
                dbg?.Add((bld.Count, Fmt1(p1.Y, bld[^1])));
                return;
            }

            var (dx, dy) = (Math.Abs(p2.X - p1.X), Math.Abs(p2.Y - p1.Y));

            bool swap = dy > dx;

            if (swap) (p1, p2) = (Swap(p1), Swap(p2));

            int sx = Math.Sign(p2.X - p1.X);
            int length = Math.Abs(p2.X - p1.X);
            double grad = (p2.Y - p1.Y) / (double)length;

            dbg?.Add((bld.Count, $"dx={dx}, dy={dy}, dy>dx={swap}, grad={grad:F3}"));

            bld.Add(ToWrite((p1, 1.0), swap));
            dbg?.Add((bld.Count, Fmt1(p1.Y, bld[^1])));

            double y = p1.Y + grad;
            for (int x = p1.X + sx; x != p2.X; x += sx)
            {
                int iy = (int)Math.Floor(y);
                double alpha = y - iy;
                var px1 = ToWrite((x, iy, 1.0 - alpha), swap);
                bld.Add(px1);
                if (alpha > 1e-6)
                {
                    var px2 = ToWrite((x, iy + 1, alpha), swap);
                    bld.Add(px2);
                    dbg?.Add((bld.Count, Fmt2(y, px1, px2)));
                }
                else
                    dbg?.Add((bld.Count, Fmt1(y, px1)));
                y += grad;
            }
            bld.Add(ToWrite((p2, 1.0), swap));
            dbg?.Add((bld.Count, Fmt1(y, bld[^1])));
        }

        public static DebugRasterResult LineDDA(Point p1, Point p2, bool genDebug)
        {
            int len = Math.Max(Math.Abs(p2.X - p1.X), Math.Abs(p2.Y - p1.Y));
            var raster = DebugRasterBuilder.GetBuilder(genDebug, len + 1);
            LineDDAInPlace(raster, p1, p2);
            return raster.ToResult();
        }

        public static DebugRasterResult LineBresenham(Point p1, Point p2, bool genDebug)
        {
            int len = Math.Max(Math.Abs(p2.X - p1.X), Math.Abs(p2.Y - p1.Y));
            var raster = DebugRasterBuilder.GetBuilder(genDebug, len + 1);
            LineBresenhamInPlace(raster, p1, p2);
            return raster.ToResult();
        }

        public static DebugRasterResult LineWu(Point p1, Point p2, bool genDebug)
        {
            int len = Math.Max(Math.Abs(p2.X - p1.X), Math.Abs(p2.Y - p1.Y));
            var raster = DebugRasterBuilder.GetBuilder(genDebug, len * 2);
            LineWuInPlace(raster, p1, p2);
            return raster.ToResult();
        }
    }
}