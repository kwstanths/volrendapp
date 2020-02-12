// Author: Marc Comino 2019

#ifndef VOLUME_H_
#define VOLUME_H_

#include <GL/glew.h>

#include <eigen3/Eigen/Geometry>

#include <string>
#include <vector>

namespace data_representation {

class Volume {
 public:
  /**
   * @brief Volume Constructor of the class. Calls clear.
   */
  Volume();

  /**
   * @brief ~Volume Destructor of the class.
   */
  ~Volume();

  /**
   * @brief Clear Empties the data arrays and resets the bounding box
   * vertices.
   */
  void Clear();

  /**
   * @brief GetTextureId Returns the id of the 3D texture where this volume is
   * stored.
   * @return The 3D texture id.
   */
  GLuint GetTextureId();

  friend bool ReadFromDicom(const std::string& path, Volume* vol);

 public:
  std::vector<double> histogram_;

  int width_, height_, depth_;

 private:
  GLuint id_;
};

}  // namespace data_representation

#endif  //  VOLUME_H_
