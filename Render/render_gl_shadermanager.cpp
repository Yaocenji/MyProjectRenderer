#include "render_gl_shadermanager.h"

namespace Render {

ShaderManager::ShaderManager(QObject *parent) : QObject{parent} {
    // 初始化屏幕shader
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

    // 初始化shader
    vert = new QOpenGLShader(QOpenGLShader::Vertex, this);
    vert->compileSourceFile(
        ":/FowardRenderingShaders/FowardRendering/vert.vert");

    vert_color_frag = new QOpenGLShader(QOpenGLShader::Fragment, this);
    vert_color_frag->compileSourceFile(
        ":/FowardRenderingShaders/FowardRendering/vertColor.frag");

    vert_color_prog = new QOpenGLShaderProgram(this);
    vert_color_prog->create();
    vert_color_prog->addShader(vert);
    vert_color_prog->addShader(vert_color_frag);
    vert_color_prog->link();

    // 初始化基础色无光照shader
    diffuseUnlit_frag = new QOpenGLShader(QOpenGLShader::Fragment, this);
    diffuseUnlit_frag->compileSourceFile(
        ":/FowardRenderingShaders/FowardRendering/diffuseUnlit.frag");

    diffuseUnlit_proj = new QOpenGLShaderProgram(this);
    diffuseUnlit_proj->create();
    diffuseUnlit_proj->addShader(vert);
    diffuseUnlit_proj->addShader(diffuseUnlit_frag);
    diffuseUnlit_proj->link();

    // 初始化兰伯特shader
    diffuseLambert_frag = new QOpenGLShader(QOpenGLShader::Fragment, this);
    diffuseLambert_frag->compileSourceFile(
        ":/FowardRenderingShaders/FowardRendering/diffuseLambert.frag");

    diffuseLambert_proj = new QOpenGLShaderProgram(this);
    diffuseLambert_proj->create();
    diffuseLambert_proj->addShader(vert);
    diffuseLambert_proj->addShader(diffuseLambert_frag);
    diffuseLambert_proj->link();

    // 初始化pbrshader
    disneyBRDF_frag = new QOpenGLShader(QOpenGLShader::Fragment, this);
    disneyBRDF_frag->compileSourceFile(
        ":/FowardRenderingShaders/FowardRendering/DisneyBRDF_PBR.frag");

    disneyBRDF_proj = new QOpenGLShaderProgram(this);
    disneyBRDF_proj->create();
    disneyBRDF_proj->addShader(vert);
    disneyBRDF_proj->addShader(disneyBRDF_frag);
    disneyBRDF_proj->link();
}

} // namespace Render
