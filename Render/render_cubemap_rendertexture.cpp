#include "render_cubemap_rendertexture.h"

namespace Render {

CubeMapRT::CubeMapRT(QObject *parent, bool hasCol, bool hasDep)
    : QObject{parent}, hasColorTexture(hasCol), hasDepthBuffer(hasDep) {
    frameBuffer = 0;
    colorBuffer = 0;
    depthBuffer = 0;
}

CubeMapRT::~CubeMapRT() {
    deleteRenderTexture();
}

void CubeMapRT::deleteRenderTexture(QOpenGLFunctions_4_5_Core &f) {
    if (frameBuffer != 0) {
        // 绑定fbo
        f.glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);

        if (hasColorTexture) {
            if (colorBuffer != 0) {
                // 解绑texture
                f.glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, 0,
                                       0);
                f.glDeleteTextures(1, &colorBuffer);
                colorBuffer = 0;
            }
        }
        if (hasDepthBuffer) {
            if (depthBuffer != 0) {
                // 解绑texture

                f.glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, 0,
                                       0);
                f.glDeleteTextures(1, &depthBuffer);
                depthBuffer = 0;
            }
        }
        f.glDeleteFramebuffers(1, &frameBuffer);
        frameBuffer = 0;
    }
}

void CubeMapRT::recreateRenderTexture(int width, int height,
                                      QOpenGLFunctions_4_5_Core &f) {
    // 获取当前的framebuffer
    GLint thisFrameBuffer = 0;
    f.glGetIntegerv(GL_FRAMEBUFFER_BINDING, &thisFrameBuffer);

    // 先删除已有的buffer
    deleteRenderTexture(f);

    // 生成frame buffer
    f.glGenFramebuffers(1, &frameBuffer);
    // 绑定frame buffer
    f.glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);

    if (hasColorTexture) {
        // 生成第一张：color buffer
        f.glGenTextures(1, &colorBuffer);
        f.glBindTexture(GL_TEXTURE_CUBE_MAP, colorBuffer);
        for (int i = 0; i < 6; ++i) {
            f.glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA32F,
                           width, height, 0, GL_RGBA, GL_FLOAT, NULL);
        }
        f.glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER,
                          GL_LINEAR);
        f.glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER,
                          GL_LINEAR);
        f.glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S,
                          GL_CLAMP_TO_EDGE);
        f.glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T,
                          GL_CLAMP_TO_EDGE);
        f.glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R,
                          GL_CLAMP_TO_EDGE);
        f.glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    }

    if (hasDepthBuffer) {
        // 生成第二张：depth buffer

        f.glGenTextures(1, &depthBuffer);
        f.glBindTexture(GL_TEXTURE_CUBE_MAP, depthBuffer);
        for (int i = 0; i < 6; ++i) {
            f.glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0,
                           GL_DEPTH_COMPONENT32, width, height, 0,
                           GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
        }
        f.glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER,
                          GL_LINEAR);
        f.glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER,
                          GL_LINEAR);
        f.glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S,
                          GL_CLAMP_TO_EDGE);
        f.glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T,
                          GL_CLAMP_TO_EDGE);
        f.glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R,
                          GL_CLAMP_TO_EDGE);
        f.glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    }

    // 绑定两张贴图
    if (hasColorTexture) {
        f.glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                               colorBuffer, 0);
    }
    if (hasDepthBuffer) {
        f.glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthBuffer,
                               0);
    }

    if (f.glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        qDebug() << "ERROR::FRAMEBUFFER:: Framebuffer is not "
                    "complete!";

    // 绑回之前的frame buffer
    f.glBindFramebuffer(GL_FRAMEBUFFER, thisFrameBuffer);
}

void CubeMapRT::bind(QOpenGLFunctions_4_5_Core &f) {
    f.glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
}

void CubeMapRT::clear(QOpenGLFunctions_4_5_Core &f) {
    // 获取当前的framebuffer
    GLint thisFrameBuffer = 0;
    f.glGetIntegerv(GL_FRAMEBUFFER_BINDING, &thisFrameBuffer);

    f.glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
    f.glClearColor(globalrender::backgroundColor.redF(),
                   globalrender::backgroundColor.greenF(),
                   globalrender::backgroundColor.blueF(), 1);
    f.glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // 绑回之前的frame buffer
    f.glBindFramebuffer(GL_FRAMEBUFFER, thisFrameBuffer);
}

GLuint CubeMapRT::colorTexture() {
    if (!hasColorTexture) {
        qDebug() << "ERROR: this RenderTexture do not has color texture, but "
                    "the App "
                    "tries getting it.";
    }
    return colorBuffer;
}

GLuint CubeMapRT::depthTexture() {
    if (!hasDepthBuffer) {
        qDebug()
            << "ERROR: this RenderTexture do not has depth buffer, but the App "
               "tries getting it.";
    }
    return depthBuffer;
}

} // namespace Render
