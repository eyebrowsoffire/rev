#pragma once

#include "rev/IModel.h"
#include "rev/NurbsCurve.h"
#include "rev/gl/Buffer.h"

namespace rev {
class TrackModel : public IModel {
public:
    TrackModel(const NurbsCurve<glm::vec3>& curve, float width, size_t segments)
    {
        VertexArrayContext vertexContext(_vao);
        ArrayBufferContext arrayBufferContext(_vertices);

        float start = curve.getStart();
        float end = curve.getEnd();
        float curveLength = end - start;
        float segmentLength = curveLength / static_cast<float>(segments);
    }

    virtual VertexArrayContext getVertexArrayContext()
    {
        return VertexArrayContext(_vao);
    }
    virtual void draw() = 0;

private:
    VertexArray _vao;
    Buffer _vertices;
    Buffer _indexes;
    size_t _vertexCount;
};
}; // namespace rev