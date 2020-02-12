// Author: Marc Comino 2019

#ifndef VOLUME_IO_H_
#define VOLUME_IO_H_

#include <volume.h>

#include <string>

namespace data_representation {

/**
 * @brief ReadFromDicom Reads a stack of images in Dicom format and generated
 * the appropiate 3D textures.
 * @param filename The path to the file containing the name of the dicom files
 * that compose the volume.
 * @param vol The resulting volumetric representation.
 * @return Whether it was able to read the file.
 */
bool ReadFromDicom(const std::string &filename, Volume *vol);

}  // namespace data_representation

#endif  // VOLUME_IO_H_
