#include "render_rendertexture.h"

namespace Render {

RenderTexture::RenderTexture(QObject *parent, bool hasCol, bool hasDep)
    : QObject{parent}, hasColorTexture(hasCol), hasDepthBuffer(hasDep) {
    frameBuffer = 0;
    colorBuffer = 0;
    depthBuffer = 0;
}

RenderTexture::~RenderTexture() {
    deleteRenderTexture();
}

void RenderTexture::deleteRenderTexture(QOpenGLFunctions_4_5_Core &f) {
    if (frameBuffer != 0) {
        // 绑定fbo
        f.glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);

        if (hasColorTexture) {
            if (colorBuffer != 0) {
                // 解绑texture
                f.glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                                         GL_TEXTURE_2D, 0, 0);
                f.glDeleteTextures(1, &colorBuffer);
                colorBuffer = 0;
            }
        }
        if (hasDepthBuffer) {
            if (depthBuffer != 0) {
                // 解绑texture

                f.glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                                         GL_TEXTURE_2D, 0, 0);
                f.glDeleteTextures(1, &depthBuffer);
                //                f.glFramebufferRenderbuffer(GL_FRAMEBUFFER,
                //                                            GL_DEPTH_STENCIL_ATTACHMENT,
                //                                            GL_RENDERBUFFER,
                //                                            0);
                //                f.glDeleteRenderbuffers(1, &depthBuffer);
                depthBuffer = 0;
            }
        }
        f.glDeleteFramebuffers(1, &frameBuffer);
        frameBuffer = 0;
    }
}

void RenderTexture::recreateRenderTexture(int width, int height,
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
        f.glBindTexture(GL_TEXTURE_2D, colorBuffer);
        // 1、作为颜色时的HDR；2、作为G-BUFFER时的
        f.glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA,
                       GL_FLOAT, NULL);
        f.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        f.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        f.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
        f.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
        f.glBindTexture(GL_TEXTURE_2D, 0);
    }

    if (hasDepthBuffer) {
        // 生成第二张：depth buffer

        f.glGenTextures(1, &depthBuffer);
        f.glBindTexture(GL_TEXTURE_2D, depthBuffer);
        f.glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, width, height, 0,
                       GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
        f.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        f.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        f.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
        f.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
        f.glBindTexture(GL_TEXTURE_2D, 0);

        /*
                f.glGenRenderbuffers(1, &depthBuffer);
                f.glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
                f.glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8,
           width, height); f.glBindRenderbuffer(GL_RENDERBUFFER, 0);*/
    }

    // 绑定两张贴图
    if (hasColorTexture) {
        f.glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                                 GL_TEXTURE_2D, colorBuffer, 0);
    }
    if (hasDepthBuffer) {
        f.glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                                 GL_TEXTURE_2D, depthBuffer, 0);
        //        f.glFramebufferRenderbuffer(GL_FRAMEBUFFER,
        //        GL_DEPTH_STENCIL_ATTACHMENT,
        //                                    GL_RENDERBUFFER, depthBuffer);
    }

    if (f.glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        qDebug() << "ERROR::FRAMEBUFFER:: Framebuffer is not "
                    "complete!";

    // 绑回之前的frame buffer
    f.glBindFramebuffer(GL_FRAMEBUFFER, thisFrameBuffer);
}

void RenderTexture::bind(QOpenGLFunctions_4_5_Core &f) {
    f.glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
}

void RenderTexture::clear(QOpenGLFunctions_4_5_Core &f) {
    // 获取当前的framebuffer
    GLint thisFrameBuffer = 0;
    f.glGetIntegerv(GL_FRAMEBUFFER_BINDING, &thisFrameBuffer);

    f.glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
    f.glClearColor(globalrender::backgroundColor.redF(),
                   globalrender::backgroundColor.greenF(),
                   globalrender::backgroundColor.blueF(), 1);
    f.glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //    f.glEnable(GL_DEPTH_TEST);
    //    f.glDisable(GL_BLEND);

    // 绑回之前的frame buffer
    f.glBindFramebuffer(GL_FRAMEBUFFER, thisFrameBuffer);
}

GLuint RenderTexture::colorTexture() {
    if (!hasColorTexture) {
        qDebug() << "ERROR: this RenderTexture do not has color texture, but "
                    "the App "
                    "tries getting it.";
    }
    return colorBuffer;
}

GLuint RenderTexture::depthTexture() {
    if (!hasDepthBuffer) {
        qDebug()
            << "ERROR: this RenderTexture do not has depth buffer, but the App "
               "tries getting it.";
    }
    return depthBuffer;
}

} // namespace Render
