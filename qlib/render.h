typedef enum {
    VECTOR2,
    VECTOR3
} Vector_Type;

#define MAX_ATTRIBUTES 5

struct Vertex_Info {
    u32 attributes_count;
    Vector_Type formats[MAX_ATTRIBUTES];
    u32 offsets[MAX_ATTRIBUTES];

    u32 size;
};

void add_attribute(struct Vertex_Info *info, Vector_Type format, u32 offset) {
    if (info->attributes_count >= MAX_ATTRIBUTES) {
        //logprint("Vertex_Info add()", "tried to add too many attributes\n");
        return;
    }

    info->formats[info->attributes_count] = format;
    info->offsets[info->attributes_count] = offset;
    info->attributes_count++;
}

typedef struct Vertex_XNU_s {
    Vector3 position;
    Vector3 normal;
    Vector2 uv;
} Vertex_XNU_t;

internal struct Vertex_Info
get_vertex_xnu_info() {
    struct Vertex_Info info = { 0 };
    add_attribute(&info, VECTOR3, offsetof(Vertex_XNU_t, position));
    add_attribute(&info, VECTOR3, offsetof(Vertex_XNU_t, normal));
    add_attribute(&info, VECTOR2, offsetof(Vertex_XNU_t, uv));
    info.size = sizeof(Vertex_XNU_t);
    return info;
}

struct Mesh {
    struct Vertex_Info vertex_info;

    void *vertices;
    u32 vertices_count;

    u32 *indices;
    u32 indices_count;

    //Material material;

    void *gpu_info;
};

typedef union Camera_u {
    struct {
        Vector3 position;
        Vector3 target;
        Vector3 up;
        float32 fov;
        float32 yaw;
        float32 pitch;
    };
    float32 E[12];
} Camera_t;

internal Matrix_4x4 get_view(Camera_t camera)  { 
    return look_at(camera.position, v3_add(camera.position, camera.target), camera.up); 
}

internal void
update_camera_target(Camera_t *camera) {
    Vector3 camera_direction = {
        cosf(DEG2RAD * camera->yaw) * cosf(DEG2RAD * camera->pitch),
        sinf(DEG2RAD * camera->pitch),
        sinf(DEG2RAD * camera->yaw) * cosf(DEG2RAD * camera->pitch)
    };
    camera->target = normalized_v3(camera_direction);
}