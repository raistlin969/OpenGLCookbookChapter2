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

  if(!_program.CompileShaderFromFile("simple_vertex.glsl", GLSLShader::VERTEX))
  {
    BOOST_LOG_SEV(_log, error) << "Vertex Shader failed\n" << _program.Log() << std::endl << "Hit any key to exit";
    char c;
    std::cin >> c;
    exit(1);
  }
  if(!_program.CompileShaderFromFile("simple_frag.glsl", GLSLShader::FRAGMENT))
  {
    BOOST_LOG_SEV(_log, error) << "Fragment Shader failed\n" << _program.Log() << std::endl << "Hit any key to exit";
    char c;
    std::cin >> c;
    exit(1);
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

  GLuint block_index = glGetUniformBlockIndex(_program.Handle(), "BlobSettings");
  GLint block_size;
  glGetActiveUniformBlockiv(_program.Handle(), block_index, GL_UNIFORM_BLOCK_DATA_SIZE, &block_size);
  GLubyte* block_buffer = new GLubyte[block_size];
  const GLchar* names[] = {"inner_color", "outer_color", "inner_radius", "outer_radius"};
  GLuint indices[4];
  glGetUniformIndices(_program.Handle(), 4, names, indices);

  GLint offset[4];
  glGetActiveUniformsiv(_program.Handle(), 4, indices, GL_UNIFORM_OFFSET, offset);

  GLfloat outer_color[] = {0.0f, 0.0f, 0.0f, 0.0f};
  GLfloat inner_color[] = {1.0f, 1.0f, 0.75f, 1.0f};
  GLfloat inner_radius = 0.25f, outer_radius = 0.45f;

  memcpy(block_buffer + offset[0], inner_color, 4 * sizeof(GLfloat));
  memcpy(block_buffer + offset[1], outer_color, 4 * sizeof(GLfloat));
  memcpy(block_buffer + offset[2], &inner_radius, sizeof(GLfloat));
  memcpy(block_buffer + offset[3], &outer_radius, sizeof(GLfloat));

  GLuint ubo;
  glGenBuffers(1, &ubo);
  glBindBuffer(GL_UNIFORM_BUFFER, ubo);
  glBufferData(GL_UNIFORM_BUFFER, block_size, block_buffer, GL_DYNAMIC_DRAW);
  glBindBufferBase(GL_UNIFORM_BUFFER, block_index, ubo);

  float pos_data[] = 
  {
    -0.8f, -0.8f, 0.0f,
    0.8f, 0.8f, 0.0f,
    -0.8f, 0.8f, 0.0f,

    -0.8f, -0.8f, 0.0f,
    0.8f, -0.8f, 0.0f,
    0.8f, 0.8f, 0.0f
  };
  float tex_data[] = 
  {
    0.0f, 0.0f,
    1.0f, 1.0f,
    0.0f, 1.0f,

    0.0f, 0.0f,
    1.0f, 0.0f, 
    1.0f, 1.0f
  };

  GLuint vbo[2];
  glGenBuffers(2, vbo);
  GLuint pos_buffer = vbo[0];
  GLuint tex_buffer = vbo[1];

  glBindBuffer(GL_ARRAY_BUFFER, pos_buffer);
  glBufferData(GL_ARRAY_BUFFER, 18 * sizeof(float), pos_data, GL_STATIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, tex_buffer);
  glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(float), tex_data, GL_STATIC_DRAW);

  glGenVertexArrays(1, &_vao);
  glBindVertexArray(_vao);

  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);

  glBindBuffer(GL_ARRAY_BUFFER, pos_buffer);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

  glBindBuffer(GL_ARRAY_BUFFER, tex_buffer);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, NULL);

  _program.PrintActiveUniforms();
}

void GLWidget::resizeGL( int w, int h )
{
    // Set the viewport to window dimensions
    glViewport( 0, 0, w, qMax( h, 1 ) );
}

void GLWidget::paintGL()
{
  glClearColor(1.0, 1.0, 1.0, 1.0);
  glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

  //glm::mat4 r_mat = glm::rotate(glm::mat4(1.0f), 15.0f, glm::vec3(0.0f, 0.0f, 1.0f));
  //GLuint location = glGetUniformLocation(_program, "rotation_matrix");
  //if(location >= 0)
  //{
  //  glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(r_mat));
  //}
  
  glBindVertexArray(_vao);
  glDrawArrays(GL_TRIANGLES, 0, 6);
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