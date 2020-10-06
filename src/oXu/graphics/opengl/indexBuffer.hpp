#pragma once

namespace oxu::OpenGL
{
    class IndexBuffer
    {
    private:
        unsigned int m_id = 0;

    public:
        IndexBuffer(const unsigned int *indices, unsigned int count);
        ~IndexBuffer();

        void bind() const;
        void unbind() const;
    };
}