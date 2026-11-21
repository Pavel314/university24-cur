namespace t7_comp_graph
{
    public enum MouseActionType { None, SelectShape, CreatePreview, CommitPreview, CancelPreview, TranslateShape, AddAndInitControlPoint, InitControlPoint, MoveControlPoint, ReleaseControlPoint }
    public readonly record struct MouseCommand(MouseActionType Action, Vector DragDelta)
    {
        public static readonly MouseCommand None = new (MouseActionType.None, Vector.Zero);
        public static MouseCommand Single(MouseActionType action) => new MouseCommand(action, Vector.Zero);
    }

    public class MouseInputHelper
    {
        private enum DragMode { None, CreateShape, TranslateShape, MoveControlPoint }

        private DragMode mode;
        private bool initDragThreshold;
        private Vector dragStart;

        public MouseInputHelper() => Reset();

        public void Reset()
        {
            mode = DragMode.None;
            initDragThreshold = false;
            dragStart = Vector.Zero;
        }

        private MouseCommand FailReleaseAll()
        {
            var wasMode = mode;
            Reset();
            return wasMode switch
            {
                DragMode.CreateShape => MouseCommand.Single(MouseActionType.CancelPreview),
                DragMode.MoveControlPoint => MouseCommand.Single(MouseActionType.ReleaseControlPoint),
                _ => MouseCommand.None,
            };
        }

        public MouseCommand OnMouseInput(MouseButtonState leftBtn, Vector worldPos, bool hasActiveCreator, bool hasSelectedShape, bool hasControlPoint, bool hasPenMode)
        {
            if (leftBtn.IsPressed)
            {
                dragStart = worldPos;
                if (hasActiveCreator)
                {
                    mode = DragMode.CreateShape;
                    return MouseCommand.None;
                }
                if (hasControlPoint)
                {
                    mode = DragMode.MoveControlPoint;
                    return MouseCommand.Single(MouseActionType.InitControlPoint);
                }
                if (hasSelectedShape && hasPenMode)
                {
                    mode = DragMode.MoveControlPoint;
                    return MouseCommand.Single(MouseActionType.AddAndInitControlPoint);
                }
                mode = DragMode.TranslateShape;
                return MouseCommand.Single(MouseActionType.SelectShape);
            }
            if (leftBtn.IsDown)
            {
                if (mode == DragMode.None) return MouseCommand.None;
                Vector dragDelta = worldPos - dragStart;
                if (!initDragThreshold)
                    if (dragDelta.LengthSquared() < 0.5*0.5) return MouseCommand.None; else initDragThreshold = true;

                return mode switch
                {
                    DragMode.CreateShape when hasActiveCreator => new MouseCommand(MouseActionType.CreatePreview, dragDelta),
                    DragMode.MoveControlPoint when hasSelectedShape => new MouseCommand(MouseActionType.MoveControlPoint, dragDelta),
                    DragMode.TranslateShape when hasSelectedShape => new MouseCommand(MouseActionType.TranslateShape, dragDelta),
                    _ => FailReleaseAll()
                };
            }
            if (leftBtn.IsReleased)
            {
                var wasMode = mode;
                Reset();
                return wasMode switch
                {
                    DragMode.CreateShape => MouseCommand.Single(MouseActionType.CommitPreview),
                    DragMode.MoveControlPoint => MouseCommand.Single(MouseActionType.ReleaseControlPoint),
                    _ => MouseCommand.None,
                };
            }
            return MouseCommand.None;

        }
    }
}
