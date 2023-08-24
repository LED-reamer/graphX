#pragma once


#include <vector>  // Include STL vector class.

#include "graphX/vendor/glm/glm.hpp"

typedef glm::vec2 Vector2d;

typedef std::vector< Vector2d > Vector2dVector;


class Triangulate
{
public:

    // triangulate a contour/polygon, places results in STL vector
    // as series of triangles.
    static bool Process(const Vector2dVector& contour, Vector2dVector& result);
    static bool Process(Vector2d* contour, size_t size, Vector2dVector& result);

    // compute area of a contour/polygon
    static float Area(const Vector2dVector& contour);
    static float Area(Vector2d* contour, size_t size);

    // decide if point Px/Py is inside triangle defined by
    // (Ax,Ay) (Bx,By) (Cx,Cy)
    static bool InsideTriangle(float Ax, float Ay,
        float Bx, float By,
        float Cx, float Cy,
        float Px, float Py);


private:
    static bool Snip(const Vector2dVector& contour, int u, int v, int w, int n, int* V);
    static bool Snip(Vector2d* contour, size_t size, int u, int v, int w, int n, int* V);

};