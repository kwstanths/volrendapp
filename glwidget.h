// Author: Marc Comino 2019

#ifndef GLWIDGET_H_
#define GLWIDGET_H_

#include <GL/glew.h>
#include <QGLWidget>
#include <QMouseEvent>
#include <QOpenGLShaderProgram>
#include <QString>
#include <QDateTime>

#include <glm/glm.hpp>

#include <memory>

#include "./camera.h"
#include "./cube.h"
#include "./volume.h"

class GLWidget : public QGLWidget {
  Q_OBJECT

 public:
  explicit GLWidget(QWidget *parent = 0);
  ~GLWidget();

  /**
   * @brief LoadVolume Loads a volume model from the input path.
   * @param filename Path to the stack of images composing the volume model.
   * @return Whether it was able to load the volume.
   */
  bool LoadVolume(const QString &filename);

  /**
   * @brief
   * @return
   */
  std::vector<double>& GetVolumeHistogram();

  /**
    Holds the transfer function values, 256 * 4, rgbargba...
  */
  std::vector<float> transfer_function_values_;

  /**
    Sends the transfer function data to the GPU, will call updateGL
    if enough time has passed since the last call
  */
  void SetTransferFunction();

 protected:
  /**
   * @brief initializeGL Initializes OpenGL variables and loads, compiles and
   * links shaders.
   */
  void initializeGL();

  /**
   * @brief resizeGL Resizes the viewport.
   * @param w New viewport width.
   * @param h New viewport height.
   */
  void resizeGL(int w, int h);

  void mousePressEvent(QMouseEvent *event);
  void mouseMoveEvent(QMouseEvent *event);
  void mouseReleaseEvent(QMouseEvent *event);
  void keyPressEvent(QKeyEvent *event);

 private:
  /**
   * @brief program_ A basic shader program.
   */
  std::unique_ptr<QOpenGLShaderProgram> program_;

  /**
   * @brief program_ A basic point rendering shader program.
   */
  std::unique_ptr<QOpenGLShaderProgram> program_points_;
  /**
    The VAO for the point rendering pipeline
  */
  GLuint points_vao_;


  /**
   * @brief camera_ Class that computes the multiple camera transform matrices.
   */
  data_visualization::Camera camera_;

  /**
   * @brief cube_ A cubic mesh used to render the colume.
   */
  std::unique_ptr<data_representation::Cube> cube_;

  /**
   * @brief mesh_ Data structure representing a volume.
   */
  std::unique_ptr<data_representation::Volume> vol_;

  /**
   * @brief initialized_ Whether the widget has finished initializations.
   */
  bool initialized_;

  /**
   * @brief width_ Viewport current width.
   */
  float width_;

  /**
   * @brief height_ Viewport current height.
   */
  float height_;

  /**
   * @brief light_position_ The position of the point light
   */
  glm::vec3 light_position_;

  /**
   * @brief light_color_ The color of the point light
   */
  glm::vec3 light_color_;

  /**
    The texture id for the transfer function
  */
  GLuint transfer_function_texture_id_;

  /**
    The timestamp for the last render
  */
  qint64 last_render_timestamp_;

  /**
    Hold wether to perform phong and shadow calculations
  */
  bool calc_phong_ = true;
  bool calc_shadow_ = true;

 protected slots:
  /**
   * @brief paintGL Function that handles rendering the scene.
   */
  void paintGL();

public slots:

    void LightPosXValueChanged(double arg);
    void LightPosYValueChanged(double arg);
    void LightPosZValueChanged(double arg);

    void LightColorXValueChanged(double arg);
    void LightColorYValueChanged(double arg);
    void LightColorZValueChanged(double arg);

    void SetPhongShadingCalc(bool arg);
    void SetShadowsCalc(bool arg);

};

#endif  //  GLWIDGET_H_
