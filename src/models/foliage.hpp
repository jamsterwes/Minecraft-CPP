namespace foliage_data
{
#define FOLIAGE_VERTICES(x, y, z, i, s) { \
    /* +X+Z */ \
    0.15 + x, 1.00 + y, 0.85 + z, 1.0, 1.0, 1.0, 0,0, i, s, \
    0.15 + x, 0.00 + y, 0.85 + z, 1.0, 1.0, 1.0, 0,1, i, s, \
    0.85 + x, 1.00 + y, 0.15 + z, 1.0, 1.0, 1.0, 1,0, i, s, \
    0.85 + x, 0.00 + y, 0.15 + z, 1.0, 1.0, 1.0, 1,1, i, s, \
    /* -X-Z */ \
    0.15 + x, 1.00 + y, 0.85 + z, -1.0, 1.0, -1.0, 0,0, i, s, \
    0.15 + x, 0.00 + y, 0.85 + z, -1.0, 1.0, -1.0, 0,1, i, s, \
    0.85 + x, 1.00 + y, 0.15 + z, -1.0, 1.0, -1.0, 1,0, i, s, \
    0.85 + x, 0.00 + y, 0.15 + z, -1.0, 1.0, -1.0, 1,1, i, s, \
    /* -X+Z */ \
    0.15 + x, 1.00 + y, 0.15 + z, -1.0, 1.0, 1.0, 0,0, i, s, \
    0.15 + x, 0.00 + y, 0.15 + z, -1.0, 1.0, 1.0, 0,1, i, s, \
    0.85 + x, 1.00 + y, 0.85 + z, -1.0, 1.0, 1.0, 1,0, i, s, \
    0.85 + x, 0.00 + y, 0.85 + z, -1.0, 1.0, 1.0, 1,1, i, s, \
    /* +X-Z */ \
    0.15 + x, 1.00 + y, 0.15 + z, 1.0, 1.0, -1.0, 0,0, i, s, \
    0.15 + x, 0.00 + y, 0.15 + z, 1.0, 1.0, -1.0, 0,1, i, s, \
    0.85 + x, 1.00 + y, 0.85 + z, 1.0, 1.0, -1.0, 1,0, i, s, \
    0.85 + x, 0.00 + y, 0.85 + z, 1.0, 1.0, -1.0, 1,1, i, s, \
};

const unsigned int indices[] = {
    // +X+Z
    0, 3, 2,
    0, 1, 3,
    // -X-Z
    6, 7, 4,
    7, 5, 4,
    // -X+Z
    8, 11, 10,
    8, 9, 11,
    // +X-Z
    14, 15, 12,
    15, 13, 12,
};

} // namespace foliage_data