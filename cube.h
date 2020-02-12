// Author: Marc Comino 2019

#ifndef CUBE_H_
#define CUBE_H_

#include <GL/glew.h>

#include <eigen3/Eigen/Geometry>

#include <vector>

namespace data_representation {

class Cube {
 public:
  /**
   * @brief Cube Constructor of the class.
   */
  Cube();

  /**
   * @brief ~Cube Destructor of the class.
   */
  ~Cube();

  /**
   * @brief Render Renders the cube.
   */
  void Render();

 public:
  /**
   * @brief min The minimum point of the bounding box.
   */
  Eigen::Vector3f min_;

  /**
   * @brief max The maximum point of the bounding box.
   */
  Eigen::Vector3f max_;

 private:
  /**
   * @brief element_count_ Number of rendered elements (vertices).
   */
  int element_count_;

  /**
   * @brief vao_id_ Vertex Array id.
   */
  GLuint vao_id_;

  /**
   * @brief vbo_id_ Vertex Buffer Object id.
   */
  GLuint vbo_id_;

  /**
   * @brief faces_id_ Face Indices Array id.
   */
  GLuint faces_id_;
};

}  // namespace data_representation

#endif  //  CUBE_H_
