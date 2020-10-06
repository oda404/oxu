#pragma once

#include<memory>
#include<vector>

#include<oXu/graphics/opengl/vertexBuffer.hpp>
#include<oXu/graphics/opengl/vertexLayout.hpp>

namespace oxu::OpenGL
{
    class VertexArrayObject
    {
    private:
        unsigned int m_id = 0;
        std::unique_ptr<VertexBuffer> m_vertexBuffer;

    public:
        VertexArrayObject();
        ~VertexArrayObject();

        template<typename T>
        void setVertexBuffer(const void *data, unsigned int count);

        template<typename T>
        void modifyVertexBuffer(const void *data, unsigned int count);

        void setVertexLayout(const VertexLayout &vertexLayout);

        void bind() const;
        void unbind() const;
    };
}