using System;
using System.Collections.Generic;
using System.Collections.Immutable;
using System.Diagnostics;
using System.Diagnostics.Metrics;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using static Sokol.SG;
using static t7_comp_graph.Rasterizers;

namespace t7_comp_graph
{
    public static partial class Rasterizers
    {
        public static DebugRasterResult Circle(ReadOnlySpan<Point> controlPoints, bool genDebug)
        {
            Debug.Assert(controlPoints.Length == 2);
            var center = controlPoints[0];
            var edge = controlPoints[1];
            var r = (int)Math.Round((edge - center).Length());
            return Circle(center, r, genDebug);
        }

        public static DebugRasterResult Ellipse(ReadOnlySpan<Point> controlPoints, bool genDebug)
        {
            Debug.Assert(controlPoints.Length == 2);
            var (center, corner) = (controlPoints[0], controlPoints[1]);
            return Ellipse(center, Math.Abs(corner.X - center.X), Math.Abs(corner.Y - center.Y), genDebug);
        }

        public static DebugRasterResult Parabola(ReadOnlySpan<Point> controlPoints, bool genDebug)
        {
            Debug.Assert(controlPoints.Length == 2);
            return Parabola(controlPoints[0], controlPoints[1], genDebug);
        }

        public static DebugRasterResult Hyperbola(ReadOnlySpan<Point> controlPoints, bool genDebug)
        {
            Debug.Assert(controlPoints.Length == 3);
            return Hyperbola(controlPoints[0], controlPoints[1], controlPoints[2], genDebug);
        }
    }


    public struct ConicDifferenceStepper
    {
        public int X, Y;

        public long F, Dx, Dy;
        public readonly long Dxx, Dyy;
        public readonly CellRegion Region;

        public readonly int Sx => Math.Sign(Region.SignWidth);
        public readonly int Sy => Math.Sign(Region.SignHeight);

        public readonly long ErrX => Math.Abs(F + Dx);
        public readonly long ErrY => Math.Abs(F + Dy);
        public readonly long ErrDiagonal => Math.Abs(F + Dx + Dy);

        public readonly bool IsFinished => (Region.End.X - X) * Sx <= 0 && (Region.End.Y - Y) * Sy <= 0;

        public string? DebugDesc;

        public ConicDifferenceStepper(CellRegion reg, ConicEquation eq, bool genDebug)
        {
            Region = reg;
            X = reg.Start.X;
            Y = reg.Start.Y;

            int sx = Math.Sign(reg.SignWidth);
            int sy = Math.Sign(reg.SignHeight);

            F = eq.Calc(X, Y);
            Dx = eq.A * (2L * X * sx + 1) + eq.D * sx;
            Dy = eq.C * (2L * Y * sy + 1) + eq.E * sy;
            Dxx = 2 * eq.A;
            Dyy = 2 * eq.C;

            DebugDesc = genDebug ? $"{eq}\nStart ({X}, {Y}) | F={F}" : null;
        }

        public void StepX() { F += Dx; Dx += Dxx; X += Sx; }
        public void StepY() { F += Dy; Dy += Dyy; Y += Sy; }
        public void StepDiagonal() { StepX(); StepY(); }

        public void Step()
        {
            var (errX, errY, errD) = (ErrX, ErrY, ErrDiagonal);
            string move;

            if (errX <= errD && errX <= errY) { StepX(); move = "X"; }
            else if (errD <= errY) { StepDiagonal(); move = "XY"; }
            else { StepY(); move = "Y"; }

            if (DebugDesc is not null)
                DebugDesc = $"({X,2}, {Y,2}) F={F,2} | (X={errX,2}, XY={errD,2}, Y={errY,2})->{move,2}";
        }

        private bool started = false;
        public readonly (Point Pos, string? Desc) Current => (new(X, Y), DebugDesc);
        public readonly ConicDifferenceStepper GetEnumerator() => this;
        public bool MoveNext()
        {
            if (!started) return started = true;
            if (IsFinished) return false;
            Step();
            return true;
        }
    }

    public readonly record struct ConicEquation(long A, long C, long D, long E, long K)
    {
        public long Calc(int x, int y) => A * x * x + C * y * y + D * x + E * y + K;

        public ConicEquation Translate(Point offset) => new(
             A: A,
             C: C,
             D: D - 2 * A * offset.X,
             E: E - 2 * C * offset.Y,
             K: K + A * offset.X * offset.X + C * offset.Y * offset.Y - D * offset.X - E * offset.Y
         );

        public override string ToString()
        {
            var sb = new StringBuilder();
            static void Append(StringBuilder sb, long coeff, string varName)
            {
                if (coeff == 0) return;
                if (sb.Length > 0) sb.Append(coeff > 0 ? " + " : " - ");
                else if (coeff < 0) sb.Append('-');
                var abs = Math.Abs(coeff);
                if (abs != 1 || varName.Length == 0) sb.Append(abs);
                sb.Append(varName);
            }

            Append(sb, A, "x^2");
            Append(sb, C, "y^2");
            Append(sb, D, "x");
            Append(sb, E, "y");
            Append(sb, K, "");

            if (sb.Length == 0)
                return "0 = 0";

            sb.Append(" = 0");
            return sb.ToString();
        }

        public static ConicEquation Circle(Point center, int r) => new ConicEquation(1, 1, 0, 0, -(long)r * r).Translate(center);
        public static ConicEquation Ellipse(Point center, int rx, int ry)
        {
            long rx2 = (long)rx * rx;
            long ry2 = (long)ry * ry;
            return new ConicEquation(ry2, rx2, 0, 0, -rx2 * ry2).Translate(center);
        }

        public static ConicEquation Parabola(Point vertex, Point end)
        {
            long dx = end.X - vertex.X;
            long dy = end.Y - vertex.Y;
            return new ConicEquation(dy, 0, 0, -dx * dx, 0).Translate(vertex);
        }

        public static ConicEquation HorizontalHyperbola(Point center, int a, Point end)
        {
            long dx = Math.Abs(end.X - center.X);
            long dy = Math.Abs(end.Y - center.Y);
            long a2 = (long)a * a;
            long dy2 = dy * dy;

            return new ConicEquation(dy2, a2 - dx * dx, 0, 0, -a2 * dy2).Translate(center);
        }

        public static ConicEquation VerticalHyperbola(Point center, int a, Point end)
        {
            long dx = Math.Abs(end.X - center.X);
            long dy = Math.Abs(end.Y - center.Y);
            long a2 = (long)a * a;
            long dx2 = dx * dx;

            return new ConicEquation(a2 - dy * dy, dx2, 0, 0, -a2 * dx2).Translate(center);
        }
    }

    public static partial class Rasterizers
    {
        private static DebugRasterResult RasterizeSymmetry2X(CellRegion reg, ConicEquation eq, int axisX, bool genDebug)
        {
            int maxSteps = reg.Width + reg.Height + 1;
            var raster = DebugRasterBuilder.GetBuilder(genDebug, maxSteps * 2);
            var (bld, dbg) = raster;
            foreach (var (p, desc) in new ConicDifferenceStepper(reg, eq, genDebug))
            {
                bld.Add((p, 1.0));
                if (p.X != axisX) bld.Add((p.MirrorX(axisX), 1.0));
                dbg?.Add((bld.Count, desc!));
            }
            return raster.ToResult();
        }

        private static DebugRasterResult RasterizeSymmetry4(CellRegion reg, ConicEquation eq, Point center, bool genDebug)
        {
            int maxSteps = reg.Width + reg.Height + 1;
            var raster = DebugRasterBuilder.GetBuilder(genDebug, maxSteps * 4);
            var (bld, dbg) = raster;
            foreach (var (p, desc) in new ConicDifferenceStepper(reg, eq, genDebug))
            {
                bld.Add((p, 1.0));
                if (p.X != center.X) bld.Add((p.MirrorX(center.X), 1.0));
                if (p.Y != center.Y) bld.Add((p.MirrorY(center.Y), 1.0));
                if (p.X != center.X && p.Y != center.Y) bld.Add((p.Mirror(center), 1.0));
                dbg?.Add((bld.Count, desc!));
            }
            return raster.ToResult();
        }

        public static DebugRasterResult Circle(Point center, int radius, bool genDebug) => radius <= 0
            ? DebugRasterResult.FromSingle((center, 1.0), genDebug)
            : RasterizeSymmetry4(new CellRegion((center.X + radius, center.Y), (center.X, center.Y + radius)), ConicEquation.Circle(center, radius), center, genDebug);

        public static DebugRasterResult Ellipse(Point center, int rx, int ry, bool genDebug) => rx <= 0 && ry <= 0
            ? DebugRasterResult.FromSingle((center, 1.0), genDebug)
            : RasterizeSymmetry4(new CellRegion((center.X + rx, center.Y), (center.X, center.Y + ry)), ConicEquation.Ellipse(center, rx, ry), center, genDebug);

        public static DebugRasterResult Parabola(Point vertex, Point end, bool genDebug) => vertex == end
            ? DebugRasterResult.FromSingle((vertex, 1.0), genDebug)
            : RasterizeSymmetry2X(new CellRegion(vertex, end), ConicEquation.Parabola(vertex, end), vertex.X, genDebug);


        public static DebugRasterResult HorizontalHyperbola(Point center, int a, Point end, bool genDebug)
        {
            int dx = Math.Abs(end.X - center.X);
            int dy = Math.Abs(end.Y - center.Y);
            if (a <= 0 || dx <= a || dy == 0) return DebugRasterResult.FromSingle((center, 1.0), genDebug);
            var start = new Point(center.X + Math.Sign(end.X - center.X) * a, center.Y);
            return RasterizeSymmetry4(new CellRegion(start, end), ConicEquation.HorizontalHyperbola(center, a, end), center, genDebug);
        }

        public static DebugRasterResult VerticalHyperbola(Point center, int a, Point end, bool genDebug)
        {
            int dx = Math.Abs(end.X - center.X);
            int dy = Math.Abs(end.Y - center.Y);
            if (a <= 0 || dy <= a || dx == 0) return DebugRasterResult.FromSingle((center, 1.0), genDebug);
            var start = new Point(center.X, center.Y + Math.Sign(end.Y - center.Y) * a);
            return RasterizeSymmetry4(new CellRegion(start, end), ConicEquation.VerticalHyperbola(center, a, end), center, genDebug);
        }

        public static DebugRasterResult Hyperbola(Point center, Point vertex, Point end, bool genDebug)
        {
            int dx = Math.Abs(vertex.X - center.X);
            int dy = Math.Abs(vertex.Y - center.Y);
            return dx >= dy? HorizontalHyperbola(center, dx, end, genDebug): VerticalHyperbola(center, dy, end, genDebug);
        }
    }
}