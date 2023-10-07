#include "render_gl_realtime_raster_renderer.h"

namespace Render {

GL_Realtime_Raster_Renderer::GL_Realtime_Raster_Renderer(QWidget *parent)
    : QOpenGLWidget{parent} {
    screenRT = nullptr;
    screenShaderVert = nullptr;
    screenShaderFrag = nullptr;
    screenShaderProgram = nullptr;
    screenMesh = nullptr;
}

void GL_Realtime_Raster_Renderer::initializeGL() {
    // 首先初始化OpenGL函数
    initializeOpenGLFunctions();
    // 同步上下文
    globalgl::thisContext = this;

    screenRT = new RenderTexture(this, false);
    screenRT->recreateRenderTexture(width(), height(), *(this));

    screenShaderVert = new QOpenGLShader(QOpenGLShader::Vertex, this);
    screenShaderVert->compileSourceFile(
        ":/ScreenShaders/ScreenSwap/ScreenVert.vert");
    screenShaderFrag = new QOpenGLShader(QOpenGLShader::Fragment, this);
    screenShaderFrag->compileSourceFile(
        ":/ScreenShaders/ScreenSwap/ScreenFrag.frag");
    screenShaderProgram = new QOpenGLShaderProgram(this);
    screenShaderProgram->create();
    screenShaderProgram->addShader(screenShaderVert);
    screenShaderProgram->addShader(screenShaderFrag);
    screenShaderProgram->link();

    screenMesh = new Mesh(this);
    Vertex *lb = new Vertex(-1.0f, -1.0f, 0.0f, 0, false, false);
    screenMesh->addVertex(lb);
    Vertex *rb = new Vertex(1.0f, -1.0f, 0.0f, 0, false, false);
    screenMesh->addVertex(rb);
    Vertex *lt = new Vertex(-1.0f, 1.0f, 0.0f, 0, false, false);
    screenMesh->addVertex(lt);
    Vertex *rt = new Vertex(1.0f, 1.0f, 0.0f, 0, false, false);
    screenMesh->addVertex(rt);
    screenMesh->addTriangle(0, 3, 2);
    screenMesh->addTriangle(0, 1, 3);

    screenMesh->synchronizeGLObjects();

    qDebug() << glGetError();
}

void GL_Realtime_Raster_Renderer::resizeGL(int w, int h) {
    makeCurrent();
    // 同步上下文
    globalgl::thisContext = this;
    screenRT->recreateRenderTexture(w, h, *this);
}

void GL_Realtime_Raster_Renderer::paintGL() {
    // 同步上下文
    globalgl::thisContext = this;

    // 准备离屏渲染
    //    PreRenderTerrainGround(swapFrameBuffer);

    /**
     *  GL准备
     */
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    //    qDebug() << glGetError();

    // 绘制1
    //    screenRT->bind();
    glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebufferObject());
    glClear(GL_COLOR_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);
    screenShaderProgram->bind();
    screenMesh->draw();
    //    qDebug() << glGetError();
}

} // namespace Render
