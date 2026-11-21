using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace t7_comp_graph
{
    public readonly record struct KeyState(bool IsDown, bool IsPressed, bool IsReleased)
    {
        public static readonly KeyState None = default;
    }

    public class KeyboardInput
    {
        private readonly Dictionary<string, KeyState> keys = new(StringComparer.OrdinalIgnoreCase);
        public KeyState GetKey(string k) => keys.GetValueOrDefault(k, KeyState.None);
        public void OnKeyDown(string k)
        {
            ref var key = ref CollectionsMarshal.GetValueRefOrAddDefault(keys, k, out bool _);
            if (key.IsDown) return;
            key = key with { IsDown = true, IsPressed = true };
        }
        public void OnKeyUp(string k)
        {
            ref var key = ref CollectionsMarshal.GetValueRefOrNullRef(keys, k);
            if (Unsafe.IsNullRef(ref key) || !key.IsDown) return;
            key = key with { IsDown = false, IsReleased = true };
        }
        public void OnFrame(double dt)
        {
            foreach (var k in keys.Keys)
            {
                ref var key = ref CollectionsMarshal.GetValueRefOrNullRef(keys, k);
                key = key with { IsPressed = false, IsReleased = false };
            }
        }
        public void Reset() => keys.Clear();
    }

    public enum MouseButton { Left = 0, Right = 1, Middle = 2 }

    public readonly record struct MouseButtonState(bool IsDown, Vector DownPos, bool IsPressed, bool IsReleased)
    {
        public MouseButtonState() : this(false, new Vector(0, 0), false, false) { }
        public static readonly MouseButtonState None = default;
    }

    public struct MouseInput
    {
        public const int ButtonsCount = 3;

        [InlineArray(ButtonsCount)]
        public struct MouseButtonsBuffer()
        {
            private MouseButtonState _element0;
        }

        public Vector Pos;
        public Vector Delta;
        public Vector Wheel;
        public MouseButtonsBuffer Buttons;

        public MouseInput()
        {
            Pos = new Vector(0, 0);
            Delta = new Vector(0, 0);
            Wheel = new Vector(0, 0);
            for (int i = 0; i < ButtonsCount; i++) Buttons[i] = MouseButtonState.None;
        }
        public static MouseInput CreateEmpty() => new();

        public MouseButtonState this[MouseButton button]
        {
            get => Buttons[(int)button];
            set => Buttons[(int)button] = value;
        }

        public void OnMouseDown(MouseButton btn, Vector pos)
        {
            Pos = pos;
            this[btn] = this[btn] with { IsDown = true, DownPos = pos, IsPressed = true };
        }

        public void OnMouseUp(MouseButton btn, Vector pos)
        {
            Pos = pos;
            this[btn] = this[btn] with { IsDown = false, IsReleased = true };
        }

        public void OnMouseMove(Vector pos)
        {
            Delta += pos - Pos;
            Pos = pos;
        }

        public void OnMouseWheel(Vector delta)
        {
            Wheel += delta;
        }

        public void OnFrame(double dt)
        {
            Delta = new Vector(0, 0);
            Wheel = new Vector(0, 0);
            for (int i = 0; i < ButtonsCount; i++) Buttons[i] = Buttons[i] with { IsPressed = false, IsReleased = false };
        }
    }

    public class InputSystem
    {
        private readonly KeyboardInput Keyboard;
        private MouseInput Mouse;
        public InputSystem()
        {
            Keyboard = new KeyboardInput();
            Mouse = MouseInput.CreateEmpty();
        }
        public void OnKeyDown(string key) => Keyboard.OnKeyDown(key);
        public void OnKeyUp(string key) => Keyboard.OnKeyUp(key);
        public void OnMouseDown(MouseButton btn, Vector pos) => Mouse.OnMouseDown(btn, pos);
        public void OnMouseUp(MouseButton btn, Vector pos) => Mouse.OnMouseUp(btn, pos);
        public void OnMouseMove(Vector pos) => Mouse.OnMouseMove(pos);
        public void OnMouseWheel(Vector delta) => Mouse.OnMouseWheel(delta);
        public void OnFrame(double dt)
        {
            Keyboard.OnFrame(dt);
            Mouse.OnFrame(dt);
        }
        public void Reset()
        {
            Keyboard.Reset();
            Mouse = MouseInput.CreateEmpty();
        }
        public KeyState GetKey(string key) => Keyboard.GetKey(key);
        public MouseInput GetMouse() => Mouse;
    }
}
