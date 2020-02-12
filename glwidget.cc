// Author: Marc Comino 2019

#include <glwidget.h>

#include <fstream>
#include <iostream>
#include <memory>
#include <string>

#include "./volume.h"
#include "./volume_io.h"

namespace {

const float kFieldOfView = 60;
const float kZNear = 0.1;
const float kZFar = 10;

const char kVertexShaderFile[] = "../shaders/raycast.vert";
const char kFragmentShaderFile[] = "../shaders/raycast.frag";

const char kVertexShaderPointsFile[] = "../shaders/point.vert";
const char kFragmentShaderPointsFile[] = "../shaders/point.frag";

const int kVertexAttributeIdx = 0;
const int kNormalAttributeIdx = 1;

bool ReadFile(const std::string filename, std::string *shader_source) {
  std::ifstream infile(filename.c_str());

  if (!infile.is_open() || !infile.good()) {
    std::cerr << "Error " + filename + " not found." << std::endl;
    return false;
  }

  std::stringstream stream;
  stream << infile.rdbuf();
  infile.close();

  *shader_source = stream.str();
  return true;
}

}  // namespace

GLWidget::GLWidget(QWidget *parent)
    : QGLWidget(parent), initialized_(false), width_(0.0), height_(0.0) {
  setFocusPolicy(Qt::StrongFocus);

  light_position_ = glm::vec3(1, 1, 1);
  light_color_ = glm::vec3(1, 1, 1);
}

GLWidget::~GLWidget() {}

bool GLWidget::LoadVolume(const QString &path) {
  std::unique_ptr<data_representation::Volume> vol =
      std::make_unique<data_representation::Volume>();

  if (data_representation::ReadFromDicom(path.toUtf8().constData(),
                                         vol.get())) {
    vol_.reset(vol.release());
    camera_.UpdateModel(cube_->min_, cube_->max_);

    return true;
  }

  return false;
}

std::vector<double>& GLWidget::GetVolumeHistogram(){
    if (vol_ != nullptr) return vol_->histogram_;
}

void GLWidget::SetTransferFunction() {

    glBindTexture(GL_TEXTURE_1D, transfer_function_texture_id_);
    glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA32F, transfer_function_values_.size()/4, 0, GL_RGBA, GL_FLOAT, &transfer_function_values_[0]);

    /*
     * Perform this check since the transfer function widget could flood the glwidget with updateGL requests
     * as the graphs are dragged around
    */
    if (last_render_timestamp_ + 50 < QDateTime::currentMSecsSinceEpoch()){
        updateGL();
    }

}

void GLWidget::LightPosXValueChanged(double arg) {
    light_position_.x = arg;
    updateGL();
}

void GLWidget::LightPosYValueChanged(double arg){
    light_position_.y = arg;
    updateGL();
}
void GLWidget::LightPosZValueChanged(double arg){
    light_position_.z = arg;
    updateGL();
}

void GLWidget::LightColorXValueChanged(double arg){
    light_color_.x = arg;
    updateGL();
}
void GLWidget::LightColorYValueChanged(double arg){
    light_color_.y = arg;
    updateGL();
}
void GLWidget::LightColorZValueChanged(double arg){
    light_color_.z = arg;
    updateGL();
}

void GLWidget::SetPhongShadingCalc(bool arg){
    calc_phong_ = arg;
    updateGL();
}

void GLWidget::SetShadowsCalc(bool arg){
    calc_shadow_ = arg;
    updateGL();
}

void GLWidget::initializeGL() {
  glewInit();

  glEnable(GL_NORMALIZE);
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  glEnable(GL_DEPTH_TEST);

  std::string vertex_shader, fragment_shader;
  bool res = ReadFile(kVertexShaderFile, &vertex_shader) &&
             ReadFile(kFragmentShaderFile, &fragment_shader);

  std::string vertex_shader_point, fragment_shader_point;
  res = ReadFile(kVertexShaderPointsFile, &vertex_shader_point) &&
             ReadFile(kFragmentShaderPointsFile, &fragment_shader_point);

  if (!res) exit(0);

  cube_ = std::make_unique<data_representation::Cube>();
  program_ = std::make_unique<QOpenGLShaderProgram>();
  program_->addShaderFromSourceCode(QOpenGLShader::Vertex,
                                    vertex_shader.c_str());
  program_->addShaderFromSourceCode(QOpenGLShader::Fragment,
                                    fragment_shader.c_str());
  program_->bindAttributeLocation("vertex", kVertexAttributeIdx);
  program_->bindAttributeLocation("normal", kNormalAttributeIdx);
  program_->link();

  /* Initialize transfer function to zeros */
  transfer_function_values_ = std::vector<float>(256 * 4, 0.0f);
  glGenTextures(1, &transfer_function_texture_id_);
  glBindTexture(GL_TEXTURE_1D, transfer_function_texture_id_);
  /* Set border style to clamp */
  glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP);
  glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_T, GL_CLAMP);
  glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  /* Initialize point rendering shader */
  program_points_ = std::make_unique<QOpenGLShaderProgram>();
  program_points_->addShaderFromSourceCode(QOpenGLShader::Vertex, vertex_shader_point.c_str());
  program_points_->addShaderFromSourceCode(QOpenGLShader::Fragment, fragment_shader_point.c_str());
  program_points_->bindAttributeLocation("vertex", kVertexAttributeIdx);
  program_points_->link();
  glEnable(GL_PROGRAM_POINT_SIZE);

  glGenVertexArrays(1, &points_vao_);
  glBindVertexArray(0);

  initialized_ = true;
}

void GLWidget::resizeGL(int w, int h) {
  if (h == 0) h = 1;
  width_ = w;
  height_ = h;

  camera_.SetViewport(0, 0, w, h);
  camera_.SetProjection(kFieldOfView, kZNear, kZFar);
}

void GLWidget::mousePressEvent(QMouseEvent *event) {
  if (event->button() == Qt::LeftButton) {
    camera_.StartRotating(event->x(), event->y());
  }
  if (event->button() == Qt::RightButton) {
    camera_.StartZooming(event->x(), event->y());
  }
  updateGL();
}

void GLWidget::mouseMoveEvent(QMouseEvent *event) {
  camera_.SetRotationX(event->y());
  camera_.SetRotationY(event->x());
  camera_.SafeZoom(event->y());
  updateGL();
}

void GLWidget::mouseReleaseEvent(QMouseEvent *event) {
  if (event->button() == Qt::LeftButton) {
    camera_.StopRotating(event->x(), event->y());
  }
  if (event->button() == Qt::RightButton) {
    camera_.StopZooming(event->x(), event->y());
  }
  updateGL();
}

void GLWidget::keyPressEvent(QKeyEvent *event) {
  if (event->key() == Qt::Key_Up) camera_.Zoom(-1);
  if (event->key() == Qt::Key_Down) camera_.Zoom(1);

  if (event->key() == Qt::Key_Left) camera_.Rotate(-1);
  if (event->key() == Qt::Key_Right) camera_.Rotate(1);

  if (event->key() == Qt::Key_W) camera_.Zoom(-1);
  if (event->key() == Qt::Key_S) camera_.Zoom(1);

  if (event->key() == Qt::Key_A) camera_.Rotate(-1);
  if (event->key() == Qt::Key_D) camera_.Rotate(1);

  if (event->key() == Qt::Key_R) {
    std::string vertex_shader, fragment_shader;
    bool res = ReadFile(kVertexShaderFile, &vertex_shader) &&
               ReadFile(kFragmentShaderFile, &fragment_shader);

    std::string vertex_shader_point, fragment_shader_point;
    res = ReadFile(kVertexShaderPointsFile, &vertex_shader_point) &&
               ReadFile(kFragmentShaderPointsFile, &fragment_shader_point);

    if (!res) exit(0);

    program_ = std::make_unique<QOpenGLShaderProgram>();
    program_->addShaderFromSourceCode(QOpenGLShader::Vertex,
                                      vertex_shader.c_str());
    program_->addShaderFromSourceCode(QOpenGLShader::Fragment,
                                      fragment_shader.c_str());
    program_->bindAttributeLocation("vertex", kVertexAttributeIdx);
    program_->bindAttributeLocation("normal", kNormalAttributeIdx);
    program_->link();

    program_points_ = std::make_unique<QOpenGLShaderProgram>();
    program_points_->addShaderFromSourceCode(QOpenGLShader::Vertex, vertex_shader_point.c_str());
    program_points_->addShaderFromSourceCode(QOpenGLShader::Fragment, fragment_shader_point.c_str());
    program_points_->bindAttributeLocation("vertex", kVertexAttributeIdx);
    program_points_->link();
  }

  updateGL();
}

void GLWidget::paintGL() {
  glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  if (initialized_) {
    camera_.SetViewport();

    Eigen::Matrix4f projection = camera_.SetProjection();
    Eigen::Matrix4f view = camera_.SetView();
    Eigen::Matrix4f model = camera_.SetModel();

    program_->bind();
    GLuint projection_location = program_->uniformLocation("projection");
    glUniformMatrix4fv(projection_location, 1, GL_FALSE, projection.data());

    GLuint view_location = program_->uniformLocation("view");
    glUniformMatrix4fv(view_location, 1, GL_FALSE, view.data());

    GLuint model_location = program_->uniformLocation("model");
    glUniformMatrix4fv(model_location, 1, GL_FALSE, model.data());

    GLuint LPOS_location = program_->uniformLocation("LPOS");
    glUniform3fv(LPOS_location, 1, &light_position_[0]);

    GLuint LCOL_location = program_->uniformLocation("LCOL");
    glUniform3fv(LCOL_location, 1, &light_color_[0]);

    GLuint calc_phong = program_->uniformLocation("calc_phong");
    glUniform1i(calc_phong, calc_phong_);

    GLuint calc_shadow = program_->uniformLocation("calc_shadow");
    glUniform1i(calc_shadow, calc_shadow_);

    if (vol_ != nullptr) {
      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_3D, vol_->GetTextureId());

      GLint volume = program_->uniformLocation("volume");
      glUniform1i(volume, 0);
    }

    /* Set transfer function */
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_1D, transfer_function_texture_id_);
    GLint TF_location = program_->uniformLocation("transfer_function");
    glUniform1i(TF_location, 1);

    cube_->Render();

    glDisable(GL_BLEND);

    /* Draw light point */
    program_points_->bind();
    projection_location = program_points_->uniformLocation("projection");
    glUniformMatrix4fv(projection_location, 1, GL_FALSE, projection.data());

    view_location = program_points_->uniformLocation("view");
    glUniformMatrix4fv(view_location, 1, GL_FALSE, view.data());

    model_location = program_points_->uniformLocation("model");
    glUniformMatrix4fv(model_location, 1, GL_FALSE, model.data());

    glBindVertexArray(points_vao_);
    GLuint point_vbo;
    GLfloat light_vertices[] = {light_position_.x, light_position_.y, light_position_.z};
    glGenBuffers(1, &point_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, point_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(light_vertices), light_vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);
    glDrawArrays(GL_POINTS, 0, 1);
    glDeleteBuffers(1, &point_vbo);
    glBindVertexArray(0);

    last_render_timestamp_ = QDateTime::currentMSecsSinceEpoch();
  }
}
