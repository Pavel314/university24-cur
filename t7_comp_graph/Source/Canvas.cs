using Imgui;
using static Sokol.SImgui;
using static Imgui.ImguiNative;

namespace t7_comp_graph
{
    public unsafe class Canvas
    {
        public readonly unsafe struct Context(ImDrawList* DrawList, Viewport Viewport)
        {
            public readonly ImDrawList* DrawList = DrawList;
            public readonly Viewport Viewport = Viewport;
        }

        private Context ctx;

        public void BeginRenderer(ImDrawList* drawList, Viewport viewport)
        {
            ctx = new Context(drawList, viewport);
        }

        public void EndRenderer() { }

        public void DrawLine(Vector p1, Vector p2, StrokeTheme theme, bool screen = false)
        {
            if (!theme.IsVisible) return;
            (p1, p2) = screen ? (p1, p2) : (ctx.Viewport.ToScreen(p1), ctx.Viewport.ToScreen(p2));
            ImDrawList_AddLine(ctx.DrawList, p1, p2, theme.Color.ImGui, theme.Width);
        }

        public void DrawBox(BoundBox box, BoxTheme theme, bool screen = false)
        {
            if (!theme.IsVisible) return;
            box = screen ? box : ctx.Viewport.ToScreen(box);
            float rounding = (float)(Math.Min(box.Width, box.Height) * 0.5 * theme.Rounding);
            if (theme.FillIsVisible)
                ImDrawList_AddRectFilled(ctx.DrawList, box.Min, box.Max, theme.FillColor.ImGui, rounding, ImDrawFlags.None);
            if (theme.StrokeIsVisible)
                ImDrawList_AddRect(ctx.DrawList, box.Min, box.Max, theme.Stroke.Color.ImGui, rounding, ImDrawFlags.None, theme.Stroke.Width);
        }

        public void DrawBox(Vector center, SizedBoxTheme theme, bool screen = false)
        {
            if (!theme.IsVisible) return;
            if (!screen) center = ctx.Viewport.ToScreen(center);
            DrawBox(BoundBox.FromCenter(theme.WidthPx, theme.HeightPx, center), theme.Theme, screen:true);
        }

        public void Clear(RgbaColor color)
        {
            DrawBox(ctx.Viewport.Screen, BoxTheme.Filled(color), screen: true);
        }

        public void DrawGrid(GridTheme theme)
        {
            var world = ctx.Viewport.GetWorldBBox();
            static double lowBorder(double v) => Math.Round(v, MidpointRounding.ToPositiveInfinity) - 0.5;
            static double upBorder(double v) => Math.Round(v, MidpointRounding.ToNegativeInfinity) + 0.5;

            if (theme.LineTheme.IsEnabled(out var lineTheme) && lineTheme.IsVisible && ctx.Viewport.ToScreen(1.0) >= lineTheme.Width * 2.0)
            {
                var (startX, endX) = (lowBorder(world.MinX), upBorder(world.MaxX));
                for (double x = startX; x <= endX; x += 1.0)
                    DrawLine((x, world.MinY), (x, world.MaxY), lineTheme);

                var (startY, endY) = (lowBorder(world.MinY), upBorder(world.MaxY));
                for (double y = startY; y <= endY; y += 1.0)
                    DrawLine((world.MinX, y), (world.MaxX, y), lineTheme);
            }

            if (theme.AxisTheme.IsEnabled(out var axisTheme) && axisTheme.IsVisible)
            {
                DrawLine((0, world.MinY), (0, world.MaxY), axisTheme);
                DrawLine((world.MinX, 0), (world.MaxX, 0), axisTheme);
            }
        }

        public void FillCell(Point cell, RgbaColor color, double cellPad)
        {
            if (!color.IsVisible) return;
            var bbox = BoundBox.FromCenter(1, 1, (cell.X, cell.Y));
            bbox = bbox.Expand(-ctx.Viewport.ToWorld(cellPad));
            DrawBox(bbox, BoxTheme.Filled(color));
        }

        public void DrawCells(ReadOnlySpan<AlphaPixel> pixels , Pen pen, Transform tr, double cellPad)
        {
            if (!pen.IsVisible) return;
            for (int i = 0; i < pixels.Length; i++)
            {
                var (pos, alpha) = (pixels[i].Pos, pixels[i].Alpha);
                var pixel = new AlphaPixel(Point.FromRound(tr.Apply(pos.ToVec())), alpha);
                FillCell(pixel.Pos, pen.Apply(pixel), cellPad);
            }
        }
        public void DrawCellsGroups(ReadOnlySpan<AlphaPixel> pixels, Transform tr, double cellPad, params ReadOnlySpan<(Pen Pen, int Count)> groups)
        {
            int offset = 0;
            foreach (var (pen, count) in groups)
            {
                if (offset >= pixels.Length) break;
                int end = (int)Math.Clamp((long)offset + count, offset, pixels.Length);
                if (end > offset) DrawCells(pixels[offset..end], pen, tr, cellPad);
                offset = end;
            }
        }


        public void DrawShape(Shape shape, Pen pen, Transform tr, double cellPad)
        {
            DrawCells(shape.Pixels.AsSpan(), pen, tr, cellPad);
        }

        public void DrawShape(TransformedShape trShape, double cellPad)
        {
            DrawShape(trShape.Shape, trShape.Pen, trShape.Tr, cellPad);
        }

        public void DrawShapeDebug(TransformedShape trShape, double cellPad, int headStart, int headEnd, DebugTheme theme)
        {
            var pixels = trShape.Shape.Pixels.AsSpan();
            var pen = trShape.Pen;
            DrawCellsGroups(
                pixels, trShape.Tr, cellPad, 
                (pen, headStart), (pen with {Color=theme.HeadColor}, headEnd - headStart), ( pen with { Color=theme.TailColor}, int.MaxValue)
            );
        }

        public void DrawControlPoints(Shape shape, ControlPointsTheme theme, Transform? tr = null, int? selectedPoint = null)
        {
            var points = shape.ControlPoints;
            static Vector ApplyTr(Transform? tr, Vector v) => tr.HasValue ? tr.Value.Apply(v) : v;
            if (theme.LineTheme.IsEnabled(out var lineTheme) && lineTheme.IsVisible)
            {
                foreach (var line in shape.ControlLines)
                {
                    var (p1, p2) = (ApplyTr(tr, points[line.From].ToVec()), ApplyTr(tr, points[line.To].ToVec()));
                    DrawLine(p1, p2, lineTheme);
                }
            }
            for (int i = 0; i < points.Length; ++i)
            {
                var ptThemeOpt = theme.GetPointTheme(shape.GetControlPointType(i));
                var worldPt = ApplyTr(tr, points[i].ToVec());
                if (ptThemeOpt.IsEnabled(out var ptTheme))
                    DrawBox(worldPt, ptTheme);
                if (i==selectedPoint && theme.ActiveTheme.IsEnabled(out var activeTheme))
                    DrawBox(worldPt, activeTheme);
            }
        }

        public void DrawControlPoints(TransformedShape trShape, ControlPointsTheme theme, int? selectedPoint=null)
        {
            DrawControlPoints(trShape.Shape, theme, tr: trShape.Tr, selectedPoint);
        }
    }
}
