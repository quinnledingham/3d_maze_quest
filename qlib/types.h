#ifndef TYPES_H
#define TYPES_H

typedef union {
	struct {
		float32 x, y;
	};
	struct {
		float32 u, v;
	};
	struct {
		float32 width, height;
	};
	float32 E[2];
} Vector2;

typedef union  {
	struct {
		s32 x, y;
	};
	struct {
		s32 u, v;
	};
	struct {
		s32 width, height;
	};
	s32 E[2];
} Vector2_s32;

typedef union {
	struct {
		float32 x, y, z;
	};
	struct {
		float32 r, g, b;
	};
	float32 E[3];
} Vector3;

typedef union {
	struct {
		float32 x, y, z, w;
	};
	struct {
		float32 r, g, b, a;
	};
	struct {
		Vector3 rgb;
	};
	float32 E[4];
} Vector4;

typedef union {
    struct {
        float32 x, y, z, w;
    };
    struct {
        Vector3 vector;
        float32 scalar;
    };
    float32 E[4];
} Quaternion;


typedef union {
	struct {
		Vector4 x, y, z, w;
	};
    float32 E[4][4];
} Matrix_4x4;


#endif // TYPES_H