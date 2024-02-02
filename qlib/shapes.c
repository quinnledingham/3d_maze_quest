void get_rect_mesh(struct Mesh *mesh) {
    Vertex_XNU_t vertices[4] = { 
        { {-0.5, -0.5, 0}, {0, 0, -1}, {0, 0} },
        { {-0.5,  0.5, 0}, {0, 0, -1}, {0, 1} },
        { { 0.5, -0.5, 0}, {0, 0, -1}, {1, 0} },
        { { 0.5,  0.5, 0}, {0, 0, -1}, {1, 1} },
    };

    mesh->vertices_count = ARRAY_COUNT(vertices);    
    mesh->vertices = ARRAY_MALLOC(Vertex_XNU_t, mesh->vertices_count);

    memcpy(mesh->vertices, vertices, sizeof(Vertex_XNU_t) * mesh->vertices_count);

    mesh->indices_count = 6;
    mesh->indices = ARRAY_MALLOC(u32, mesh->indices_count);

    // vertex locations
    u32 top_left = 0, top_right = 2, bottom_left = 1, bottom_right = 3;
   
    mesh->indices[0] = top_left;
    mesh->indices[1] = bottom_left;
    mesh->indices[2] = bottom_right;
    mesh->indices[3] = top_left;
    mesh->indices[4] = bottom_right;
    mesh->indices[5] = top_right;

    mesh->vertex_info = get_vertex_xnu_info();
}