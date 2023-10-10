#include "render_gl_realtime_raster_renderer.h"

namespace Render {

GL_Realtime_Raster_Renderer::GL_Realtime_Raster_Renderer(QWidget *parent)
    : QOpenGLWidget{parent} {
    screenRT = nullptr;
    screenMesh = nullptr;

    camera0 = nullptr;
    model = nullptr;

    mainLight = nullptr;

    cornellBox = nullptr;
}

void GL_Realtime_Raster_Renderer::initializeGL() {
    // 首先初始化OpenGL函数
    initializeOpenGLFunctions();
    // 同步上下文
    globalgl::thisContext = this;

    shaders = new ShaderManager(this);

    // 初始化屏幕shader
    screenRT = new RenderTexture(this, false);
    screenRT->recreateRenderTexture(width(), height(), *(this));

    // screenMesh是屏幕空间的平面
    screenMesh = new Mesh(this);

    Vertex *lb = new Vertex(-1.0f, -1.0f, 0.0f, 0, false, false, false);
    screenMesh->addVertex(lb);

    Vertex *rb = new Vertex(1.0f, -1.0f, 0.0f, 0, false, false, false);
    screenMesh->addVertex(rb);

    Vertex *lt = new Vertex(-1.0f, 1.0f, 0.0f, 0, false, false, false);
    screenMesh->addVertex(lt);

    Vertex *rt = new Vertex(1.0f, 1.0f, 0.0f, 0, false, false, false);
    screenMesh->addVertex(rt);

    screenMesh->addTriangle(0, 3, 2);
    screenMesh->addTriangle(0, 1, 3);

    screenMesh->synchronizeGLObjects();

    // 初始化摄像机
    model = new QMatrix4x4();
    model->setToIdentity();

    camera0 = new Camera(this);

    // 初始化光源
    mainLight = new ParallelLight(this);

    // 读取康奈尔盒
    cornellBox = new Model(
        "E:/Qt/QtProjects/MyProjectRenderer/testModelsAndTextures/"
        "CornellBox_2_test.fbx",
        this);
    cornellBox->synchronizeGLObjects(this);

    qDebug() << glGetError();
}

void GL_Realtime_Raster_Renderer::resizeGL(int w, int h) {
    makeCurrent();
    globalgl::thisContext = this;
    // 对屏幕空间的RT缩放
    screenRT->recreateRenderTexture(w, h, *this);
    // 摄像机设置
    camera0->setAspectRatio(float(w) / float(h));
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

    screenRT->bind(*this);
    //    glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebufferObject());
    glClear(GL_COLOR_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    shaders->diffuseUnlit_proj->bind();
    shaders->diffuseUnlit_proj->setUniformValue("model", *model);
    shaders->diffuseUnlit_proj->setUniformValue("view", camera0->matrixView());
    shaders->diffuseUnlit_proj->setUniformValue("proj",
                                                camera0->matrixProjection());
    mainLight->bind(shaders->diffuseUnlit_proj);
    cornellBox->draw(shaders->diffuseUnlit_proj, this);

    // 帧缓冲搬运
    glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebufferObject());
    glClear(GL_COLOR_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);

    shaders->screenShaderProgram->bind();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, screenRT->colorTexture());

    shaders->screenShaderProgram->setUniformValue("screenRT", 0);

    screenMesh->draw(shaders->screenShaderProgram, this);
}

} // namespace Render
