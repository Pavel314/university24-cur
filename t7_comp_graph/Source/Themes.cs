namespace t7_comp_graph
{
    public readonly record struct StrokeTheme(RgbaColor Color, float Width)
    {
        public bool IsVisible => Color.IsVisible && Width > 0f;
        public static StrokeTheme None => default;
        public static StrokeTheme Solid(RgbaColor color, float width = 1.0f) => new(color, width);
    }
    public readonly record struct BoxTheme(RgbaColor FillColor, StrokeTheme Stroke, float Rounding) //Rounding - corner rounding in [0, 1]
    {
        public bool FillIsVisible => FillColor.IsVisible;
        public bool StrokeIsVisible => Stroke.IsVisible;
        public bool IsVisible => FillIsVisible || StrokeIsVisible;
        public static BoxTheme None => default;
        public static BoxTheme Filled(RgbaColor fillColor, StrokeTheme stroke = default, float rounding = 0f) => new(fillColor, stroke, rounding);
        public static BoxTheme Outlined(StrokeTheme stroke, float rounding = 0f) => new(RgbaColor.None, stroke, rounding);
    }

    public readonly record struct GridTheme(Option<StrokeTheme> LineTheme, Option<StrokeTheme> AxisTheme)
    {
        public bool HasLines => LineTheme.Enabled && LineTheme.Value.IsVisible;
        public double CellPad => LineTheme.IsEnabled(out var line) && line.IsVisible ? line.Width * 0.5 : 0.0;
    }

    public readonly record struct SelectedShapeTheme(Option<BoxTheme> BoundBox)
    {

    }

    public readonly record struct PreviewTheme(Option<BoxTheme> BoundBox)
    {

    }

    public readonly record struct SizedBoxTheme(float WidthPx, float HeightPx, BoxTheme Theme)
    {
        public bool IsVisible => Theme.IsVisible && WidthPx > 0 && HeightPx > 0;
        public static SizedBoxTheme None => default;
        public SizedBoxTheme(float widthPx, BoxTheme theme, float? heightPx = null) : this(widthPx, heightPx ?? widthPx, theme) { }
        public static SizedBoxTheme Filled(float widthPx, RgbaColor fillColor, StrokeTheme stroke = default, float rounding = 0f, float? heightPx = null) => new(widthPx, BoxTheme.Filled(fillColor, stroke, rounding), heightPx);
        public static SizedBoxTheme Outlined(float widthPx, StrokeTheme stroke, float rounding = 0f, float? heightPx = null) => new(widthPx, BoxTheme.Outlined(stroke, rounding), heightPx);
        public static SizedBoxTheme FilledCircle(float diam, RgbaColor fillColor, StrokeTheme stroke = default) => new(diam, diam, BoxTheme.Filled(fillColor, stroke, 1));
    }

    public readonly record struct ControlPointsTheme(Option<SizedBoxTheme> AnchorTheme, Option<SizedBoxTheme> ActiveTheme, Option<SizedBoxTheme> HandleTheme, Option<StrokeTheme> LineTheme)
    {
        public Option<SizedBoxTheme> GetPointTheme(ControlPointType type) => type switch
        {
            ControlPointType.Anchor => AnchorTheme,
            ControlPointType.Handle => HandleTheme,
            _ => default
        };
    }

    public readonly record struct DebugTheme(RgbaColor HeadColor, RgbaColor TailColor)
    {
        public bool HasHead => HeadColor.IsVisible;
        public bool HasTail => TailColor.IsVisible;
        public bool IsVisible => HasHead || HasTail;
    }

    public readonly record struct Theme(RgbaColor BgColor, GridTheme GridTheme, RgbaColor CellTheme, PreviewTheme PreviewTheme, SelectedShapeTheme SelectedTheme, ControlPointsTheme ControlTheme, DebugTheme DebugTheme)
    {
        public static Theme CreateDefault() => new(
            BgColor: new RgbaColor(0x1F2129FF),// AA0000FF
            GridTheme: new GridTheme(new StrokeTheme(new RgbaColor(0x3D424DFF), 2.0f), new StrokeTheme(new RgbaColor(0x738099FF), 1)),
            CellTheme: new RgbaColor(0x00FF00FF),
            PreviewTheme: new PreviewTheme(BoxTheme.Outlined(StrokeTheme.Solid(new RgbaColor(0x88FFFFFF)))),
            SelectedTheme: new SelectedShapeTheme(BoxTheme.Outlined(StrokeTheme.Solid(new RgbaColor(0xFFAAAAFF)))),
            ControlTheme: new ControlPointsTheme(
                AnchorTheme: SizedBoxTheme.FilledCircle(9f, new RgbaColor(0x4FA3FFFF), StrokeTheme.Solid(new RgbaColor(0x1F2129FF), 1.5f)),
                ActiveTheme: SizedBoxTheme.Outlined(13f, StrokeTheme.Solid(new RgbaColor(0xEF4444FF), 2.0f)),
                HandleTheme: SizedBoxTheme.FilledCircle(9f, new RgbaColor(0xFFFFFFFF), StrokeTheme.Solid(new RgbaColor(0x4FA3FFFF), 1.5f)),
                LineTheme: StrokeTheme.Solid(new RgbaColor(0xFFFFFFAA), 1.5f)
            ),
            DebugTheme: new DebugTheme(HeadColor: new RgbaColor(0xFFB300FF), TailColor: new RgbaColor(0xFF000044))
        );
    }
}
