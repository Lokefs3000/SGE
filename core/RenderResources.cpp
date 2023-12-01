#include "pch.h"
#include "RenderResources.h"

#include <glad/gl.h>

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

    auto find = std::find(m_ShaderKeys.begin(), m_ShaderKeys.end(), guid);
    int idx;
    if (find == m_ShaderKeys.end()) {
        idx = m_ShaderKeys.size();

        m_ShaderKeys.push_back(guid);
        m_ShaderValues.push_back(std::make_shared<Shader>(vertex, fragment));
    }
    else
        idx = find - m_ShaderKeys.begin();

   
    actual->shader = m_ShaderValues[idx];

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

            int idx = std::find(m_ShaderKeys.begin(), m_ShaderKeys.end(), shaderId) - m_ShaderKeys.begin();
            if (m_ShaderValues[idx].use_count() <= 1) {
                m_ShaderKeys.erase(m_ShaderKeys.begin() + idx);
                m_ShaderValues.erase(m_ShaderValues.begin() + idx);
            }
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
