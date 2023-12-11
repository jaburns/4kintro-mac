
static float r = 0.0, g = 0.0, b = 0.0;

static void render() {
    r += 0.03;
    g += 0.07;
    b += 0.011;
    glClearColor(
        0.5+0.5*sinf(r),
        0.5+0.5*cosf(g),
        0.5+0.5*sinf(b),
        1.0
    );
    glClear(GL_COLOR_BUFFER_BIT);
}
