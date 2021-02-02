#include"backend.hpp"

#include<glad/glad.h>

#include<vector>

#include<oXu/graphics/opengl/core.hpp>
#include<oXu/graphics/opengl/shader.hpp>
#include<oXu/graphics/opengl/indexBuffer.hpp>
#include<oXu/graphics/opengl/vertexArrayObject.hpp>
#include<oXu/graphics/opengl/vertexLayout.hpp>
#include<oXu/graphics/opengl/texture.hpp>
#include<oXu/core/window.hpp>
#include<oXu/core/logger.hpp>

#define SCREEN_START_COORD -1.0f
#define SCREEN_END_COORD 1.0f

namespace oxu::graphics::opengl
{
    enum Shaders
    {
        TEXTURE = 0
    };

    std::vector<Shader> c_shaders;
    SDL_GLContext c_GL_context;

    static float getNormalizedScreenCoord(float coord, bool xAxis)
    {
        return SCREEN_START_COORD + 
        (coord * (SCREEN_END_COORD -  SCREEN_START_COORD)) / 
        (xAxis ? (float)window::get_window_size().x : (float)window::get_window_size().y);
    }

    bool Backend::init(SDL_Window *p_window_p)
    {
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

        c_GL_context = SDL_GL_CreateContext(p_window_p);

        if(!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress))
        {
            OXU_LOG_ERR("Failed to initialize glad");
            return false;
        }

        OXU_LOG_DEBUG("OpenGL {}.{}", GLVersion.major, GLVersion.minor);

        oxu_glCall_Assert(glEnable(GL_BLEND));
        oxu_glCall_Assert(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
        oxu_glCall_Assert(glClearColor(0.f, 0.f, 0.f, 1.0f));

        c_shaders.emplace_back("../src/oXu/graphics/opengl/shaders/tex.glsl");

        return true;
    }

    void Backend::destroy()
    {
        SDL_GL_DeleteContext(c_GL_context);
    }

    void Backend::clear()
    {
        glClear(GL_COLOR_BUFFER_BIT);
    }

    void Backend::render(SDL_Window *game_window)
    {
        SDL_GL_SwapWindow(game_window);
    }

    void Backend::copy_texture(
        const Vector2<float> &position,
        const Vector2<float> &size,
        const graphics::Texture &tex
    )
    {
        float normX = getNormalizedScreenCoord(position.x, true);
        float normY = getNormalizedScreenCoord(position.y + size.y, false) * SCREEN_START_COORD;

        float normXW = getNormalizedScreenCoord(position.x + size.x, true);
        float normYH = getNormalizedScreenCoord(position.y, false) * SCREEN_START_COORD;

        float vbData[] = {
            normX, normY, 0.0f, 0.0f,
            normXW, normY, 1.0f, 0.0f,
            normXW, normYH, 1.0f, 1.0f,
            normX, normYH, 0.0f, 1.0f
        };

        static unsigned int indices[] = {
            0, 1, 2,
            2, 3, 0
        };

        const opengl::VertexArrayObject &vao = tex.getGLTexture()->getVao();
        vao.bind();
        vao.modifyVertexBuffer<float>(vbData, 16);
        tex.getGLTexture()->bind(0);
        
        c_shaders[Shaders::TEXTURE].bind();
        c_shaders[Shaders::TEXTURE].setUniform1i("u_textureSlot", 0);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, indices);
    }
}