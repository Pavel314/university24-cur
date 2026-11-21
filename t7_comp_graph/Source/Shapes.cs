using System.Collections.Immutable;
using System.Diagnostics;

namespace t7_comp_graph {

    public enum ControlPointType{Anchor, Handle}
    public readonly record struct ControlLineInds(int From, int To);

    public interface IUiNamed
    {
        static abstract string UiName { get; }
    }

    public abstract record class Shape
    {
        protected static ImmutableArray<ControlLineInds> CreateChainLines(int coount)
        {
            if (coount <= 1) return [];
            var builder = ImmutableArray.CreateBuilder<ControlLineInds>(coount - 1);
            for (int i = 0; i < coount - 1; i++)
                builder.Add(new ControlLineInds(i, i + 1));
            return builder.MoveToImmutable();
        }

        protected static readonly ImmutableArray<ControlLineInds> ControlLinesChain2 = CreateChainLines(2);
        protected static readonly ImmutableArray<ControlLineInds> ControlLinesChain3 = CreateChainLines(3);

        public static BoundBox CalculateBounds(ReadOnlySpan<AlphaPixel> pixels)
        {
            var bounds = BoundBox.Empty();
            foreach (var p in pixels)
                bounds.Extend(new Vector(p.Pos.X, p.Pos.Y));
            return bounds.Expand(0.5); //TODO not sure
        }

        protected abstract DebugRasterResult GeneratePixels(ReadOnlySpan<Point> controlPoints, bool genDebug);
        protected abstract ImmutableArray<ControlLineInds> GenerateControlLines(ReadOnlySpan<Point> controlPoints);

        private void Init(ImmutableArray<Point> controlPoints, ImmutableArray<ControlLineInds> controlIndices, DebugRasterResult rasterResult)
        {
            var pixels = rasterResult.Pixels;
            Debug.Assert(pixels.Length > 0);
            Bounds = CalculateBounds(pixels.AsSpan());
            Pixels = pixels;
            DebugSteps = rasterResult.Steps;
            ControlPoints = controlPoints;
            ControlLines = controlIndices;
        }

        protected Shape(ImmutableArray<Point> controlPoints, ImmutableArray<ControlLineInds> controlIndices, DebugRasterResult rasterResult) => Init(controlPoints, controlIndices, rasterResult);
        protected Shape(ImmutableArray<Point> controlPoints) => Init(controlPoints, GenerateControlLines(controlPoints.AsSpan()), GeneratePixels(controlPoints.AsSpan(), false));

        protected TShape WithControlPointsImpl<TShape>(TShape original, ImmutableArray<Point> controlPoints) where TShape : Shape
        {
            var data = GeneratePixels(controlPoints.AsSpan(), false);
            Debug.Assert(data.Pixels.Length > 0);
            return original with
            {
                ControlPoints = controlPoints,
                ControlLines = GenerateControlLines(controlPoints.AsSpan()),
                Pixels = data.Pixels,
                DebugSteps = data.Steps,
                Bounds = CalculateBounds(data.Pixels.AsSpan())
            };
        }

        public abstract Shape WithControlPoints(ImmutableArray<Point> controlPoints);

        public Shape WithControlPoint(int ind, Point pt)
        {
            Debug.Assert(ind >= 0 && ind < ControlPoints.Length);
            if (ControlPoints[ind] == pt) return this;
            return WithControlPoints(ControlPoints.SetItem(ind, pt));
        }

        public Shape WithDebugSteps()
        {
            var data = GeneratePixels(ControlPoints.AsSpan(), true);
            Debug.Assert(data.Pixels.Length > 0);
            return this with
            {
                Pixels = data.Pixels,
                DebugSteps = data.Steps,
                Bounds = CalculateBounds(data.Pixels.AsSpan())
            };
        }

        protected abstract ControlPointType GetControlPointTypeImpl(int ind);
        public ControlPointType GetControlPointType(int ind)
        {
            Debug.Assert(ind >= 0 && ind < ControlPoints.Length);
            return GetControlPointTypeImpl(ind);
        }

        public BoundBox Bounds { get; private set; }
        public ImmutableArray<Point> ControlPoints { get; private set; }
        public ImmutableArray<ControlLineInds> ControlLines { get; private set; }
        public ImmutableArray<DebugRasterStep>? DebugSteps { get; private set; }
        public ImmutableArray<AlphaPixel> Pixels { get; private set; }

        public virtual bool HitTest(Vector pos, double tol)
        {
            if (!Bounds.Expand(tol).Contains(pos)) return false;
            foreach (var pixel in Pixels)
            {
                if (BoundBox.FromCenter(1, 1, (pixel.Pos.X, pixel.Pos.Y)).Expand(tol).Contains(pos))
                    return true;
            }
            return false;
        }

        public abstract int SegmentCount { get; }
        public abstract string ShowName { get; }
        public abstract string Summary { get; }
    }

    public record class Line : Shape
    {
        private static readonly ImmutableArray<ControlLineInds> lineInds = [];//ControlLinesChain2;
        private static DebugRasterResult GeneratePoints(ReadOnlySpan<Point> controlPoints, LineAlgorithm algorithm, bool genDebug=false) => Rasterizers.Line(controlPoints, algorithm, genDebug);
        protected override DebugRasterResult GeneratePixels(ReadOnlySpan<Point> controlPoints, bool genDebug) => GeneratePoints(controlPoints, Algorithm, genDebug);
        protected override ImmutableArray<ControlLineInds> GenerateControlLines(ReadOnlySpan<Point> controlPoints) => lineInds;

        public override Line WithControlPoints(ImmutableArray<Point> controlPoints) => WithControlPointsImpl(this, controlPoints);
        protected override ControlPointType GetControlPointTypeImpl(int ind) => ControlPointType.Anchor;

        public Line(Point start, Point end, LineAlgorithm algorithm) : base([start, end], lineInds, GeneratePoints([start, end], algorithm)) => Algorithm = algorithm;
        public LineAlgorithm Algorithm { get; }
        public Point Start => ControlPoints[0];
        public Point End => ControlPoints[1];
        public double Length => double.Hypot(End.X - Start.X, End.Y - Start.Y);

        public override int SegmentCount => 1;
        public static string GetShowName(LineAlgorithm algo) => $"{nameof(Line)} {EnumUtils<LineAlgorithm>.GetName(algo)}";
        public override string ShowName => GetShowName(Algorithm);
        public override string Summary => $"{ShowName}: start = ({Start.X}, {Start.Y}), end = ({End.X}, {End.Y}), len = {Length:F1}";
    }

    public record class Circle : Shape, IUiNamed
    {
        protected override DebugRasterResult GeneratePixels(ReadOnlySpan<Point> controlPoints, bool genDebug) => Rasterizers.Circle(controlPoints, genDebug);
        protected override ImmutableArray<ControlLineInds> GenerateControlLines(ReadOnlySpan<Point> controlPoints) => ControlLinesChain2;

        public override Circle WithControlPoints(ImmutableArray<Point> controlPoints) => WithControlPointsImpl(this, controlPoints);
        protected override ControlPointType GetControlPointTypeImpl(int ind) => ControlPointType.Anchor;

        public Circle(Point center, Point edgePoint) : base([center, edgePoint]) { }
        public Point Center => ControlPoints[0];
        public Point EdgePoint => ControlPoints[1];
        public double Radius => double.Hypot(EdgePoint.X - Center.X, EdgePoint.Y - Center.Y);

        public override int SegmentCount => 1;
        public static string UiName => nameof(Circle);
        public override string ShowName => UiName;
        public override string Summary => $"{ShowName}: center = ({Center.X}, {Center.Y}), r = {Radius:F1}";
    }


    public record class Ellipse : Shape, IUiNamed
    {
        protected override DebugRasterResult GeneratePixels(ReadOnlySpan<Point> controlPoints, bool genDebug) => Rasterizers.Ellipse(controlPoints, genDebug);
        protected override ImmutableArray<ControlLineInds> GenerateControlLines(ReadOnlySpan<Point> controlPoints) => ControlLinesChain2;

        public override Ellipse WithControlPoints(ImmutableArray<Point> controlPoints) => WithControlPointsImpl(this, controlPoints);
        protected override ControlPointType GetControlPointTypeImpl(int ind) => ind == 0 ? ControlPointType.Anchor : ControlPointType.Handle;

        public Ellipse(Point center, Point corner) : base([center, corner]) { }
        public Point Center => ControlPoints[0];
        public Point Corner => ControlPoints[1];
        public int Rx => Math.Abs(Corner.X - Center.X);
        public int Ry => Math.Abs(Corner.Y - Center.Y);

        public override int SegmentCount => 1;
        public static string UiName => nameof(Ellipse);
        public override string ShowName => UiName;
        public override string Summary => $"{ShowName}: center = ({Center.X}, {Center.Y}), rx = {Rx}, ry = {Ry}";
    }

    public record class Parabola : Shape, IUiNamed
    {
        protected override DebugRasterResult GeneratePixels(ReadOnlySpan<Point> controlPoints, bool genDebug) => Rasterizers.Parabola(controlPoints, genDebug);
        protected override ImmutableArray<ControlLineInds> GenerateControlLines(ReadOnlySpan<Point> controlPoints) => ControlLinesChain2;
        public override Parabola WithControlPoints(ImmutableArray<Point> controlPoints) => WithControlPointsImpl(this, controlPoints);
        protected override ControlPointType GetControlPointTypeImpl(int ind) => ControlPointType.Anchor;

        public Parabola(Point vertex, Point end) : base([vertex, end]) { }
        public Point Vertex => ControlPoints[0];
        public Point End => ControlPoints[1];

        public override int SegmentCount => 1;
        public static string UiName => nameof(Parabola);
        public override string ShowName => UiName;
        public override string Summary => $"{ShowName}: vertex = ({Vertex.X}, {Vertex.Y}), end = ({End.X}, {End.Y})";
    }

    public record class Hyperbola : Shape, IUiNamed
    {
        protected override DebugRasterResult GeneratePixels(ReadOnlySpan<Point> controlPoints, bool genDebug) => Rasterizers.Hyperbola(controlPoints, genDebug);
        protected override ImmutableArray<ControlLineInds> GenerateControlLines(ReadOnlySpan<Point> controlPoints) => ControlLinesChain3;
        public override Hyperbola WithControlPoints(ImmutableArray<Point> controlPoints) => WithControlPointsImpl(this, controlPoints);
        protected override ControlPointType GetControlPointTypeImpl(int ind) => ControlPointType.Anchor;

        public Hyperbola(Point center, Point vertex, Point branchPoint) : base([center, vertex, branchPoint]) { }
        public Point Center => ControlPoints[0];
        public Point Vertex => ControlPoints[1];
        public Point BranchPoint => ControlPoints[2];

        public override int SegmentCount => 2;
        public static string UiName => nameof(Hyperbola);
        public override string ShowName => UiName;
        public override string Summary => $"{ShowName}: center = ({Center.X}, {Center.Y}), vertex = ({Vertex.X}, {Vertex.Y}), branch = ({BranchPoint.X}, {BranchPoint.Y})";
    }

    public abstract record class EditableShape : Shape
    {
        public EditableShape(ImmutableArray<Point> controlPoints) : base(controlPoints) { }
        public abstract EditableShape AddControlPoint(Point point);
        protected abstract bool CanRemoveControlPointImpl(int index);
        public abstract EditableShape RemoveControlPoint(int index);
        public bool CanRemoveControlPoint(int index) => index >= 0 && index < ControlPoints.Length ? CanRemoveControlPointImpl(index) : false;
    }

    public record class BSplineCurve : EditableShape, IUiNamed
    {
        protected override DebugRasterResult GeneratePixels(ReadOnlySpan<Point> controlPoints, bool genDebug) => Rasterizers.BSpline(controlPoints, genDebug);
        protected override ImmutableArray<ControlLineInds> GenerateControlLines(ReadOnlySpan<Point> controlPoints) => CreateChainLines(controlPoints.Length);
        public override BSplineCurve WithControlPoints(ImmutableArray<Point> controlPoints) => WithControlPointsImpl(this, controlPoints);
        protected override ControlPointType GetControlPointTypeImpl(int ind) => ControlPointType.Anchor;
        public BSplineCurve(ImmutableArray<Point> controlPoints) : base(controlPoints) { }

        public override BSplineCurve AddControlPoint(Point point) => WithControlPoints(ControlPoints.Add(point));
        protected override bool CanRemoveControlPointImpl(int index) => Rasterizers.BSplineCheckPointsCount(ControlPoints.Length - 1);
        public override BSplineCurve RemoveControlPoint(int index) => WithControlPoints(ControlPoints.RemoveAt(index));

        public override int SegmentCount => ControlPoints.Length - 3;
        public static string UiName => "BSpline";
        public override string ShowName => UiName;
        public override string Summary => $"{ShowName}: points = {ControlPoints.Length}, segments = {SegmentCount}";
    }


    public record class BezierCurve : EditableShape, IUiNamed
    {
        protected override DebugRasterResult GeneratePixels(ReadOnlySpan<Point> controlPoints, bool genDebug) => Rasterizers.Bezier(controlPoints, genDebug);
        protected override ImmutableArray<ControlLineInds> GenerateControlLines(ReadOnlySpan<Point> controlPoints) => CreateChainLines(controlPoints.Length);
        public override BezierCurve WithControlPoints(ImmutableArray<Point> controlPoints) => WithControlPointsImpl(this, controlPoints);
        protected override ControlPointType GetControlPointTypeImpl(int ind) => ind % 3 == 0 ? ControlPointType.Anchor : ControlPointType.Handle;
        public BezierCurve(ImmutableArray<Point> controlPoints) : base(controlPoints) { }

        public BezierCurve AddControlPoint(Point handle1, Point handle2, Point anchor) => WithControlPoints(ControlPoints.AddRange(handle1, handle2, anchor));
        public override BezierCurve AddControlPoint(Point point) => AddControlPoint(Point.RoundLerp(ControlPoints[^1], point, 1.0 / 3.0), Point.RoundLerp(ControlPoints[^1], point, 2.0 / 3.0), point);
        protected override bool CanRemoveControlPointImpl(int index) => GetControlPointType(index) == ControlPointType.Anchor && Rasterizers.BezierCheckPointsCount(ControlPoints.Length - 3);
        public override BezierCurve RemoveControlPoint(int index)
        {
            if (index == 0)
                return WithControlPoints(ControlPoints.RemoveRange(0, 3));
            if (index == ControlPoints.Length - 1)
                return WithControlPoints(ControlPoints.RemoveRange(index - 2, 3));
            return WithControlPoints(ControlPoints.RemoveRange(index - 1, 3));
        }

        public override int SegmentCount => (ControlPoints.Length - 1) / 3;
        public static string UiName => "Bezier";
        public override string ShowName => UiName;
        public override string Summary => $"{ShowName}: points = {ControlPoints.Length}, segments = {SegmentCount}";
    }




    public record class HermiteCurve : EditableShape, IUiNamed
    {
        protected static ImmutableArray<ControlLineInds> CreatePairLines(int count)
        {
            if (count <= 1) return [];
            var builder = ImmutableArray.CreateBuilder<ControlLineInds>(count / 2);
            for (int i = 0; i < count - 1; i += 2)
                builder.Add(new ControlLineInds(i, i + 1));
            return builder.MoveToImmutable();
        }
        protected override DebugRasterResult GeneratePixels(ReadOnlySpan<Point> controlPoints, bool genDebug) => Rasterizers.Hermite(controlPoints, genDebug);
        protected override ImmutableArray<ControlLineInds> GenerateControlLines(ReadOnlySpan<Point> controlPoints) => CreatePairLines(controlPoints.Length);
        public override HermiteCurve WithControlPoints(ImmutableArray<Point> controlPoints) => WithControlPointsImpl(this, controlPoints);
        protected override ControlPointType GetControlPointTypeImpl(int ind) => ind % 2 == 0 ? ControlPointType.Anchor : ControlPointType.Handle;
        public HermiteCurve(ImmutableArray<Point> controlPoints) : base(controlPoints) { }

        public HermiteCurve AddControlPoint(Point anchor, Point handle) => WithControlPoints(ControlPoints.AddRange(anchor, handle));
        public override HermiteCurve AddControlPoint(Point point) => AddControlPoint(point, Point.RoundLerp(ControlPoints[^2], point, 1.25));
        protected override bool CanRemoveControlPointImpl(int index) => GetControlPointType(index) == ControlPointType.Anchor && Rasterizers.HermiteCheckPointsCount(ControlPoints.Length - 2);
        public override HermiteCurve RemoveControlPoint(int index) => WithControlPoints(ControlPoints.RemoveRange(index, 2));

        public override int SegmentCount => (ControlPoints.Length / 2) - 1;
        public static string UiName => "Hermite";
        public override string ShowName => UiName;
        public override string Summary => $"{ShowName}: points = {ControlPoints.Length / 2}, segments = {SegmentCount}";
    }
}
