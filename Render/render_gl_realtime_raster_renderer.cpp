#include "render_gl_realtime_raster_renderer.h"

namespace Render {

GL_Realtime_Raster_Renderer::GL_Realtime_Raster_Renderer(QWidget *parent)
    : QOpenGLWidget{parent} {
    screenRT = nullptr;
    screenMesh = nullptr;

    camera0 = nullptr;
    model = nullptr;

    mainLight = nullptr;
    pointLight0 = nullptr;
    pointLight1 = nullptr;

    cornellBox = nullptr;
}

void GL_Realtime_Raster_Renderer::initializeGL() {
    // 首先初始化OpenGL函数
    initializeOpenGLFunctions();
    // 同步上下文
    globalgl::thisContext = this;

    shaders = new ShaderManager(this);

    // 初始化屏幕shader
    screenRT = new RenderTexture(this, true);
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
    //    camera0->setClipPlanes(0.01, 3.0);

    // 初始化光源
    mainLight = new ParallelLight(this);
    mainLight->setStrength(20.0f);

    pointLight0 = new PointLight(this);
    pointLight0->setPos(QVector3D(-0.4, 0.4, 0.4));
    pointLight0->setStrength(5.0f);

    pointLight1 = new PointLight(this);
    pointLight1->setPos(QVector3D(0.0, 0.4, 0.0));
    pointLight1->setStrength(20.0f);

    //    qDebug() << glGetError();

    // 读取康奈尔盒
    cornellBox = new Model(
        "E:/Qt/QtProjects/MyProjectRenderer/testModelsAndTextures/"
        "CornellBox_2.fbx",
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

    /**
     *  GL准备
     */
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    screenRT->clear(*this);
    screenRT->bind(*this);
    //    glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebufferObject());
    glClear(GL_COLOR_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    shaders->disneyBRDF_proj->bind();

    // 摄像机设置
    shaders->disneyBRDF_proj->setUniformValue("model", *model);
    shaders->disneyBRDF_proj->setUniformValue("view", camera0->matrixView());
    shaders->disneyBRDF_proj->setUniformValue("proj",
                                              camera0->matrixProjection());

    shaders->disneyBRDF_proj->setUniformValue("model_inverse",
                                              model->inverted());
    shaders->disneyBRDF_proj->setUniformValue("model_inverse_transpose",
                                              model->inverted().transposed());
    shaders->disneyBRDF_proj->setUniformValue("view_inverse",
                                              camera0->matrixView().inverted());
    shaders->disneyBRDF_proj->setUniformValue(
        "proj_inverse", camera0->matrixProjection().inverted());

    shaders->disneyBRDF_proj->setUniformValue("near", camera0->Near());
    shaders->disneyBRDF_proj->setUniformValue("far", camera0->Far());

    shaders->disneyBRDF_proj->setUniformValue("cameraPos",
                                              camera0->cameraPos());

    // 灯光设置

    //    mainLight->bind(shaders->disneyBRDF_proj);

    pointLight0->bind(shaders->disneyBRDF_proj, 0);
    pointLight1->bind(shaders->disneyBRDF_proj, 1);
    shaders->disneyBRDF_proj->setUniformValue("usedPointLightNumber", 2);

    // 正式绘制
    cornellBox->draw(shaders->disneyBRDF_proj, this);

    // 帧缓冲搬运
    glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebufferObject());
    glClear(GL_COLOR_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);

    shaders->screenShaderProgram->bind();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, screenRT->colorTexture());

    shaders->screenShaderProgram->setUniformValue("screenRT", 0);

    // 是否进行hdr映射和gamma变换
    shaders->screenShaderProgram->setUniformValue("isDebug", false);

    screenMesh->draw(shaders->screenShaderProgram, this);
}

} // namespace Render
