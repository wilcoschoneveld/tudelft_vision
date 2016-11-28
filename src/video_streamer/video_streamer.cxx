/*
 * This file is part of the TU Delft Vision programs (https://github.com/tudelft/tudelft_vision).
 * Copyright (c) 2016 Freek van Tienen <freek.v.tienen@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <tuv/tuv.h>
#include PLATFORM_CONFIG
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <iostream>
#include <fstream>

int main(int argc, char *argv[])
{
  PLATFORM target;
#if TARGET == Bebop
  EncoderH264 encoder(1920, 1088, 30, 4000000);
  //EncoderJPEG encoder_jpeg;
#else
  EncoderJPEG encoder;
#endif
  UDPSocket::Ptr udp = std::make_shared<UDPSocket>(UDP_TARGET, 5000);
  EncoderRTP rtp(udp);

  Cam::Ptr cam = target.getCamera(CAMERA_ID);
  cam->setOutput(Image::FMT_YUYV, 1088, 1920);
  cam->setCrop(114 + 2300, 106 + 500, 1088, 1920);
  encoder.setInput(cam, EncoderH264::ROTATE_90L);
  encoder.start();

  FILE *fp = fopen("video.h264", "w");
  std::vector<uint8_t> sps = encoder.getSPS();
  std::vector<uint8_t> pps = encoder.getPPS();
  rtp.setSPSPPS(sps, pps);

  cam->start();
  uint32_t i = 0;
  while(true) {
    Image::Ptr img = cam->getImage();
    Image::Ptr enc_img = encoder.encode(img);

    //img->downsample(4);
    //Image::Ptr jpeg_img = encoder_jpeg.encode(img);
    rtp.encode(enc_img);
    //usleep(200000);

    fwrite(sps.data(), sps.size(), 1, fp);
    fwrite(pps.data(), pps.size(), 1, fp);
    fwrite((uint8_t *)enc_img->getData(), enc_img->getSize(), 1, fp);

    /*std::string test = "out" + std::to_string(i) + ".jpg";
    FILE *fp = fopen(test.c_str(), "w");
    fwrite((uint8_t *)jpeg_img->getData(), jpeg_img->getSize(), 1, fp);
    fclose(fp);*/
    ++i;
    i = i %20;
  }


  cam->stop();
  return 0;
}
