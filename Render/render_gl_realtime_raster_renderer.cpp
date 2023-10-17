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

    timer = new QTimer(this);
    timer->setTimerType(Qt::PreciseTimer);
    timer->setInterval(33);
    connect(timer, SIGNAL(timeout()), this, SLOT(TimerUpdate()));
}

void GL_Realtime_Raster_Renderer::initializeGL() {
    // 首先初始化OpenGL函数
    initializeOpenGLFunctions();
    // 同步上下文
    globalgl::thisContext = this;

    shaders = new ShaderManager(this);

    // 初始化屏幕shader
    screenRT = new RenderTexture(this, true, true);
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
    camera0->setClipPlanes(0.5, 3.0);
    //    camera0->rotate(0, 225);
    //    camera0->camlookPos = QVector3D(-0.1, -0.5, -0.1);

    // 初始化光源
    mainLight = new ParallelLight(this);
    mainLight->setStrength(20.0f);

    pointLight0 = new PointLight(this);
    pointLight0->setPos(QVector3D(-0.4, 0.4, 0.4));
    pointLight0->setStrength(0.0f);

    pointLight1 = new PointLight(this);
    pointLight1->setPos(QVector3D(0.4, 0, 0.4));
    pointLight1->setStrength(25.0f);
    // 6张顺序：X+X-Y+Y-Z+Z-；
    pointLight1->shadowMap->recreateRenderTexture(1024 * 6, 1024, *this);

    //    qDebug() << glGetError();

    // 读取康奈尔盒
    cornellBox = new Model(
        "E:/Qt/QtProjects/MyProjectRenderer/testModelsAndTextures/"
        "CornellBox_2_tesselled.fbx",
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
    // 开始计时器
    if (!timer->isActive()) timer->start();
    // 同步上下文
    globalgl::thisContext = this;

    /**
     *  GL准备
     */
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    /**
     * 阴影贴图渲染阶段
     */

    //    qDebug() << "阴影贴图" << glGetError();
    pointLight1->RenderShadow(cornellBox, this, shaders);

    /**
     * 离屏渲染阶段
     */

    glViewport(0, 0, width(), height());
    // 清空和绑定屏幕空间渲染纹理
    screenRT->clear(*this);
    screenRT->bind(*this);

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

    // 将pLight1的阴影贴图绑定到12位置
    // 将阴影贴图绑定到12+index纹理空间
    //    glActiveTexture(GL_TEXTURE12);
    //    glBindTexture(GL_TEXTURE_2D, pointLight1->shadowMap->depthTexture());
    //    shaders->disneyBRDF_proj->setUniformValue("pLight[1].ShadowMap", 12);
    //    shaders->disneyBRDF_proj->setUniformValue("pLight[1].hasShadow",
    //    true);

    // 正式绘制
    cornellBox->draw(shaders->disneyBRDF_proj, this);
    qDebug() << "离屏渲染" << glGetError();

    /**
     * 将渲染纹理的内容搬运到屏幕
     */

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
    qDebug() << "搬运" << glGetError();
}

void GL_Realtime_Raster_Renderer::TimerUpdate() {
    //    update();
    //    pointLight1->position.setY(pointLight1->position.y() + 0.005);
}

} // namespace Render
