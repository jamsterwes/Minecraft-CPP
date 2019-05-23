namespace fs_quad_data
{
const float vertices[20] = {
    -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
    -1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
    1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
    1.0f, -1.0f, 0.0f, 1.0f, 0.0f};

const unsigned int indices[6] = {
    0, 3, 1,
    1, 3, 2};
} // namespace fs_quad_data