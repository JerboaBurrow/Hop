#ifndef RENDERSTATE_H
#define RENDERSTATE_H

struct RenderState {
    RenderState()
    : ox(0.),oy(0.),otheta(0.),os(0.)
    {}
    RenderState(float x, float y, float t, float s)
    : ox(x), oy(y), otheta(t), os(s)
    {}
    float ox;
    float oy;
    float otheta;
    float os;
};

#endif /* RENDERSTATE_H */
