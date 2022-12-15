#ifndef CTRANSFORM_H
#define CTRANSFORM_H

struct cTransform {
    double x, y, scale, theta;

    cTransform() = default;

    cTransform(
        double x,
        double y,
        double s,
        double t
    )
    : x(x), y(y), scale(s), theta(t)
    {}
};

#endif /* CTRANSFORM_H */
