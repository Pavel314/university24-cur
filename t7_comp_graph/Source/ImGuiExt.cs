using Imgui;
using System.Numerics;
using System.Runtime.CompilerServices;
using static Sokol.SImgui;
using static Imgui.ImguiNative;

namespace Imgui
{
    public static unsafe partial class ImguiNative
    {
        public static void igTextUnformatted(string text) => igTextUnformatted(text, null);
        public static bool igBegin(string name, ImGuiWindowFlags flags) => igBegin(name, ref Unsafe.NullRef<byte>(), flags);
        public static bool igBegin(string name, ref bool p_open, ImGuiWindowFlags flags)
        {
            if (Unsafe.IsNullRef(ref p_open))
                return igBegin(name, ref Unsafe.NullRef<byte>(), flags);
            byte p_openU8 = p_open ? (byte)1 : (byte)0;
            bool ret = igBegin(name, ref p_openU8, flags);
            p_open = p_openU8 != 0;
            return ret;
        }
        public static Vector2 igGetCursorScreenPos()
        {
            Vector2 pos = default;
            igGetCursorScreenPos(ref pos);
            return pos;
        }
        public static bool igCheckbox(string text, ref bool p_open)
        {
            byte p_openU8 = p_open ? (byte)1 : (byte)0;
            bool changed = igCheckbox(text, ref p_openU8);
            p_open = (p_openU8 != 0);
            return changed;
        }

        public static Vector2 igGetCursorPosExt()
        {
            Vector2 pos = default;
            igGetCursorPos(ref pos);
            return pos;
        }

        public static unsafe bool igInputDoubleExt(string label, ref double v, double step, double step_fast, string format, ImGuiInputTextFlags flags, double min = double.MinValue, double max = double.MaxValue)
        {
            fixed (double* pV = &v)
            {
                if (!igInputDouble(label, pV, step, step_fast, format, flags)) return false;
            }
            v = Math.Clamp(v, min, max);
            return true;
        }

        public static unsafe bool igInputIntExt(string label, ref int v, int step, int step_fast, ImGuiInputTextFlags flags = ImGuiInputTextFlags.None, int min = int.MinValue, int max = int.MaxValue)
        {
            if (!igInputInt(label, ref v, step, step_fast, flags)) return false;
            v = Math.Clamp(v, min, max);
            return true;
        }
    }
}

namespace t7_comp_graph.ImGuiExtensions
{
    public enum SymbolButtonIcon { Play, Pause, Stop, First, StepBack, StepNext, Last }

    public static unsafe class ImGuiExt
    {
        public static unsafe void TextWithMarker(ImDrawList* drawList, string text, RgbaColor color, float markerScale = 0.75f)
        {
            float textHeight = igGetTextLineHeight();
            float markerSize = textHeight * markerScale;
            Vector2 markerPos = igGetCursorScreenPos();
            markerPos.Y += (textHeight - markerSize) * 0.5f;
            ImDrawList_AddRectFilled(drawList, markerPos, markerPos + new Vector2(markerSize), color.ImGui, 2.0f, ImDrawFlags.None);
            igSetCursorPosX(igGetCursorPosX() + markerSize + 6.0f);
            igTextUnformatted(text);
        }

        public static bool Checkbox(string text, ref bool selected) => igCheckbox(text, ref selected);

        public static void Text(string text) => igTextUnformatted(text);

        public static void BeginGroup(string text, bool showText = true)
        {
            if (showText)
                igText(text);
            igPushID_Str(text);
        }
        public static void EndGroup()
        {
            igPopID();
            igSeparator();
        }

        public static bool SliderDoubleV(string label, ref double value, double min, double max, bool clamp = true, string format = "%.3f")
        {
            igTextUnformatted(label);
            igPushItemWidth(-1f);
            float fValue = (float)value;

            igPushID_Str(label);
            bool changed = igSliderFloat("###slider", ref fValue, (float)min, (float)max, format, clamp ? ImGuiSliderFlags.AlwaysClamp : ImGuiSliderFlags.None);
            igPopID();

            igPopItemWidth();
            if (changed) value = fValue;
            igSpacing();
            return changed;
        }

        public static bool SliderIntV(string label, ref int value, int min, int max, bool clamp = true)
        {
            igTextUnformatted(label);
            igPushItemWidth(-1f);

            igPushID_Str(label);
            bool changed = igSliderInt("###slider", ref value, min, max, "%d", clamp ? ImGuiSliderFlags.AlwaysClamp : ImGuiSliderFlags.None);
            igPopID();

            igPopItemWidth();
            igSpacing();
            return changed;
        }

        public static bool InputDoubleV(string label, ref double value, double step, double stepFast, string format, double min = double.MinValue, double max = double.MaxValue)
        {
            igTextUnformatted(label);
            igPushItemWidth(-1f);

            igPushID_Str(label);
            bool changed = igInputDoubleExt("###input", ref value, step, stepFast, format, ImGuiInputTextFlags.None, min, max);
            igPopID();

            igPopItemWidth();
            igSpacing();
            return changed;
        }

        public static bool InputIntV(string label, ref int value, int step, int stepFast, int min = int.MinValue, int max = int.MaxValue)
        {
            igTextUnformatted(label);
            igPushItemWidth(-1f);

            igPushID_Str(label);
            bool changed = igInputIntExt("###input", ref value, step, stepFast, ImGuiInputTextFlags.None, min, max);
            igPopID();

            igPopItemWidth();
            igSpacing();
            return changed;
        }

        public static bool InputPointV(string label, ref Point point, int min = int.MinValue, int max = int.MaxValue)
        {
            igTextUnformatted(label);
            bool changed = false;
            Vector2 avail = default;
            igGetContentRegionAvail(ref avail);
            var itemWidth = (avail.X - igGetStyle()->ItemSpacing.X) * 0.5f;
            var (x, y) = (point.X, point.Y);
            igSetNextItemWidth(itemWidth);
            changed |= igInputIntExt($"##{label}_X", ref x, 1, 10, ImGuiInputTextFlags.None, min, max);
            igSameLine(0f, -1f);
            igSetNextItemWidth(itemWidth);
            changed |= igInputIntExt($"##{label}_Y", ref y, 1, 10, ImGuiInputTextFlags.None, min, max);
            igSpacing();
            if (changed) point = new Point(x, y);
            return changed;
        }

        public static bool ComboBoxV(string label, string[] items, ref int selected)
        {
            bool changed = false;
            string currentItem = (selected >= 0 && selected < items.Length) ? items[selected] : "";

            igPushID_Str(label);
            igTextUnformatted(label);

            igPushItemWidth(-1f);
            if (igBeginCombo("##combo", currentItem, ImGuiComboFlags.None))
            {
                for (int i = 0; i < items.Length; i++)
                {
                    bool isSelected = (selected == i);
                    igPushID_Int(i);
                    if (igSelectable_Bool(items[i], isSelected, ImGuiSelectableFlags.None, Vector2.Zero))
                    {
                        selected = i;
                        changed = true;
                    }
                    igPopID();
                    if (isSelected) igSetItemDefaultFocus();
                }
                igEndCombo();
            }
            igPopItemWidth();
            igPopID();
            igSpacing();
            return changed;
        }

        private delegate bool UIWidgetDelegate(string text, string[] items, ref int selected);
        private static bool GlueEnum<TEnum>(string text, ref TEnum selected, UIWidgetDelegate widget) where TEnum : struct, Enum
        {
            int ind = EnumUtils<TEnum>.ToInt(selected);
            if (widget(text, EnumUtils<TEnum>.Names, ref ind))
            {
                selected = EnumUtils<TEnum>.FromInt(ind);
                return true;
            }
            return false;
        }

        public static bool ComboBoxV<TEnum>(string label, ref TEnum selected) where TEnum : struct, Enum =>
            GlueEnum(label, ref selected, ComboBoxV);

        public static bool BeginPanel(string name, ref bool p_open, BoundBox box, ImGuiCond cond, ImGuiWindowFlags flags, Vector2 pivot = default)
        {
            igSetNextWindowPos(box.Pos, cond, pivot);
            igSetNextWindowSize(box.Size, cond);
            return igBegin(name, ref p_open, flags);
        }
        public static bool BeginStaticPanel(string name, BoundBox box, bool allowCollapse) =>
            BeginPanel(name, ref Unsafe.NullRef<bool>(), box, ImGuiCond.Always, ImGuiWindowFlags.NoMove | ImGuiWindowFlags.NoResize | (allowCollapse ? 0 : ImGuiWindowFlags.NoCollapse));

        public static bool BeginMinimalisticPanel(string name, BoundBox box, Vector2? padding = null, float? rounding = null)
        {
            var flags = ImGuiWindowFlags.NoDecoration | ImGuiWindowFlags.NoMove | ImGuiWindowFlags.NoSavedSettings;
            if (padding.HasValue) igPushStyleVar_Vec2(ImGuiStyleVar.WindowPadding, padding.Value);
            if (rounding.HasValue) igPushStyleVar_Float(ImGuiStyleVar.WindowRounding, rounding.Value);
            var open = BeginPanel(name, ref Unsafe.NullRef<bool>(), box, ImGuiCond.Always, flags);
            int count = (padding.HasValue ? 1 : 0) + (rounding.HasValue ? 1 : 0);
            if (count > 0) igPopStyleVar(count);
            return open;
        }

        public static void TextPanel(string name, string text, Vector2 pos, RgbaColor bg, Vector2 pivot = default)
        {
            var flags = ImGuiWindowFlags.NoDecoration | ImGuiWindowFlags.NoInputs | ImGuiWindowFlags.AlwaysAutoResize | ImGuiWindowFlags.NoSavedSettings;

            igPushStyleColor_U32(ImGuiCol.WindowBg, bg.ImGui);
            igPushStyleVar_Float(ImGuiStyleVar.WindowBorderSize, 0f);
            if (BeginPanel(name, ref Unsafe.NullRef<bool>(), BoundBox.FromXywh(pos, Vector2.Zero), ImGuiCond.Always, flags, pivot))
                igTextDisabled(text);
            EndPanel();

            igPopStyleVar(1);
            igPopStyleColor(1);
        }


        public static void EndPanel() => igEnd();

        public static void DelimText(string delimiter, params ReadOnlySpan<string?> items)
        {
            bool isf = true;
            for (var i = 0; i < items.Length; ++i)
            {
                var item = items[i];
                if (string.IsNullOrEmpty(item)) continue;
                if (!isf)
                {
                    igSameLine(0f, 0f);
                    igTextDisabled(delimiter);
                    igSameLine(0f, 0f);
                }
                igTextUnformatted(item);
                isf = false;
            }
        }

        private static unsafe void DrawSymbol(SymbolButtonIcon symbol, BoundBox box, ImDrawList* drawList)
        {
            double cx = box.Center.X;
            Vector Flip(double cx, Vector v, bool flip) => flip ? new Vector(2.0 * cx - v.X, v.Y) : v;
            BoundBox FlipBox(double cx, BoundBox b, bool flip) => flip ? BoundBox.FromMinMax(2.0 * cx - b.MaxX, b.MinY, 2.0 * cx - b.MinX, b.MaxY) : b;

            switch (symbol)
            {
                case SymbolButtonIcon.Play:
                    {
                        var col = new RgbaColor(65, 200, 85, 255);
                        var inner = box.Expand(-box.Width * 0.32, -box.Height * 0.25);
                        var a = inner.Min;
                        var b = new Vector(inner.MaxX + 2.0, inner.Center.Y);
                        var c = new Vector(inner.MinX, inner.MaxY);
                        ImDrawList_AddTriangleFilled(drawList, a, b, c, col.ImGui);
                        break;
                    }

                case SymbolButtonIcon.Pause:
                    {
                        var col = new RgbaColor(90, 185, 255, 255);
                        var inner = box.Expand(-box.Width * 0.30, -box.Height * 0.26);
                        var barW = box.Width * 0.14;
                        var leftBar = BoundBox.FromXywh(inner.MinX, inner.MinY, barW, inner.Height);
                        var rightBar = BoundBox.FromXywh(inner.MaxX - barW, inner.MinY, barW, inner.Height);
                        ImDrawList_AddRectFilled(drawList, leftBar.Min, leftBar.Max, col.ImGui, 1.0f, ImDrawFlags.None);
                        ImDrawList_AddRectFilled(drawList, rightBar.Min, rightBar.Max, col.ImGui, 1.0f, ImDrawFlags.None);
                        break;
                    }

                case SymbolButtonIcon.Stop:
                    {
                        var col = new RgbaColor(235, 75, 75, 255);
                        var inner = box.Expand(-box.Width * 0.29, -box.Height * 0.29);
                        ImDrawList_AddRectFilled(drawList, inner.Min, inner.Max, col.ImGui, 1.5f, ImDrawFlags.None);
                        break;
                    }
                case SymbolButtonIcon.StepNext:
                case SymbolButtonIcon.StepBack:
                    {
                        bool flip = symbol == SymbolButtonIcon.StepBack;
                        var col = new RgbaColor(210, 220, 230, 255);
                        var side = Math.Min(box.Width, box.Height);
                        var inner = BoundBox.FromCenter(side, side, box.Center).Expand(-side * 0.35, -side * 0.31);

                        var a = Flip(cx, inner.Min, flip);
                        var b = Flip(cx, new Vector(inner.MaxX, inner.Center.Y), flip);
                        var c = Flip(cx, new Vector(inner.MinX, inner.MaxY), flip);
                        ImDrawList_AddTriangleFilled(drawList, a, b, c, col.ImGui);
                        break;
                    }

                case SymbolButtonIcon.Last:
                case SymbolButtonIcon.First:
                    {
                        bool flip = symbol == SymbolButtonIcon.First;
                        var col = new RgbaColor(210, 220, 230, 255);
                        var side = Math.Min(box.Width, box.Height);
                        var inner = BoundBox.FromCenter(side, side, box.Center).Expand(-side * 0.32, -side * 0.31);
                        var barW = side * 0.08;

                        var bar = FlipBox(cx, BoundBox.FromXywh(inner.MaxX - barW, inner.MinY, barW, inner.Height), flip);
                        ImDrawList_AddRectFilled(drawList, bar.Min, bar.Max, col.ImGui, 0.5f, ImDrawFlags.None);

                        var triRight = inner.MaxX - barW - side * 0.03;
                        var a = Flip(cx, inner.Min, flip);
                        var b = Flip(cx, new Vector(triRight, inner.Center.Y), flip);
                        var c = Flip(cx, new Vector(inner.MinX, inner.MaxY), flip);
                        ImDrawList_AddTriangleFilled(drawList, a, b, c, col.ImGui);
                        break;
                    }
            }
        }




        public static bool SymbolButton(string id, Vector2 size, SymbolButtonIcon symbol)
        {
            ImDrawList* drawList = igGetWindowDrawList();
            var clicked = igInvisibleButton(id, size, ImGuiButtonFlags.None);
            uint bgCol;
            if (igIsItemActive())
                bgCol = igGetColorU32_Col(ImGuiCol.ButtonActive, 1);
            else if (igIsItemHovered(ImGuiHoveredFlags.None))
                bgCol = igGetColorU32_Col(ImGuiCol.ButtonHovered, 1);
            else
                bgCol = igGetColorU32_Col(ImGuiCol.Button, 1);
            Vector2 min = default;
            Vector2 max = default;
            igGetItemRectMin(ref min);
            igGetItemRectMax(ref max);
            ImDrawList_AddRectFilled(drawList, min, max, bgCol, 4.0f, ImDrawFlags.None);
            DrawSymbol(symbol, BoundBox.FromMinMax(min, max), drawList);
            return clicked;
        }
    }


    public readonly unsafe ref struct SameLineLayout
    {
        public readonly float Spacing;
        public SameLineLayout(float spacing = -1)
        {
            Spacing = spacing;
            igBeginGroup();

        }
        public void Next() => igSameLine(0f, Spacing);
        public void End() => igEndGroup();
        public float NextFill()
        {
            Next();
            Vector2 avail = default;
            igGetContentRegionAvail(ref avail);
            igSetNextItemWidth(avail.X);
            return avail.X;
        }

    }
}
