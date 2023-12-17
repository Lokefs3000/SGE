#include "pch.h"
#include "RenderResources.h"

#include <algorithm>

#include <glad/gl.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <ft2build.h>
#include FT_FREETYPE_H

#include "DebugLogger.h"
#include "Shader.h"

RenderResources::~RenderResources()
{
    for (int i = 0; i < m_ResourceKeys.size(); i++)
    {
        switch (m_ResourceValues[i]->resourceType)
        {
        case 0:
            deleteCanvas(m_ResourceKeys[i]);
            break;
        case 2:
            deleteTexture(m_ResourceKeys[i]);
            break;
        default:
            break;
        }
    }

    m_ResourceMap.clear();
}

xg::Guid& RenderResources::createCanvas(int width, int height)
{
    DEBUGLOG("Creating canvas with dimensions of: ", width, "x", height);

    RenderCanvasResource* actual = new RenderCanvasResource();

    actual->width = width;
    actual->height = height;
    actual->resourceType = 0;

    glGenTextures(1, &actual->id);
    glBindTexture(GL_TEXTURE_2D, actual->id);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

    //////////////////////////////////////////////////////////////////////////////////////////////////

    glGenFramebuffers(1, &actual->fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, actual->fbo);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, actual->id, 0);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    xg::Guid guid = xg::newGuid();
    actual->guid = guid;

    m_ResourceKeys.push_back(guid);
    m_ResourceValues.push_back(std::unique_ptr<RenderResource>(actual));

    return actual->guid;
}

void RenderResources::deleteCanvas(xg::Guid canvasId)
{
    DEBUGLOG("Attempting to delete canvas with id: ", canvasId.str());

    auto idx = std::find(m_ResourceKeys.begin(), m_ResourceKeys.end(), canvasId);
    if (idx != m_ResourceKeys.end()) {
        int idxInt = idx - m_ResourceKeys.begin();
        RenderResource* res = m_ResourceValues[idxInt].get();

        if (res->resourceType == 0) {
            RenderCanvasResource* aRes = (RenderCanvasResource*)res;

            glDeleteTextures(1, &aRes->id);
            glDeleteFramebuffers(1, &aRes->fbo);

            m_ResourceKeys.erase(m_ResourceKeys.begin() + idxInt);
            m_ResourceValues.erase(m_ResourceValues.begin() + idxInt);
        }
    }
}

xg::Guid& RenderResources::createShader(const char* vertex, const char* fragment)
{
    DEBUGLOG("Creating shader");

    RenderShaderResource* actual = new RenderShaderResource();

    actual->resourceType = 1;

    xg::Guid guid = xg::newGuid();
    actual->shader = std::make_shared<Shader>(vertex, fragment);

    actual->guid = guid;

    m_ResourceKeys.push_back(guid);
    m_ResourceValues.push_back(std::unique_ptr<RenderResource>(actual));

    return actual->guid;
}

void RenderResources::deleteShader(xg::Guid shaderId)
{
    DEBUGLOG("Attempting to delete shader with id: ", shaderId.str());

    auto idx = std::find(m_ResourceKeys.begin(), m_ResourceKeys.end(), shaderId);
    if (idx != m_ResourceKeys.end()) {
        int idxInt = idx - m_ResourceKeys.begin();
        RenderResource* res = m_ResourceValues[idxInt].get();

        if (res->resourceType == 1) {
            RenderShaderResource* aRes = (RenderShaderResource*)res;

            m_ResourceKeys.erase(m_ResourceKeys.begin() + idxInt);
            m_ResourceValues.erase(m_ResourceValues.begin() + idxInt);
        }
    }
}

xg::Guid& RenderResources::createTexture(const char* path)
{
    DEBUGLOG("Creating texture at location: ", path);

    RenderTextureResource* actual = new RenderTextureResource();

    int channels = 0;
    stbi_uc* pixels = stbi_load(path, &actual->width, &actual->height, &channels, 0);

    actual->resourceType = 2;

    uint32_t channelsgl;

    switch (channels)
    {
    case 1:
        channelsgl = GL_RED;
        break;
    case 2:
        channelsgl = GL_RG;
        break;
    case 3:
        channelsgl = GL_RGB;
        break;
    case 4:
        channelsgl = GL_RGBA;
        break;
    default:
        channelsgl = GL_RGB;
        break;
    }

    glGenTextures(1, &actual->id);
    glBindTexture(GL_TEXTURE_2D, actual->id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexImage2D(GL_TEXTURE_2D, 0, channelsgl, actual->width, actual->height, 0, channelsgl, GL_UNSIGNED_BYTE, pixels);

    stbi_image_free(pixels);

    xg::Guid guid = xg::newGuid();
    actual->guid = guid;

    m_ResourceKeys.push_back(guid);
    m_ResourceValues.push_back(std::unique_ptr<RenderResource>(actual));

    return actual->guid;
}

void RenderResources::deleteTexture(xg::Guid textureId)
{
    DEBUGLOG("Attempting to delete texture with id: ", textureId.str());

    auto idx = std::find(m_ResourceKeys.begin(), m_ResourceKeys.end(), textureId);
    if (idx != m_ResourceKeys.end()) {
        int idxInt = idx - m_ResourceKeys.begin();
        RenderResource* res = m_ResourceValues[idxInt].get();

        if (res->resourceType == 2) {
            RenderTextureResource* aRes = (RenderTextureResource*)res;

            glDeleteTextures(1, &aRes->id);

            m_ResourceKeys.erase(m_ResourceKeys.begin() + idxInt);
            m_ResourceValues.erase(m_ResourceValues.begin() + idxInt);
        }
    }
}

xg::Guid& RenderResources::createFont(const char* path)
{
    DEBUGLOG("Creating font at location: ", path);

    RenderFontResource* actual = new RenderFontResource();
    actual->resourceType = 3;

    ////////////////////////////////////

    FT_Library ft;
    if (FT_Init_FreeType(&ft))
    {
        std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
    }

    FT_Face face;
    if (FT_New_Face(ft, path, 0, &face))
    {
        std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
    }

    FT_Set_Pixel_Sizes(face, 0, 48);

    uint32_t w = 0;
    uint32_t h = 0;

    FT_GlyphSlot gslot = face->glyph;
    for (size_t i = 32; i < 128; i++)
    {
        if (FT_Load_Char(face, i, FT_LOAD_RENDER)) {
            continue;
        }

        w += gslot->bitmap.width;
        h = std::max(h, gslot->bitmap.rows);
    }

    int atlasw = w;

    glGenTextures(1, &actual->id);
    glBindTexture(GL_TEXTURE_2D, actual->id);

    int align = 0;
    glGetIntegerv(GL_UNPACK_ALIGNMENT, &align);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, w, h, 0, GL_RED, GL_UNSIGNED_BYTE, NULL);

    uint32_t x = 0;
    for (size_t i = 32; i < 128; i++)
    {
        if (FT_Load_Char(face, i, FT_LOAD_RENDER)) {
            continue;
        }

        glTexSubImage2D(GL_TEXTURE_2D, 0, x, 0, gslot->bitmap.width, gslot->bitmap.rows, GL_RED, GL_UNSIGNED_BYTE, gslot->bitmap.buffer);
        x += gslot->bitmap.width;

        RenderFontGlyph& glyph = actual->glyphs[i - 32];

        glyph.width = gslot->bitmap.width;
        glyph.height = gslot->bitmap.rows;
        glyph.offsetX = gslot->bitmap_left;
        glyph.offsetY = gslot->bitmap_top;
        glyph.advance = gslot->advance.x;

        glyph.uv1 = glm::vec2(x / (float)w, 0.0f);
        glyph.uv2 = glm::vec2(glyph.uv1.x + glyph.width / (float)w, glyph.height / (float)glyph.height);
    }

    glPixelStorei(GL_UNPACK_ALIGNMENT, align);

    FT_Done_Face(face);
    FT_Done_FreeType(ft);

    ////////////////////////////////////

    xg::Guid guid = xg::newGuid();
    actual->guid = guid;

    m_ResourceKeys.push_back(guid);
    m_ResourceValues.push_back(std::unique_ptr<RenderResource>(actual));

    return actual->guid;
}

void RenderResources::deleteFont(xg::Guid fontId)
{
    DEBUGLOG("Attempting to delete font with id: ", fontId.str());

    auto idx = std::find(m_ResourceKeys.begin(), m_ResourceKeys.end(), fontId);
    if (idx != m_ResourceKeys.end()) {
        int idxInt = idx - m_ResourceKeys.begin();
        RenderResource* res = m_ResourceValues[idxInt].get();

        if (res->resourceType == 3) {
            RenderFontResource* aRes = (RenderFontResource*)res;

            glDeleteTextures(1, &aRes->id);

            m_ResourceKeys.erase(m_ResourceKeys.begin() + idxInt);
            m_ResourceValues.erase(m_ResourceValues.begin() + idxInt);
        }
    }
}

RenderCanvasResource* RenderResources::getCanvas(xg::Guid canvasId)
{
    auto idx = std::find(m_ResourceKeys.begin(), m_ResourceKeys.end(), canvasId);
    if (idx != m_ResourceKeys.end()) {
        int idxInt = idx - m_ResourceKeys.begin();
        RenderResource* res = m_ResourceValues[idxInt].get();

        if (res->resourceType == 0) {
            RenderCanvasResource* aRes = (RenderCanvasResource*)res;
            return aRes;
        }
    }

    return nullptr;
}

RenderShaderResource* RenderResources::getShader(xg::Guid shaderId)
{
    auto idx = std::find(m_ResourceKeys.begin(), m_ResourceKeys.end(), shaderId);
    if (idx != m_ResourceKeys.end()) {
        int idxInt = idx - m_ResourceKeys.begin();
        RenderResource* res = m_ResourceValues[idxInt].get();

        if (res->resourceType == 1) {
            RenderShaderResource* aRes = (RenderShaderResource*)res;
            return aRes;
        }
    }

    return nullptr;
}

RenderTextureResource* RenderResources::getTexture(xg::Guid textureId)
{
    auto idx = std::find(m_ResourceKeys.begin(), m_ResourceKeys.end(), textureId);
    if (idx != m_ResourceKeys.end()) {
        int idxInt = idx - m_ResourceKeys.begin();
        RenderResource* res = m_ResourceValues[idxInt].get();

        if (res->resourceType == 2) {
            RenderTextureResource* aRes = (RenderTextureResource*)res;
            return aRes;
        }
    }

    return nullptr;
}

RenderFontResource* RenderResources::getFont(xg::Guid fontId)
{
    auto idx = std::find(m_ResourceKeys.begin(), m_ResourceKeys.end(), fontId);
    if (idx != m_ResourceKeys.end()) {
        int idxInt = idx - m_ResourceKeys.begin();
        RenderResource* res = m_ResourceValues[idxInt].get();

        if (res->resourceType == 3) {
            RenderFontResource* aRes = (RenderFontResource*)res;
            return aRes;
        }
    }

    return nullptr;
}
