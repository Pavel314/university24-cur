using System;
using Sokol;
using System.Runtime.InteropServices;
using System.Runtime.CompilerServices;
using System.Numerics;
using static Sokol.SApp;
using static Sokol.SG;
using static Sokol.SGlue;
using static Sokol.SG.sg_vertex_format;
using static Sokol.SG.sg_index_type;
using static Sokol.SG.sg_cull_mode;
using static Sokol.SG.sg_compare_func;
using static Sokol.Utils;
using System.Diagnostics;
using static Sokol.SLog;
using static Sokol.SDebugUI;
using Imgui;
using static Sokol.SImgui;
using static Imgui.ImguiNative;
using t7_comp_graph;

public static unsafe class T7CompGraphApp
{
    private static sg_pass_action pass_action;
    
    private static readonly VectorEditor editor = new VectorEditor(BoundBox.FromXywh(0, 0, 640, 480));

    [UnmanagedCallersOnly]
    private static unsafe void Init()
    {
        const int maxVerts = 131072;
        editor.Statistics.CaptureBufferSize(maxVerts, maxVerts * 3);
        sg_setup(new sg_desc() { environment = sglue_environment(), logger = { func = &slog_func, } });
        simgui_setup(new simgui_desc_t { max_vertices = maxVerts, logger = { func = &slog_func } });//max_vertices = 262144 - It's requare 32-bit indices, so it's mean sokol should be compiled
        pass_action = default;
        pass_action.colors[0].load_action = sg_load_action.SG_LOADACTION_CLEAR;
        pass_action.colors[0].clear_value = new sg_color { r = 0.25f, g = 0.5f, b = 0.75f, a = 1.0f };
    }

    [UnmanagedCallersOnly]
    private static unsafe void Frame()
    {
        double deltaTime = sapp_frame_duration();

        float dpi = sapp_dpi_scale();
        float w = sapp_widthf() / dpi;
        float h = sapp_heightf() / dpi;
        float fps = igGetIO_Nil()->Framerate;

        simgui_new_frame(new simgui_frame_desc_t
        {
            width = sapp_width(),
            height = sapp_height(),
            delta_time = (float)deltaTime,
            dpi_scale = dpi,
        });
        sg_begin_pass(new sg_pass { action = pass_action, swapchain = sglue_swapchain() });
        editor.OnFrame(BoundBox.FromMinMax(0, 0, w, h), deltaTime);
        simgui_render();
        var stats = igGetDrawData();
        editor.Statistics.CaptureFrame(stats->TotalVtxCount, stats->TotalIdxCount, fps);
        sg_end_pass();
        sg_commit();
    }

    private static bool ToAppMouseButton(sapp_mousebutton sappBtn, out MouseButton balanceBtn)
    {
        (balanceBtn, bool ok) = sappBtn switch
        {
            sapp_mousebutton.SAPP_MOUSEBUTTON_LEFT => (MouseButton.Left, true),
            sapp_mousebutton.SAPP_MOUSEBUTTON_RIGHT => (MouseButton.Right, true),
            sapp_mousebutton.SAPP_MOUSEBUTTON_MIDDLE => (MouseButton.Middle, true),
            _ => (default, false),
        };
        return ok;
    }

    private static string ToAppKey(sapp_keycode key_code)
    {
        static string safeRemovePref(string pref, string s) => s.StartsWith(pref) ? s[pref.Length..] : s;
        return safeRemovePref("SAPP_KEYCODE_", key_code.ToString());
    }

    [UnmanagedCallersOnly]
    private static unsafe void Event(sapp_event* e)
    {
        simgui_handle_event(*e);

        ImGuiIO* io = igGetIO_Nil();
        bool hasMouse = io->WantCaptureMouse == 0;
        bool hasKeyboard = io->WantCaptureKeyboard == 0;

        static Vector2 getPos(float x, float y) => new Vector2(x, y) / sapp_dpi_scale();
        static Vector2 mousePos(sapp_event* e) => getPos(e->mouse_x, e->mouse_y);
        static Vector2 touchPos(sapp_event* e) => getPos(e->touches[0].pos_x, e->touches[0].pos_y);

        var inputSystem = editor.InputSystem;
        const MouseButton touchMouseButton = MouseButton.Left;

        switch (e->type)
        {
            case sapp_event_type.SAPP_EVENTTYPE_KEY_DOWN:
                if (hasKeyboard)
                    inputSystem.OnKeyDown(ToAppKey(e->key_code));
                break;

            case sapp_event_type.SAPP_EVENTTYPE_KEY_UP:
                if (hasKeyboard)
                    inputSystem.OnKeyUp(ToAppKey(e->key_code));
                break;

            case sapp_event_type.SAPP_EVENTTYPE_MOUSE_DOWN:
                if (hasMouse && ToAppMouseButton(e->mouse_button, out MouseButton mouseBtn))
                    inputSystem.OnMouseDown(mouseBtn, mousePos(e));
                break;
            case sapp_event_type.SAPP_EVENTTYPE_MOUSE_MOVE:
                if (hasMouse)
                    inputSystem.OnMouseMove(mousePos(e));
                break;
            case sapp_event_type.SAPP_EVENTTYPE_MOUSE_UP:
                if (ToAppMouseButton(e->mouse_button, out MouseButton btn) && inputSystem.GetMouse()[btn].IsDown)
                    inputSystem.OnMouseUp(btn, mousePos(e));
                break;
            case sapp_event_type.SAPP_EVENTTYPE_MOUSE_SCROLL:
                if (hasMouse)
                    inputSystem.OnMouseWheel((e->scroll_x, e->scroll_y));
                break;

            case sapp_event_type.SAPP_EVENTTYPE_TOUCHES_BEGAN:
                if (hasMouse && e->num_touches > 0)
                    inputSystem.OnMouseDown(touchMouseButton, touchPos(e));
                break;
            case sapp_event_type.SAPP_EVENTTYPE_TOUCHES_MOVED:
                if (hasMouse && e->num_touches > 0)
                    inputSystem.OnMouseMove(touchPos(e));
                break;
            case sapp_event_type.SAPP_EVENTTYPE_TOUCHES_ENDED:
            case sapp_event_type.SAPP_EVENTTYPE_TOUCHES_CANCELLED:
                if (inputSystem.GetMouse()[touchMouseButton].IsDown && e->num_touches > 0) 
                    inputSystem.OnMouseUp(touchMouseButton, touchPos(e));
                break;
        }
    }

    [UnmanagedCallersOnly]
    static void Cleanup()
    {
        sg_shutdown();

        // Force a complete shutdown if debugging
        if (Debugger.IsAttached)
        {
            Environment.Exit(0);
        }
    }

    public static SApp.sapp_desc sokol_main()
    {
        return new SApp.sapp_desc()
        {
            init_cb = &Init,
            frame_cb = &Frame,
            event_cb = &Event,
            cleanup_cb = &Cleanup,
            width = 1100,
            height = 900,
            sample_count = 4,
            window_title = "Discrete Vector Editor",
            high_dpi = true,
            icon = { sokol_default = true },
            logger = {
                func = &slog_func,
            }
        };
    }
}