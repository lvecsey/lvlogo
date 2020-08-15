/*
    Create a 3D particle render from a 2D input image.
    Copyright (C) 2020  Lester Vecsey

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <math.h>

#include <sys/mman.h>

#include "mini_gxkit.h"

#include "writefile.h"

typedef struct {

  point3d_t pnta;
  
  pixel_t color;
  
} gridpoint;

int main(int argc, char *argv[]) {

  image_t img;

  long int num_pixels;

  size_t img_sz;

  gridpoint *points;

  long int xgrid;

  long int ygrid;

  long int num_points;

  int retval;

  double freqs[2] = { 7.0, 9.0 };

  double depths[2] = { 1.0, 0.80 };
  
  double depth;
  
  point_t pt;

  long int xpos, ypos;

  long int pointno;

  double radians;

  double hyp;

  point3d_t pnta;

  point3d_t pntb;

  double aspect;
  
  long int xi, yi;

  double v;

  pixel_t white;

  long int offset;
  
  img.xres = 500;
  img.yres = 500;

  num_pixels = img.xres * img.yres;
  img_sz = num_pixels * sizeof(pixel_t);

  img.rgb = malloc(img_sz);
  if (img.rgb == NULL) {
    perror("malloc");
    return -1;
  }

  memset(img.rgb, 0, img_sz);
  
  xgrid = 960;
  ygrid = 960;

  num_points = xgrid * ygrid;

  points = malloc(sizeof(gridpoint) * num_points);
  if (points == NULL) {
    perror("malloc");
    return -1;
  }
  
  {

    int fd;
    struct stat buf;
    void *m;

    pixel_t *rgb;

    long int gridno;

    double height;
    
    fd = open("flatlogo_960x960.rgb", O_RDWR);
    retval = fstat(fd, &buf);
    m = mmap(NULL, buf.st_size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
    if (m == MAP_FAILED) {
      perror("mmap");
      return -1;
    }

    rgb = (pixel_t*) m;
    
    for (yi = 0; yi < ygrid; yi++) {
      for (xi = 0; xi < xgrid; xi++) {

	gridno = yi * xgrid + xi;

	height = (rgb[gridno].r / 65535.0);
	height += (rgb[gridno].g / 65535.0);
	height += (rgb[gridno].b / 65535.0);	
	height /= 3.0;
	
	points[gridno] = (gridpoint) { .pnta.x = -1.0 + 2.0 * xi / xgrid, .pnta.y = -1.0 + 2.0 * yi / ygrid, .pnta.z = 0.5 - height * 0.5, .color = rgb[gridno] };
	
      }
    }
    
    munmap(m, buf.st_size);

    retval = close(fd);
    if (retval == -1) {
      perror("close");
      return -1;
    }
    
  }

  depth = 1.0;

  aspect = ((double) img.xres) / img.yres;

  v = 0.5;

  white = (pixel_t) { .r = 65535, .g = 65535, .b = 65535 };
  
  for (yi = 0; yi < ygrid; yi++) {
    for (xi = 0; xi < xgrid; xi++) {

      pointno = yi * xgrid + xi;

      if (points[pointno].color.r == 0) {
	continue;
      }
      
      hyp = sqrt(points[pointno].pnta.x * points[pointno].pnta.x + points[pointno].pnta.y * points[pointno].pnta.y);

      radians = 2.0 * M_PI * hyp / M_SQRT2;

      pnta.x = points[pointno].pnta.x + 0.01 * cos(radians * freqs[0]);
      pnta.y = points[pointno].pnta.y + 0.01 * sin(radians * freqs[1]);
      pnta.z = points[pointno].pnta.z;

      pntb.x = (1.0 - v) * pnta.x + v * hyp * pnta.x / 1.15;
      pntb.y = (1.0 - v) * pnta.y + v * hyp * pnta.y / 1.15;
      pntb.z = (1.0 - v) * pnta.z + v * pnta.z;

      depth = (1.0 - v) * depths[0] + v * depths[1];
      
      pt.x = pntb.x / (pntb.z + depth);
      pt.y = pntb.y / (pntb.z + depth);

      pt.x /= aspect;
      
      xpos = pt.x * (img.xres >> 1); xpos += (img.xres >> 1);
      ypos = pt.y * (img.yres >> 1); ypos += (img.yres >> 1);

      if (xpos < 0 || xpos >= img.xres) {
	continue;
      }

      if (ypos < 0 || ypos >= img.yres) {
	continue;
      }

      offset = ypos * img.xres + xpos;
      
      img.rgb[offset] = white;

    }
      
  }
    
  free(points);
  
  {

    int out_fd;
    ssize_t bytes_written;

    out_fd = 1;

    bytes_written = writefile(out_fd, img.rgb, img_sz);
    if (bytes_written != img_sz) {
      perror("write");
      return -1;
    }
    
  }
  
  free(img.rgb);
  
  return 0;

}
