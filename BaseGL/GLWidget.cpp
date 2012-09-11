#include "GLWidget.h"
#include <QtCore/qcoreapplication.h>
#include <iostream>
#include <math.h>
#include <gli/gli.hpp>
#include <gli/gtx/gl_texture2d.hpp>

GLWidget::GLWidget(const QGLFormat& format, QWidget* parent)
  : QGLWidget(format, parent), _log(get_global_log())
{
  makeCurrent();
  glewExperimental= GL_TRUE;
  GLenum err = glewInit();
  if(err != GLEW_OK)
    exit(1);
  setAutoBufferSwap(true);
  _timer = new QTimer(this);
  _elapsed.start();
}

void GLWidget::DumpGLInfo(bool dump_extentions)
{
  const GLubyte* renderer = glGetString(GL_RENDERER);
  const GLubyte* vendor = glGetString(GL_VENDOR);
  const GLubyte* version = glGetString(GL_VERSION);
  const GLubyte* glsl_version = glGetString(GL_SHADING_LANGUAGE_VERSION);

  GLint major, minor;
  glGetIntegerv(GL_MAJOR_VERSION, &major);
  glGetIntegerv(GL_MINOR_VERSION, &minor);

  BOOST_LOG_SEV(_log, info) << "GL Vendor:             " << vendor;
  BOOST_LOG_SEV(_log, info) << "GL Renderer:           " << renderer;
  BOOST_LOG_SEV(_log, info) << "GL Version (string):   " << version;
  BOOST_LOG_SEV(_log, info) << "GL Version (integer):  " << major << "." << minor;
  BOOST_LOG_SEV(_log, info) << "GLSL Version:          " << glsl_version;

  if(dump_extentions)
  {
    GLint num;
    glGetIntegerv(GL_NUM_EXTENSIONS, &num);
    for(int i = 0; i < num; ++i)
      BOOST_LOG_SEV(_log, info) << glGetStringi(GL_EXTENSIONS, i);
  }
}

void GLWidget::initializeGL()
{
  glGenVertexArrays = (_glGenVertexArrays) QGLWidget::context()->getProcAddress("glGenVertexArrays");
  glBindVertexArray = (_glBindVertexArray) QGLWidget::context()->getProcAddress("glBindVertexArray");

  DumpGLInfo(false);

  connect(_timer, SIGNAL(timeout()), this, SLOT(Idle()));
  _timer->start(0);

  _model = glm::mat4(1.0f);
  _view = glm::lookAt(vec3(0.0f, 0.0f, 2.0f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
  _projection = mat4(1.0f);

}

void GLWidget::resizeGL( int w, int h )
{
    // Set the viewport to window dimensions
    glViewport( 0, 0, w, qMax( h, 1 ) );
}

void GLWidget::paintGL()
{
  glClearColor(0.0, 0.0, 0.0, 1.0);
  glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
}

void GLWidget::keyPressEvent( QKeyEvent* e )
{
    switch ( e->key() )
    {
        case Qt::Key_Escape:
            QCoreApplication::instance()->quit();
            break;
 
        default:
            QGLWidget::keyPressEvent( e );
    }
}

void GLWidget::FreeResources()
{
}

GLWidget::~GLWidget()
{
  FreeResources();
}

void GLWidget::Idle()
{
  updateGL();
}

void GLWidget::CompileAndLinkShaders()
{
  //first do vertex files
  for(ShaderFiles::iterator it = _vertex_shader_files.begin(); it != _vertex_shader_files.end(); ++it)
  {
    if(!_program.CompileShaderFromFile(it->c_str(), GLSLShader::VERTEX))
    {
      BOOST_LOG_SEV(_log, error) << "Vertex Shader " << it->c_str() << " failed\n" << _program.Log() << std::endl << "Hit any key to exit";
      char c;
      std::cin >> c;
      exit(1);
    }
  }
  //fragment
  for(ShaderFiles::iterator it = _fragment_shader_files.begin(); it != _fragment_shader_files.end(); ++it)
  {
    if(!_program.CompileShaderFromFile(it->c_str(), GLSLShader::FRAGMENT))
    {
      BOOST_LOG_SEV(_log, error) << "Fragment Shader " << it->c_str() << " failed\n" << _program.Log() << std::endl << "Hit any key to exit";
      char c;
      std::cin >> c;
      exit(1);
    }
  }
  if(!_program.Link())
  {
    BOOST_LOG_SEV(_log, error) << "Program failed to link\n" << _program.Log() << std::endl << "Hit any key to exit";
    char c;
    std::cin >> c;
    exit(1);
  }
  _program.Use();
  _program.PrintActiveAttribs();
  _program.PrintActiveUniforms();

}

void GLWidget::SetMatrices()
{
  glm::mat4 mv = _view * _model;
  _program.SetUniform("model_view_matrix", mv);
  _program.SetUniform("mvp_matrix", _projection * mv);
}