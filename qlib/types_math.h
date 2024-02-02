#ifndef TYPES_MATH_H
#define TYPES_MATH_H

//
// Vector3
//

Vector3 v3_add(const Vector3 l, const Vector3 r) {
    Vector3 result = { l.x + r.x, l.y + r.y, l.z + r.z, };
    return result;
}

Vector3 v3_subtract(const Vector3 l, const Vector3 r) {
    Vector3 result = { l.x - r.x, l.y - r.y, l.z - r.z, };
    return result;
}

Vector3 v3_multiply_float32(const Vector3 l, const float32 r) {
    Vector3 result = { l.x * r, l.y * r, l.z * r, };
    return result;
}

Vector3 v3_multiply(const Vector3 l, const Vector3 r) {
    Vector3 result = { l.x * r.x, l.y * r.y, l.z * r.z, };
    return result;
}

bool8 v3_compare_float32(const Vector3 l, const float32 r) {
    if (l.x == r && l.y == r && l.z == r)
        return TRUE;
    else
        return FALSE;
}

float32 dot_product(const Vector3 l, const Vector3 r) { return (l.x * r.x) + (l.y * r.y) + (l.z * r.z); }
float32 length_squared(const Vector3 v) { return (v.x * v.x) + (v.y * v.y) + (v.z * v.z); }

void normalize(Vector3 *v) {
    float32 len_sq = length_squared(*v);
    if (len_sq < EPSILON) return;
    float32 inverse_length = 1.0f / sqrtf(len_sq);
    v->x *= inverse_length;
    v->y *= inverse_length;
    v->z *= inverse_length;
}

Vector3 normalized_v3(const Vector3 v) {
    float32 len_sq = length_squared(v);
    if (len_sq < EPSILON) return v;
    float32 inverse_length = 1.0f / sqrtf(len_sq);
    Vector3 result = { v.x * inverse_length, v.y * inverse_length, v.z * inverse_length };
    return result;
}

Vector3 cross_product(const Vector3 l, const Vector3 r) {
    Vector3 result =  {
        (l.y * r.z - l.z * r.y),
        (l.z * r.x - l.x * r.z),
        (l.x * r.y - l.y * r.x)
    };
    return result;
}

//
// Vector4
//

float32 v4_dot_product(const Vector4 l, const Vector4 r) { return (l.x * r.x) + (l.y * r.y) + (l.z * r.z) + (l.w * r.w); }

//
// Matrix_4x4
//

Vector4 m4x4_get_row(Matrix_4x4 m, u32 i) {
    Vector4 row;
    row.E[0] = m.E[0][i];
    row.E[1] = m.E[1][i];
    row.E[2] = m.E[2][i];
    row.E[3] = m.E[3][i];
    return row;
}

Vector4 m4x4_mul_v4(Matrix_4x4 m, Vector4 v) {
    
    Vector4 result =  {
        v4_dot_product(m4x4_get_row(m, 0), v),
        v4_dot_product(m4x4_get_row(m, 1), v),
        v4_dot_product(m4x4_get_row(m, 2), v),
        v4_dot_product(m4x4_get_row(m, 3), v)
    };
    return result;
}

Matrix_4x4 identity_m4x4() {
    Matrix_4x4 result =  {
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
    };
    return result;
}

Matrix_4x4 get_frustum(float32 l, float32 r, float32 b, float32 t, float32 n, float32 f) {
    if (l == r || t == b || n == f) {
        //logprint("get_frustum", "Invalid frustum");
        return (Matrix_4x4) { 0 };
    }
    
    return (Matrix_4x4) {
        (2.0f * n) / (r - l), 0, 0, 0,
        0, (2.0f * n) / (t - b), 0, 0,
        (r + l) / (r - l), (t + b) / (t - b), (-(f + n)) / (f - n), -1,
        0, 0, (-2 * f * n) / (f - n), 0
    };
}

Matrix_4x4 perspective_projection(float32 fov, float32 aspect_ratio, float32 n, float32 f)
{
    // (fov / 2.0f) * (2PI / 360.0f)
    // x_max = y_max * (window_width / window_height)
    float32 y_max = n * tanf(fov * PI / 360.0f);
    float32 x_max = y_max * aspect_ratio;
    return get_frustum(-x_max, x_max, -y_max, y_max, n, f);
}

Matrix_4x4 look_at(const Vector3 position, const Vector3 target, const Vector3 up) {
    Vector3 s = v3_subtract(target, position);
    Vector3 f = normalized_v3(s);
    f = v3_multiply_float32(f, -1.0f);
    
    Vector3 r = cross_product(up, f);
    if (v3_compare_float32(r, 0)) 
        return identity_m4x4();
    normalize(&r);
    Vector3 u = normalized_v3(cross_product(f, r));
    Vector3 t = {-dot_product(r, position), -dot_product(u, position), -dot_product(f, position)};
    
    Matrix_4x4 result = {
        r.x, u.x, f.x, 0,
        r.y, u.y, f.y, 0,
        r.z, u.z, f.z, 0,
        t.x, t.y, t.z, 1
    };
    return result;
}

Matrix_4x4 create_transform_m4x4(Vector3 position, Vector3 scale) {
    Vector3 x = {1, 0, 0};
    Vector3 y = {0, 1, 0};
    Vector3 z = {0, 0, 1};
    
    //x = rotation * x;
    //y = rotation * y;
    //z = rotation * z;
    
    x = v3_multiply_float32(x, scale.x);
    y = v3_multiply_float32(y, scale.y);
    z = v3_multiply_float32(z, scale.z);
    
    Matrix_4x4 result = {
        x.x, x.y, x.z, 0,
        y.x, y.y, y.z, 0,
        z.x, z.y, z.z, 0,
        position.x, position.y, position.z, 1
    };
    return result;
}

#endif // TYPES_MATH_H