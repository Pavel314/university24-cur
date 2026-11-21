using Imgui;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Numerics;
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading.Tasks;
using Imgui;
using static Sokol.SImgui;
using static Imgui.ImguiNative;
using t7_comp_graph.ImGuiExtensions;

//TODO[Done, Поворот - Canceled] передвижение фигур мышью, и поворот (использовать Transform)
//TODO[Done] у выбранной фигуры строить BoundBox но и пиксели выделять особым цветом
//TODO[Done] При нажатии на кнопку Remove - надо фокус переводить на другой элемент, для удобного массового удаления
//TODO[Done] Перейти к бесконечной сетке, отказаться от постоянной проверки contains, использовать камеру для увеличения
//TODO[Canceled] в статус баре выводить цвет под мышкой
//TODO[Done] в статус баре при создании фигуры выводить ее параметры
//TODO[Done] в RemoveShape удаление делать по индексу
//TODO[Done, Move - Canceled] Кнопка "Clone"(Copy) - отвечает за копирование фигуры, можно ещё up down - поднятие и опускание в списке shapes для управления порядком отрисовки
//TODO[Canceled] Для перетаскивания лучше тоже ввести специальную точку - перетаскивать по факту попадания в пиксели - зависимо от масштаба
//TODO[Done] в статус баре выводить информацию связанную с масштабом, может быть размер клетки? или их кол-во [UPD, Да размер клетки, от size отказаться]
//TODO[Canceled] автоматическое отключение сетки(перевести галочку) при мелком масштабе
//TODO[Done] Ограничить минимальный масштаб, иначе отрисовка примитива сможет израсходовать весь буфер
//TODO[Done]  Для удобного выделения фигуры можно ввести некий зазор в пикселях между позицией клика до ближайшего пикселя фигуры
//TODO[Done] Из класса Shape убрать Pen, он должен храниться в высокоуровневом представлении, как например TransformedShape

namespace t7_comp_graph
{
    public class VectorEditor
    {
        private class UiState
        {
            public bool SyncCells = true;
            public bool DrawGrid = true;
            public bool DrawAxes = true;
            public bool SnapToGrid = true;
            public bool KeepTool = false;
            public bool ShowAbout = false;
            public bool ShowDebug = false;

            public int? SelectedTrShape = null;
            public int SelectedCreator = 0;
            public int SelectedControlPoint = 0; //Note: use GetSelectedControlPoint for reading
            public bool EditorPenMode = false;
            public int SelectedDebugStep = 0;
            public FreqTimer DebugStepTimer = new FreqTimer(10, false);

            public Pen Pen = new Pen(new RgbaColor(0xFFFF55FF));
            public RgbaColor BgColor = new RgbaColor(0xAAAAAAFF);

            public int? _shapeToCopy = null;
            public int? _shapeToRemove = null;
            public TransformedShape? _shapeWithNewProps = null;
            public bool _needSync = true;

            public bool _debugWidgetRequestScroll;//Note: fully controlled by DrawDebugWidget
        }
        private class State
        {
            public Transform TranslateOrigin = Transform.Empty;
            public Shape? PreviewShape = null;
            public BoundBox PreviewBoundBox;

            public int? ActiveControlPoint = null;
            public Point ActiveControlPointOrigin;
        }
        public TransformedShape? SelectedTrShape => ui.SelectedTrShape.HasValue ? trShapes[ui.SelectedTrShape.Value] : null;
        public TransformedShape? SelectedTrEdShape => SelectedTrShape is { } trShape && trShape.Shape is EditableShape ? trShape : null;
        public ShapeCreator.Creator? SelectedCreator => ShapeCreator.Creators[ShapeCreator.Names[ui.SelectedCreator]];
        public bool IsPenMode => ui.EditorPenMode && SelectedTrEdShape.HasValue;
        public bool SnapIsAvail => ui.SnapToGrid; //TODO vs ui.DrawGrid && theme.GridTheme.HasLines && ui.SnapToGrid;

        private const float leftPanelW = 200f;
        private const float rightPanelW = 180f;
        private const int gridSensibleWidth = 10_000;
        private const float listBoxH = 121f;
        private const float minZoom = 0.25f;
        private const float maxZoom = 128.0f;//in px per cell
        private const float initZoom = 0.5f;
        private readonly Vector2 statusPad = new Vector2(8f, 4f);

        public Viewport Viewport { get; private set; }
        public InputSystem InputSystem { get; private set; }
        public Statistics Statistics { get; private set; }
        private List<TransformedShape> trShapes;
        private Theme theme;
        private Canvas canvas;
        private MouseInputHelper mouseHelper;
        private UiState ui;
        private State st;

        private int? GetSelectedControlPoint()
        {
            if (SelectedTrShape is { } selTrShape)
                return ui.SelectedControlPoint = Math.Clamp(ui.SelectedControlPoint, 0, selTrShape.Shape.ControlPoints.Length - 1);
            return null;
        }
        private int? GetSelectedDebugStep()
        {
            if (ui.ShowDebug && SelectedTrShape is { } selTrShape && selTrShape.Shape.DebugSteps is { } steps && steps.Length>0)
                return ui.SelectedDebugStep = Math.Clamp(ui.SelectedDebugStep, 0, steps.Length - 1);
            return null;
        }

        private static int FindLastAnchorPoint(Shape shape)
        {
            for (int i = shape.ControlPoints.Length - 1; i >= 0; --i)
            {
                if (shape.GetControlPointType(i) == ControlPointType.Anchor)
                    return i;
            }
            return 0;
        }

        private bool TryToChangePenMode(bool enabled)
        {
            if (SelectedTrShape is { } trShape && trShape.Shape is EditableShape)
            {
                ui.EditorPenMode = enabled;
                ui._needSync = true;
                return true;
            }
            return false;
        }

        public bool TryToRemoveSelectedControlPoint()
        {
            if (SelectedTrEdShape is not { } trShape) return false;
            var edShape = (EditableShape)trShape.Shape;
            if (GetSelectedControlPoint() is not { } selControlPt) return false;
            if (edShape.CanRemoveControlPoint(selControlPt))
            {
                var selInd = ui.SelectedTrShape!.Value;
                trShapes[selInd] = trShape with { Shape = edShape.RemoveControlPoint(selControlPt) };
                ui.SelectedControlPoint = selControlPt > 0 ? selControlPt - 1 : 0;
                ui._needSync = true;
                return true;
            }
            return false;
        }

        private int? FindShapeByPos(Vector pos, double tolPx = 6)
        {
            for (int i = trShapes.Count - 1; i >= 0; --i)
            {
                var trShape = trShapes[i];
                if (trShape.Shape.HitTest(trShape.Tr.Unapply(pos), Viewport.ToWorld(tolPx)))
                    return i;
            }
            return null;
        }

        public int? FindControlPoint(TransformedShape trShape, Vector screenPos, float padPx = 3f)
        {
            var points = trShape.Shape.ControlPoints;
            for (int i = points.Length - 1; i >= 0; --i)
            {
                var pointThemeOpt = theme.ControlTheme.GetPointTheme(trShape.Shape.GetControlPointType(i));
                if (!pointThemeOpt.IsEnabled(out var pointTheme) || !pointTheme.IsVisible)
                    continue;
                Vector pt = Viewport.ToScreen(trShape.Tr.Apply(points[i].ToVec()));
                var hitBox = BoundBox.FromCenter(pointTheme.WidthPx, pointTheme.HeightPx, pt).Expand(padPx * 2f);
                if (hitBox.Contains(screenPos))
                    return i;
            }
            return null;
        }

        //private TransformedShape? RemoveShape(TransformedShape shape) => trShapes.Remove(shape) ? null : shape;

        private void ResetViewport()
        {
            Viewport.Cam.Tr = Transform.Empty;
            Viewport.Cam.Zoom = initZoom;
        }

        public VectorEditor(BoundBox screen)
        {
            ui = new UiState();
            st = new State();

            var camera = new Camera(new Transform((0, 0)), minZoom:minZoom, maxZoom:maxZoom);
            Viewport = new Viewport(camera, screen); 

            InputSystem = new InputSystem();
            Statistics = new Statistics();

            theme = Theme.CreateDefault();
            canvas = new Canvas();
            mouseHelper = new MouseInputHelper();
            
            trShapes = new List<TransformedShape>();

            ui.BgColor = theme.BgColor;
            ResetViewport();
        }

        private unsafe bool DrawUiNewShape()
        {
            bool changed = false;
            ImGuiExt.BeginGroup("New shape");
            if (igBeginListBox("##ListBox", new Vector2(-1f, 190)))
            {
                for (int i = 0; i < ShapeCreator.Names.Length; i++)
                {
                    bool isSelected = ui.SelectedCreator == i;
                    if (igSelectable_Bool(ShapeCreator.Names[i], isSelected, ImGuiSelectableFlags.None, Vector2.Zero))
                    {
                        ui.SelectedCreator = i;
                        changed = true;
                    }
                }
                igEndListBox();
            }
            ImGuiExt.EndGroup();
            return changed;
        }

        private unsafe bool DrawUiCreatedShapes()
        {
            bool changed = false;
            ImGuiExt.BeginGroup("Created shapes");
            if (igBeginListBox("##ListBox", new Vector2(-1f, listBoxH)))
            {
                var drawList = igGetWindowDrawList();
                for (int i = 0; i < trShapes.Count; i++)
                {
                    var trShape = trShapes[i];
                    bool isSelected = ui.SelectedTrShape == i;
                    Vector2 cursorPos = igGetCursorPosExt();
                    if (igSelectable_Bool($"##Shape_{i}", isSelected, ImGuiSelectableFlags.None, Vector2.Zero))
                    {
                        ui.SelectedTrShape = i;
                        changed = true;
                    }
                    igSetCursorPos(cursorPos);
                    ImGuiExt.TextWithMarker(drawList, $"{trShape.Shape.ShowName} ({trShape.Shape.Pixels.Length} px)", trShape.Pen.Color);
                }
                igEndListBox();
            }
            if (igButton("Copy", new Vector(-1, 0))){
                ui._shapeToCopy = ui.SelectedTrShape;
                changed = true;
            }
            if (igButton("Remove", new Vector(-1, 0)))
            {
                ui._shapeToRemove = ui.SelectedTrShape;
                changed = true;
            }
            ImGuiExt.EndGroup();
            return changed;
        }

        private bool DrawUiOptions()
        {
            bool changed = false;
            ImGuiExt.BeginGroup("Options");
            changed |= igCheckbox("Draw grid/cell pad", ref ui.DrawGrid);
            changed |= igCheckbox("Draw axes", ref ui.DrawAxes);
            changed |= igCheckbox("Snap to grid", ref ui.SnapToGrid);
            changed |= igCheckbox("Keep tool", ref ui.KeepTool);
            changed |= igCheckbox("Show about", ref ui.ShowAbout);
            ImGuiExt.EndGroup();
            return changed;
        }

        private unsafe void DrawUiStatistics()
        {
            var totalPixels = 0;
            for (int i = 0; i < trShapes.Count; ++i)
                totalPixels += trShapes[i].Shape.Pixels.Length;

            var stats = Statistics;
            ImGuiExt.BeginGroup("Statistics");
            ImGuiExt.Text($"FPS: {stats.Fps:F2}");
            ImGuiExt.Text($"Pixels: {totalPixels}");
            ImGuiExt.Text($"Verts: {stats.VertsCount}/{stats.MaxVerts}");
            ImGuiExt.Text($"Inds: {stats.IdxCount} / {stats.MaxIdx}");
            ImGuiExt.EndGroup();

        }

        private bool DrawUiRightPanel(ref BoundBox screenBox)
        {
            bool changed = false;
            BoundBox rightBox = screenBox.Expand(-(screenBox.Width - rightPanelW), 0, 0, 0);
            if (ImGuiExt.BeginStaticPanel("Actions", rightBox, allowCollapse: true))
            {
                screenBox.MaxX = rightBox.MinX;
                changed |= DrawPenWidget(ref ui.Pen);
                changed |= DrawColorWidget("Background color", ref ui.BgColor);
                changed |= DrawUiNewShape();
                changed |= DrawUiCreatedShapes();
                changed |= DrawUiOptions();
                DrawUiStatistics();
            }
            ImGuiExt.EndPanel();
            return changed;
        }

        private static bool DrawColorWidget(string text, ref RgbaColor clr)
        {
            bool changed = false;
            var vclr = clr.ToVector3();
            ImGuiExt.BeginGroup(text);
            changed |= igColorEdit3("##Color", ref vclr, ImGuiColorEditFlags.None);
            ImGuiExt.EndGroup();
            if (changed) clr = RgbaColor.FromVector3(vclr, clr.A);
            return changed;
        }

        private static bool DrawPenWidget(ref Pen pen)
        {
            bool changed = false;
            var clr = pen.Color;
            changed |= DrawColorWidget("Pen", ref clr);
            if (changed) pen = pen with { Color = clr};
            return changed;
        }

        private static bool DrawTransformWidget(ref Transform tr)
        {
            var (pos, ang) = (Point.FromRound(tr.Pos), (int)Math.Round(double.RadiansToDegrees(tr.Ang)));
            ImGuiExt.BeginGroup("Transform");
            bool posChanged = ImGuiExt.InputPointV("Pos", ref pos, -gridSensibleWidth / 2, gridSensibleWidth / 2);
            bool angChanged = ImGuiExt.InputIntV("Angle", ref ang, 1, 10, 0, 360);
            ImGuiExt.EndGroup();
            if (posChanged) tr = tr.Replace(pos: pos.ToVec());
            if (angChanged) tr = tr.Replace(ang: double.DegreesToRadians(ang));
            return posChanged | angChanged;
        }

        private static bool DrawControlPointsEditorWidget(ref Shape shape, ref int? selectedPoint, ref bool penMode)
        {
            static string PenTypeToString(ControlPointType type) => type switch
            {
                ControlPointType.Anchor => "Anchor",
                ControlPointType.Handle => "Handle",
                _ => "Unknown"
            };

            bool changed = false;
            var points = shape.ControlPoints;
            var edShape = shape as EditableShape;

            ImGuiExt.BeginGroup("Editor", showText: false);
            ImGuiExt.Text($"Points: {points.Length}");
            igTextDisabled($"Segments: {shape.SegmentCount}");
            if (igBeginListBox("##EditorListBox", new Vector2(-1f, edShape is null ? 80f : 155f)))
            {
                for (int i = 0; i < points.Length; i++)
                {
                    var pointType = shape.GetControlPointType(i);
                    //if (pointType != ControlPointType.Anchor) continue;
                    var pointTypeStr = PenTypeToString(shape.GetControlPointType(i));
                    var isSel = selectedPoint == i;
                    var pointStr = $"{pointTypeStr} {i}: ({points[i].X}, {points[i].Y})###{i}";
                    if (igSelectable_Bool(pointStr, isSel, ImGuiSelectableFlags.None, Vector2.Zero))
                    {
                        selectedPoint = i;
                        changed = true;
                    }
                }
                igEndListBox();
            }

            if (selectedPoint is { } selPtInd)
            {
                var selPt = points[selPtInd];
                var pointType = PenTypeToString(shape.GetControlPointType(selPtInd));
                if (ImGuiExt.InputPointV($"Editing {pointType}: {selPtInd}", ref selPt, -gridSensibleWidth / 2, gridSensibleWidth / 2))
                {
                    shape = shape.WithControlPoint(selPtInd, selPt);
                    changed = true;
                }
                if (edShape is not null)
                {
                    igSpacing();
                    igSeparator();
                    changed |= igCheckbox("Pen Mode", ref penMode);
                    if (edShape.CanRemoveControlPoint(selPtInd) && igButton("Remove Selected Point", new Vector(-1f, 0f)))
                    {
                        shape = edShape.RemoveControlPoint(selPtInd);
                        selectedPoint = selPtInd > 0 ? selPtInd - 1 : 0;
                        changed = true;
                    }
                }
            }
            ImGuiExt.EndGroup();
            return changed;
        }

        private static unsafe bool DrawDebugWidget(ref Shape shape, ref int selStep, ref FreqTimer timer, ref bool _requstScroll)
        {
            static int Wrap(int a, int step, int max) => ((a + step) % max + max) % max;

            bool changed = false;
            ImGuiExt.BeginGroup("Debug", false);
            if (shape.DebugSteps is { } steps)
            {
                ImGuiExt.Text($"Steps({selStep}/{steps.Length-1})");
                if (igBeginChild_Str("##Listbox", new Vector2(-1, 170), ImGuiChildFlags.FrameStyle, ImGuiWindowFlags.HorizontalScrollbar))
                    {
                    for (int i = 0; i < steps.Length; i++)
                    {
                        if (igSelectable_Bool($"{steps[i].Desc}###Step_{i}", i<= selStep, ImGuiSelectableFlags.None, Vector2.Zero))
                        {
                            selStep = i;
                            changed = true;
                        }
                        if (selStep == i && _requstScroll)
                        {
                            igSetScrollHereY(0.5f);
                            _requstScroll = false;
                        }
                    }
                }
                igEndChild();
                igSeparator();

                var layout = new SameLineLayout(-1);
                var sz = new Vector2(26, 26);
                var srcSelStep = selStep;
                (bool Enabled, int Freq) tim = (timer.Enabled, (int)timer.Freq);
                var srcTim = tim;

                if (ImGuiExt.SymbolButton("First", sz, SymbolButtonIcon.First)) selStep = 0;
                layout.Next();
                if (ImGuiExt.SymbolButton("Back", sz, SymbolButtonIcon.StepBack)) selStep = Wrap(selStep, -1, steps.Length);
                layout.Next();
                if (ImGuiExt.SymbolButton("Next", sz, SymbolButtonIcon.StepNext)) selStep = Wrap(selStep, 1, steps.Length);
                layout.Next();
                if (ImGuiExt.SymbolButton("Last", sz, SymbolButtonIcon.Last)) selStep = steps.Length - 1;
                layout.Next();
                if (ImGuiExt.SymbolButton("Play/Pause", sz, tim.Enabled ? SymbolButtonIcon.Pause : SymbolButtonIcon.Play))
                {
                    if (selStep == steps.Length-1) selStep = 0;
                    tim.Enabled ^= true;
                }

                float padY = (sz.Y - igGetFontSize()) * 0.5f;
                igPushStyleVar_Vec2(ImGuiStyleVar.FramePadding, new Vector2(igGetStyle()->FramePadding.X, padY));
                layout.NextFill();
                igSliderInt("###Speed", ref tim.Freq, 1, 50, "Speed: %d", ImGuiSliderFlags.AlwaysClamp);
                igPopStyleVar(1);

                layout.End();
                if (srcSelStep != selStep || srcTim != tim)
                {
                    timer.Freq = tim.Freq;
                    timer.Enabled = tim.Enabled;
                    changed = true;
                    _requstScroll = true;
                }
            }
            ImGuiExt.EndGroup();
            return changed;
        }

        private bool DrawUiShapeCommon(ref Transform tr, ref Pen pen)
        {
            bool changed = false;
            changed |= DrawPenWidget(ref pen);
            changed |= DrawTransformWidget(ref tr);
            return changed;
        }

        private bool DrawControlPointsEditor(ref Shape shape)
        {
            int? selControlPoint = GetSelectedControlPoint();
            bool changed = DrawControlPointsEditorWidget(ref shape, ref selControlPoint, ref ui.EditorPenMode);
            if (changed)
            {
                if (selControlPoint is { } selPt)
                    ui.SelectedControlPoint = selPt;
            }
            return changed;
        }

        private bool DrawUiLine(ref Shape shape)
        {
            var line = (Line)shape;
            bool changed = false;
            var (algorithm, start, end) = (line.Algorithm, line.Start, line.End);
            changed |= ImGuiExt.ComboBoxV("Algorithm", ref algorithm);
            changed |= ImGuiExt.InputPointV("Start", ref start, -gridSensibleWidth / 2, gridSensibleWidth / 2);
            changed |= ImGuiExt.InputPointV("End", ref end, -gridSensibleWidth / 2, gridSensibleWidth / 2);
            if (changed) shape = new Line(start, end, algorithm);
            return changed;
        }

        private bool DrawUiCircle(ref Shape shape)
        {
            var circle = (Circle)shape;
            bool changed = false;
            var (center, radius) = (circle.Center, circle.Radius);
            changed |= ImGuiExt.InputPointV("Center", ref center, -gridSensibleWidth / 2, gridSensibleWidth / 2);
            changed |= ImGuiExt.InputDoubleV("Radius", ref radius, step: 1, stepFast: 10, format: "%.1f", min: 1, max: gridSensibleWidth);
            if (changed)
            {
                var dir = (circle.EdgePoint - circle.Center).ToVec();
                var edge = Point.FromRound(center.ToVec() + dir.SafeNorm(new Vector(1, 0)) * radius);
                shape = new Circle(center, edge);
            }
            return changed;
        }


        private bool DrawUiLeftPanel(ref BoundBox screenBox, out BoundBox debugBox)
        {
            bool changed = false;
            var leftBox = BoundBox.FromXywh(screenBox.X, screenBox.Y, leftPanelW, 0);
            debugBox = BoundBox.FromXywh(leftBox.X + leftBox.Width, leftBox.Y, 350, 260);
            if (!(SelectedTrShape is { } selTrShape)) return false;
            var (tr, pen, shape) = (selTrShape.Tr, selTrShape.Pen, selTrShape.Shape);

            if (ImGuiExt.BeginPanel("Properties", ref Unsafe.NullRef<bool>(), leftBox, ImGuiCond.Always, ImGuiWindowFlags.NoSavedSettings | ImGuiWindowFlags.NoResize | ImGuiWindowFlags.NoMove))
            {
                ImGuiExt.BeginGroup("Title", showText: false);
                ImGuiExt.Text($"{shape.ShowName}");
                ImGuiExt.EndGroup();
                changed |= DrawUiShapeCommon(ref tr, ref pen);
                ImGuiExt.BeginGroup("Main", showText: false);
                changed |= shape switch
                {
                    Line => DrawUiLine(ref shape),
                    Circle => DrawUiCircle(ref shape),
                    _ => DrawControlPointsEditor(ref shape)
                };
                if (igButton("Generate Debug", new Vector2(-1f, 0f)))
                {
                    shape = shape.WithDebugSteps();
                    ui.SelectedDebugStep = 0;
                    ui.DebugStepTimer.Reset();
                    ui.ShowDebug = true;
                    ui._debugWidgetRequestScroll = true;
                    changed = true;
                }
                ImGuiExt.EndGroup();
                if (changed) ui._shapeWithNewProps = new TransformedShape(tr, pen, shape);
                
            }
            ImGuiExt.EndPanel();
            return changed;
        }

        private bool DrawUiDebugPanel(BoundBox debugBox)
        {
            if (!ui.ShowDebug || (!(GetSelectedDebugStep() is { } selStep))) return false;
            Debug.Assert(SelectedTrShape.HasValue);
            var shape = SelectedTrShape.Value.Shape;
            bool changed = false;
            if (ImGuiExt.BeginPanel("Debug", ref ui.ShowDebug, debugBox, ImGuiCond.Once, ImGuiWindowFlags.NoSavedSettings | ImGuiWindowFlags.NoResize))
            {
                if (DrawDebugWidget(ref shape, ref selStep, ref ui.DebugStepTimer, ref ui._debugWidgetRequestScroll))
                    ui.SelectedDebugStep = selStep;
            }
            ImGuiExt.EndPanel();
            return changed;
        }

        private bool DrawUiStatusBar(ref BoundBox screenBox)
        {
            var statusH = igGetTextLineHeight() + statusPad.Y * 2f;
            BoundBox statusBox = screenBox.Expand(0, 0, 0, -(screenBox.Height - statusH));
            if (ImGuiExt.BeginMinimalisticPanel("StatusBar", statusBox, padding: statusPad))
            {
                screenBox.MaxY = statusBox.MinY;

                var pos = Viewport.ToWorld(InputSystem.GetMouse().Pos);
                var shape = st.PreviewShape ?? (SelectedTrShape is { } selTrShape ? selTrShape.Shape : null);

                string posStr = $"Pos: {pos.X,5:F1}, {pos.Y,5:F1}";
                string zoomStr = $"Cell: {Viewport.Cam.Zoom:F2} px";
                string? shapeStr = shape is not null ? shape.Summary : null;
                ImGuiExt.DelimText(" | ", posStr, zoomStr, shapeStr);
            }
            ImGuiExt.EndPanel();
            return false;
        }

        private bool DrawUiAbout(BoundBox screenBox)
        {
            if (!ui.ShowAbout) return false;
            var pos = new Vector(screenBox.MaxX, screenBox.MaxY);
            var pivot = new Vector(1, 1);
            ImGuiExt.TextPanel("About", 
                """
                Discrete Vector Editor v0.1
                Created by Pavel314
                github.com/Pavel314
                2026.11.17
                """, pos, RgbaColor.FromArgb(0), pivot);
            return false;
        }

        private void DrawUi(ref BoundBox screenBox, double dt)
        {
            ui._needSync |= DrawUiRightPanel(ref screenBox);
            ui._needSync |= DrawUiLeftPanel(ref screenBox, out var debugBox);
            ui._needSync |= DrawUiDebugPanel(debugBox);
            ui._needSync |= DrawUiStatusBar(ref screenBox);
            ui._needSync |= DrawUiAbout(screenBox);
        }

        private void SyncUi(BoundBox screenBox)
        {
            if (!ui._needSync) return;
            Viewport.Resize(screenBox);

            theme = theme with
            {
                BgColor = ui.BgColor,
                GridTheme = theme.GridTheme with
                {
                    LineTheme = theme.GridTheme.LineTheme with { Enabled = ui.DrawGrid },
                    AxisTheme = theme.GridTheme.AxisTheme with { Enabled = ui.DrawAxes },
                }
            };

            if (ui._shapeToCopy is { } shapeToCopy)
            {
                var insInd = shapeToCopy + 1;
                trShapes.Insert(insInd, trShapes[shapeToCopy]);
                ui.SelectedTrShape = insInd;
                ui._shapeToCopy = null;
                Debug.Assert(!ui._shapeToRemove.HasValue);
            }

            if (ui._shapeToRemove is { } shapeToRemove)
            {
                trShapes.RemoveAt(shapeToRemove);
                ui._shapeToRemove = null;
                ui.SelectedTrShape = trShapes.Count > 0 ? Math.Clamp(shapeToRemove, 0, trShapes.Count - 1) : null;
            }

            if (ui._shapeWithNewProps is { } newShape)
            {
                Debug.Assert(ui.SelectedTrShape.HasValue);
                var selInd = ui.SelectedTrShape.Value;
                trShapes[selInd] = newShape;
                ui._shapeWithNewProps = null;
            }

            ui._needSync = false;
        }

        private void AcceptInputKeyboard(double dt)
        {
            if (InputSystem.GetKey("R").IsPressed)
                ResetViewport();

            var ctrl = InputSystem.GetKey("LEFT_CONTROL");
            if (ctrl.IsPressed)
                TryToChangePenMode(true);
            if (ctrl.IsReleased)
                TryToChangePenMode(false);

            if (InputSystem.GetKey("DELETE").IsPressed)
            {
                if (!TryToRemoveSelectedControlPoint() && ui.SelectedTrShape is { } selShape)
                {
                    ui._shapeToRemove = selShape;
                    ui._needSync = true;
                }
            }
        }

        private void AcceptRightMouseInput(MouseButtonState rightBtn, double dt)
        {
            var mouse = InputSystem.GetMouse();
            if (mouse.Wheel.Y != 0)
            {
                Viewport.Cam.ZoomAt(Viewport.ToWorld(mouse.Pos), Math.Pow(1.15, mouse.Wheel.Y));
            }
            if (rightBtn.IsDown && mouse.Delta != Vector.Zero)
            {
                Viewport.Cam.Pos -= Viewport.ToWorldDir(mouse.Delta);
            }
        }

        private void AcceptLeftMouseInput(MouseButtonState leftBtn, Vector screenPos, double dt)
        {
            static void SetControlCursor() => igSetMouseCursor(ImGuiMouseCursor.Hand);
            var pos = Viewport.ToWorld(screenPos);
            var creator = SelectedCreator;
            var selShape = SelectedTrShape;
            int? hovControlPoint = selShape.HasValue ? FindControlPoint(selShape.Value, screenPos) : null;
            if (creator is null && (st.ActiveControlPoint.HasValue || hovControlPoint.HasValue)) SetControlCursor();
            var cmd = mouseHelper.OnMouseInput(leftBtn, pos, creator is not null, selShape is not null, hovControlPoint is not null, IsPenMode);
            switch (cmd.Action)
            {
                case MouseActionType.None: break;
                case MouseActionType.SelectShape:
                    {
                        ui.SelectedTrShape = FindShapeByPos(pos);
                        if (ui.SelectedTrShape is int selInd)
                            st.TranslateOrigin = trShapes[selInd].Tr;
                        break;
                    }
                case MouseActionType.TranslateShape:
                    {
                        Debug.Assert(ui.SelectedTrShape.HasValue);
                        var selInd = ui.SelectedTrShape.Value;
                        var step = new Vector(Math.Round(cmd.DragDelta.X), Math.Round(cmd.DragDelta.Y));
                        trShapes[selInd] = trShapes[selInd] with { Tr = st.TranslateOrigin.Moved(step) };
                        break;
                    }

                case MouseActionType.CreatePreview:
                    {
                        var reg = CellRegion.FromVectors(pos - cmd.DragDelta, pos, CellRegion.FitMode.Shrink);
                        st.PreviewShape = creator!(reg);
                        st.PreviewBoundBox = SnapIsAvail ? reg.ToBoundBox() : BoundBox.FromUnsortMinMax(pos - cmd.DragDelta, pos);
                        break;
                    }
                case MouseActionType.CommitPreview:
                    if (st.PreviewShape is not null && st.PreviewShape.Pixels.Length > 0)
                    {
                        trShapes.Add(new TransformedShape(Transform.Empty, ui.Pen, st.PreviewShape));
                        ui.SelectedTrShape = trShapes.Count - 1;
                        ui.SelectedControlPoint = FindLastAnchorPoint(st.PreviewShape);
                        if (!ui.KeepTool)
                            ui.SelectedCreator = 0;
                    }
                    st.PreviewShape = null;
                    break;
                case MouseActionType.CancelPreview:
                    st.PreviewShape = null;
                    break;


                case MouseActionType.AddAndInitControlPoint:
                    {
                        Debug.Assert(ui.SelectedTrShape.HasValue);
                        var selInd = ui.SelectedTrShape.Value;
                        var trShape = trShapes[selInd];
                        var edShape = (EditableShape)trShape.Shape;
                        var pt = Point.FromRound(trShape.Tr.Unapply(pos));
                        var newShape = trShape with { Shape = edShape.AddControlPoint(pt) };
                        trShapes[selInd] = newShape;
                        hovControlPoint = FindLastAnchorPoint(newShape.Shape);
                        goto case MouseActionType.InitControlPoint;
                    }
                case MouseActionType.InitControlPoint:
                    {
                        Debug.Assert(SelectedTrShape.HasValue && hovControlPoint.HasValue);
                        var shape = SelectedTrShape.Value;
                        st.ActiveControlPoint = hovControlPoint.Value;
                        st.ActiveControlPointOrigin = shape.Shape.ControlPoints[hovControlPoint.Value];
                        ui.SelectedControlPoint = hovControlPoint.Value;
                        break;
                    }

                case MouseActionType.MoveControlPoint:
                    {
                        //if (ui.SelectedTrShape is int selInd && st.ActiveControlPoint is int activeIdx)
                        Debug.Assert(ui.SelectedTrShape.HasValue && st.ActiveControlPoint.HasValue);
                        var selInd = ui.SelectedTrShape.Value;
                        var shape = trShapes[selInd];
                        Point newPt = Point.FromRound(st.ActiveControlPointOrigin.ToVec() + shape.Tr.UnapplyDir(cmd.DragDelta));
                        trShapes[selInd] = shape with { Shape = shape.Shape.WithControlPoint(st.ActiveControlPoint.Value, newPt) };
                        break;
                    }
                case MouseActionType.ReleaseControlPoint:
                    {
                        st.ActiveControlPoint = null;
                        st.ActiveControlPointOrigin = default;
                        break;
                    }
            }
        }

        private unsafe void AcceptInput(double dt)
        {
            var mouse = InputSystem.GetMouse();
            var leftBtn = mouse[MouseButton.Left];
            var rightBtn = mouse[MouseButton.Right];
            AcceptInputKeyboard(dt);
            AcceptRightMouseInput(rightBtn, dt);
            AcceptLeftMouseInput(leftBtn, mouse.Pos, dt);
        }


        public unsafe void OnFrame(BoundBox screenBox, double dt)
        {
            DrawUi(ref screenBox, dt);
            if (!BoundBox.StrictEquals(Viewport.Screen, screenBox)) Viewport.Resize(screenBox);
            SyncUi(screenBox);
            AcceptInput(dt);

            ImDrawList* drawList = igGetBackgroundDrawList(null);
            canvas.BeginRenderer(drawList, Viewport);
            canvas.Clear(theme.BgColor);
            canvas.DrawGrid(theme.GridTheme);
            for (int i = 0; i < trShapes.Count; ++i)
            {
                var trShape = trShapes[i];
                if (i == ui.SelectedTrShape)
                {
                    var selTheme = theme.SelectedTheme;
                    var cellPad = theme.GridTheme.CellPad;
                    if (GetSelectedDebugStep() is { } dbgStep)
                    {
                        var steps = trShape.Shape.DebugSteps!.Value;
                        int prevPixels = dbgStep > 0 ? steps[dbgStep - 1].PixelCount : 0;
                        canvas.DrawShapeDebug(trShape, cellPad, prevPixels, steps[dbgStep].PixelCount, theme.DebugTheme);
                    } else 
                        canvas.DrawShape(trShape, cellPad);
                    if (selTheme.BoundBox.IsEnabled(out var boxTheme)) canvas.DrawBox(trShape.Bounds, boxTheme);
                }
                else
                    canvas.DrawShape(trShape, theme.GridTheme.CellPad);
            }
            if (st.PreviewShape is not null)
            {
                canvas.DrawShape(st.PreviewShape, ui.Pen, Transform.Empty, theme.GridTheme.CellPad);
                if (theme.PreviewTheme.BoundBox.IsEnabled(out var boxTheme))
                {
                    canvas.DrawBox(st.PreviewBoundBox, boxTheme);
                }
            }

            if (SelectedTrShape is { } selTrShape)
            {
                canvas.DrawControlPoints(selTrShape, theme.ControlTheme, GetSelectedControlPoint());
            }

            if (GetSelectedDebugStep() is { } stepInd)
            {
                if (ui.DebugStepTimer.Update(dt))
                {
                    var steps = SelectedTrShape!.Value.Shape.DebugSteps!.Value;
                    ui._debugWidgetRequestScroll = true;
                    var nextInd = stepInd + 1;
                    if (nextInd < steps.Length)
                        ui.SelectedDebugStep = nextInd;
                    else
                        ui.DebugStepTimer.Enabled = false;
                }
            }
            else
            {
                ui.DebugStepTimer.Enabled=false;
            }

            canvas.EndRenderer();
            InputSystem.OnFrame(dt); //This causes the reset. It's placed at the end so that the input is available in this function fully.
        }
    }
}
