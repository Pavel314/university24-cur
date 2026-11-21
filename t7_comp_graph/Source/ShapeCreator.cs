using System.Collections.Immutable;

namespace t7_comp_graph
{
    public static class ShapeCreator
    {
        public delegate Shape Creator(CellRegion reg);

        public static IReadOnlyDictionary<string, Creator?> Creators { get; } = new Dictionary<string, Creator?>
        {
            ["None"] = null,
            [Line.GetShowName(LineAlgorithm.DDA)] = reg => CreateLine(reg, LineAlgorithm.DDA),
            [Line.GetShowName(LineAlgorithm.Bresen)] = reg => CreateLine(reg, LineAlgorithm.Bresen),
            [Line.GetShowName(LineAlgorithm.Wu)] = reg => CreateLine(reg, LineAlgorithm.Wu),
            [Circle.UiName] = CreateCircle,
            [Ellipse.UiName] = CreateEllipse,
            [Parabola.UiName] = CreateParabola,
            [Hyperbola.UiName] = CreateHyperbola,
            [BSplineCurve.UiName] = CreateBSpline,
            [BezierCurve.UiName] = CreateBezier,
            [HermiteCurve.UiName] = CreateHermite,
        };

        public static readonly string[] Names = [..Creators.Keys];

        public static Line CreateLine(CellRegion reg, LineAlgorithm algorithm) => new Line(reg.Start, reg.End, algorithm);

        public static Circle CreateCircle(CellRegion reg)
        {
            var r = Math.Min(reg.Width, reg.Height) / 2;
            return new Circle(reg.Center, reg.Center + (r, 0));
        }

        public static Ellipse CreateEllipse(CellRegion reg) => new Ellipse(reg.Center, reg.End);

        public static Parabola CreateParabola(CellRegion reg)
        {
            var vertex = new Point(reg.Center.X, reg.Start.Y);
            var end = reg.End;
            return new Parabola(vertex, end);
            //var start = new Point(reg.Start.X, reg.End.Y);
            //var control = new Point(reg.Center.X, reg.Start.Y);
            //var end = reg.End;
            //return new Parabola(start, control, end);
        }

        public static Hyperbola CreateHyperbola(CellRegion reg)
        {
            var vertex = reg.Center + (reg.SignWidth / 4, 0);
            return new Hyperbola(reg.Center, vertex, reg.End);
        }

        private static ImmutableArray<Point> CreateLineControlPoints(CellRegion reg) => [reg.Start, Point.RoundLerp(reg.Start, reg.End, 1.0 / 3.0), Point.RoundLerp(reg.Start, reg.End, 2.0 / 3.0), reg.End];
        public static BSplineCurve CreateBSpline(CellRegion reg) => new(CreateLineControlPoints(reg));
        public static BezierCurve CreateBezier(CellRegion reg) => new(CreateLineControlPoints(reg));
        public static HermiteCurve CreateHermite(CellRegion reg) => new([reg.Start, Point.RoundLerp(reg.Start, reg.End, 0.25), reg.End, Point.RoundLerp(reg.Start, reg.End, 1.25)]);
    }
}
