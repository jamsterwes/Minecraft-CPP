#include "fs_quad.hpp"
#include "../models/fs_quad.hpp"

namespace post
{
    FSQuad::FSQuad()
    {
        buffer = new gfx::iv_buffer<>(fs_quad_data::indices, ARRAYSIZE(fs_quad_data::indices),
                                      fs_quad_data::vertices, ARRAYSIZE(fs_quad_data::vertices),
                                      [] () {
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(0));
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
            glEnableVertexAttribArray(1);
        });
    }

    void FSQuad::Draw()
    {
        buffer->Draw();
    }
}