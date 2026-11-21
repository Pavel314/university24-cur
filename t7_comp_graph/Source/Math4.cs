namespace t7_comp_graph
{
    public readonly record struct Vector4(double X, double Y, double Z, double W)
    {
        public readonly static Vector4 Zero = new Vector4(0, 0, 0, 0);
        public static double Dot(Vector4 v1, Vector4 v2) => v1.X * v2.X+ v1.Y * v2.Y+ v1.Z * v2.Z+ v1.W * v2.W;
        public double LengthSquared() => Dot(this, this);
        public double Length() => Math.Sqrt(LengthSquared());
        public Vector4 SafeDiv(double s, Vector4 dflt=default) => Math.Abs(s) < 1e-12 ? dflt : this / s;
        public Vector4 Norm() => SafeDiv(Length());

        public static Vector4 operator +(Vector4 v1, Vector4 v2) => new(v1.X + v2.X, v1.Y + v2.Y, v1.Z + v2.Z, v1.W + v2.W);
        public static Vector4 operator -(Vector4 v1, Vector4 v2) => new(v1.X - v2.X, v1.Y - v2.Y, v1.Z - v2.Z, v1.W - v2.W);
        public static Vector4 operator *(Vector4 v, double s) => new(v.X * s, v.Y * s, v.Z * s, v.W * s);
        public static Vector4 operator *(double s, Vector4 v) => v * s;
        public static double operator *(Vector4 v1, Vector4 v2) => Dot(v1, v2);
        public static Vector4 operator /(Vector4 v, double s) => new(v.X / s, v.Y / s, v.Z / s, v.W / s);
        public static Vector4 operator -(Vector4 v) => new(-v.X, -v.Y, -v.Z, -v.W);
        public static implicit operator Vector4((double X, double Y, double Z, double W) t) => new(t.X, t.Y, t.Z, t.W);
    }

    public readonly record struct Matrix4(Vector4 Row1, Vector4 Row2, Vector4 Row3, Vector4 Row4)
    {
        public static readonly Matrix4 Identity = new(
            1, 0, 0, 0,
            0, 1, 0, 0,
            0, 0, 1, 0,
            0, 0, 0, 1
        );
        public double M11 => Row1.X;
        public double M12 => Row1.Y;
        public double M13 => Row1.Z;
        public double M14 => Row1.W;
        public double M21 => Row2.X;
        public double M22 => Row2.Y;
        public double M23 => Row2.Z;
        public double M24 => Row2.W;
        public double M31 => Row3.X;
        public double M32 => Row3.Y;
        public double M33 => Row3.Z;
        public double M34 => Row3.W;
        public double M41 => Row4.X;
        public double M42 => Row4.Y;
        public double M43 => Row4.Z;
        public double M44 => Row4.W;

        public Vector4 Col1 => new(M11, M21, M31, M41);
        public Vector4 Col2 => new(M12, M22, M32, M42);
        public Vector4 Col3 => new(M13, M23, M33, M43);
        public Vector4 Col4 => new(M14, M24, M34, M44);

        public Matrix4(
            double m11, double m12, double m13, double m14,
            double m21, double m22, double m23, double m24,
            double m31, double m32, double m33, double m34,
            double m41, double m42, double m43, double m44)
            : this(
                new Vector4(m11, m12, m13, m14),
                new Vector4(m21, m22, m23, m24),
                new Vector4(m31, m32, m33, m34),
                new Vector4(m41, m42, m43, m44))
                { }

        public Matrix4 Transpose() => new(Col1, Col2, Col3, Col4);
        public static Vector4 operator *(Matrix4 a, Vector4 b) => new(a.Row1 * b, a.Row2 * b, a.Row3 * b, a.Row4 * b);
        public static Vector4 operator *(Vector4 a, Matrix4 b) => a.X * b.Row1 + a.Y * b.Row2 + a.Z * b.Row3 + a.W * b.Row4;
        public static Matrix4 operator *(Matrix4 a, Matrix4 b) => new(a.Row1 * b, a.Row2 * b, a.Row3 * b, a.Row4 * b);
        public static Matrix4 operator *(Matrix4 a, double b) => new(a.Row1 * b, a.Row2 * b, a.Row3 * b, a.Row4 * b);
        public static Matrix4 operator *(double a, Matrix4 b) => b * a;
        public static Matrix4 operator +(Matrix4 a, Matrix4 b) => new(a.Row1 + b.Row1, a.Row2 + b.Row2, a.Row3 + b.Row3, a.Row4 + b.Row4);
        public static Matrix4 operator -(Matrix4 a, Matrix4 b) => new(a.Row1 - b.Row1, a.Row2 - b.Row2, a.Row3 - b.Row3, a.Row4 - b.Row4);
    }
}
