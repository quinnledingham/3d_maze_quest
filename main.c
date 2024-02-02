#ifdef _WINDLL
__declspec(dllexport)
#endif

#include <stdio.h>
#include <stdlib.h>

#include "pd_api.h"

#include "defines.h"
#include "types.h"
#include "types_math.h"
#include "render.h"

#include "shapes.c"

PlaydateAPI *pd = 0;

typedef struct {
    Matrix_4x4 projection;
    Matrix_4x4 view;
} Scene_t;

struct Game_Data_s {
    LCDFont *font;
    struct Mesh rect;

    Scene_t scene;
    Camera_t camera;
};

struct Game_Data_s game_data;

LCDPattern grey50 = {
    0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, // Bitmap, each byte is a row of pixel
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, // Mask, here fully opaque
};

internal Vector2_s32
get_viewport_coords(float32 x, float32 y, 
             s32 x_wmax, s32 y_wmax, s32 x_wmin, s32 y_wmin,
             s32 x_vmax, s32 y_vmax, s32 x_vmin, s32 y_vmin) {
    Vector2_s32 result;
    float32 sx, sy;
    sx = (float32)(x_vmax - x_vmin) / (x_wmax - x_wmin);
    sy = (float32)(y_vmax - y_vmin) / (y_wmax - y_wmin);

    result.x = (s32)(x_vmin + (float32)((x - (float32)x_wmin) * sx));
    result.y = (s32)(y_vmin + (float32)((y - (float32)y_wmin) * sy));
    return result;
}

internal u32
get_screen_coords(Vector2_s32 *result, Vertex_XNU_t v, Matrix_4x4 model) {
    Vector3 v3 = v.position;
    Vector4 v4 = { v3.x, v3.y, v3.z, 1.0f };
    Vector4 pos = m4x4_mul_v4(game_data.scene.projection, m4x4_mul_v4(game_data.scene.view, m4x4_mul_v4(model, v4)));
    pos.x = pos.x / pos.w;
    pos.y = pos.y / pos.w;
    pos.z = pos.z / pos.w;
    pos.w = pos.w / pos.w;

    if (pos.z > 1.0f || pos.z < -1.0f) {
        //pd->system->logToConsole("%f", pos.z);
        return 1;
    }
    *result = get_viewport_coords(pos.x, pos.y, 
                                             1, 1, -1, -1,
                                             LCD_COLUMNS, LCD_ROWS, 0, 0);

    return 0;
}

internal void
draw_mesh(struct Mesh *mesh, Matrix_4x4 model) {
    Vertex_XNU_t *vertices = mesh->vertices;
    for (u32 i = 0; i < mesh->indices_count; i += 3) {
        Vector2_s32 a, b, c;
        if (get_screen_coords(&a, vertices[mesh->indices[i]    ], model) ||
            get_screen_coords(&b, vertices[mesh->indices[i + 1]], model) ||
            get_screen_coords(&c, vertices[mesh->indices[i + 2]], model))
            continue;
        pd->graphics->fillTriangle(a.x, a.y, c.x, c.y, b.x, b.y, grey50);
    }
}

internal void
draw_rect(Vector3 position, Vector3 scale) {
    Matrix_4x4 model = create_transform_m4x4(position, scale);
    draw_mesh(&game_data.rect, model);
}

internal int
update(void *userdata) {
    pd->graphics->clear(kColorWhite);
    pd->graphics->setFont(game_data.font);
    pd->system->drawFPS(0,0);

    PDButtons down;
    pd->system->getButtonState(&down, 0, 0);

    Vector3 magnitude = { 0.01f, 0.01f, 0.01f };

    if (down & kButtonRight) {
        game_data.camera.yaw += 1.0f;
    }

    if (down & kButtonLeft) {
        game_data.camera.yaw -= 1.0f;
    }

    float32 crank_angle = pd->system->getCrankAngle();
    float32 crank_delta = pd->system->getCrankChange();
    
    Vector3 position_delta = { crank_delta / 360.0f, crank_delta / 360.0f, crank_delta / 360.0f };
    v3_multiply(position_delta, magnitude);
    game_data.camera.position = v3_add(game_data.camera.position, v3_multiply(game_data.camera.target, position_delta));
    //pd->system->logToConsole("%f", position_delta.x);
    
    float32 bob_magnitude = 0.5f;
    game_data.camera.position.y = bob_magnitude * sinf(crank_angle * DEG2RAD);

    update_camera_target(&game_data.camera);
    game_data.scene.view = get_view(game_data.camera);

    Vector3 pos = { 0, 0, 0 };
    Vector3 scale = { 2, 2, 2 };
    draw_rect(pos, scale);

    return 1;
}

int eventHandler(PlaydateAPI* playdate, PDSystemEvent event, uint32_t arg) {
    switch(event) {
        case kEventInit: {
            pd = playdate;
            pd->system->setUpdateCallback(update, pd);
            
            const char *fontpath = "/System/Fonts/Asheville-Sans-14-Bold.pft";
            const char* err;
            game_data.font = pd->graphics->loadFont(fontpath, &err);
            if (game_data.font == NULL)
                pd->system->error("%s:%i Couldn't load font %s: %s", __FILE__, __LINE__, fontpath, err);

            Camera_t camera = {
                .position = { 0.0f,  0.0f, -5.0f },
                { 0.0f,  0.0f,  0.0f },
                { 0.0f,  1.0f,  0.0f },
                75.0f,
                45.0f,
                0.0f
            };
            game_data.camera = camera;
            update_camera_target(&game_data.camera);

            get_rect_mesh(&game_data.rect);

            game_data.scene.projection = perspective_projection(75.0f, (float32)LCD_COLUMNS / (float32)LCD_ROWS, 0.1f, 1000.0f);
            game_data.scene.view = get_view(game_data.camera);

        } break;
    }

    return 0;
}