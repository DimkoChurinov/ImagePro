#include "filter_binary.h"
#include <QImage>

void FilterInversion::Apply (const QImage &imageSrc, QImage &imageDest) const
{
  imageDest.invertPixels();
}

void FilterErosion::Apply (const QImage &imageSrc, QImage &imageDest) const
{
  int sx = imageSrc.width(),
      sy = imageSrc.height();
  int bpl = imageSrc.bytesPerLine();
  const uchar *src = imageSrc.constBits();
  uchar       *dest = imageDest.bits();

  for (int y = 0; y < sy; y++) {
    for (int x = 0; x < sx; x++) {
      for (int c = 0; c < 3; c++) {
        uchar minpix = src[y * bpl + (x << 2) + c];
        for (int j = -1; j <= 1; j++) {
          for (int i = -1; i <= 1; i++) {
            int xx = x + i;
            int yy = y + j;
            if (xx < 0 || xx >= sx || yy < 0 || yy >= sy) {
              continue;
            }
            uchar pix = src[yy * bpl + (xx << 2) + c];
            if (pix < minpix) {
              minpix = pix;
            }
          }
        }
        dest[y * bpl + (x << 2) + c] = minpix;
      }
    }
  }
}

void FilterDilatation::Apply (const QImage &imageSrc, QImage &imageDest) const
{
  int sx = imageSrc.width(),
      sy = imageSrc.height();
  int bpl = imageSrc.bytesPerLine();
  const uchar *src = imageSrc.constBits();
        uchar *dest = imageDest.bits();
  int index = 0;
  for (int y = 0; y < sy; y++) {
    for (int x = 0; x < bpl; x++, index++) {
      if ((x & 3) == 3) {
        continue;
      }
      uchar maxpix = src[index];
      for (int j = -1; j <= 1; j++) {
        for (int i = -1; i <= 1; i++) {
          int xx = (x >> 2) + i;
          int yy = y + j;
          if (xx < 0 || xx >= sx || yy < 0 || yy >= sy) {
            continue;
          }
          uchar pix = src[yy * bpl + (xx << 2) + (x & 3)];
          if (pix > maxpix) {
            maxpix = pix;
          }
        }
      }
      dest[index] = maxpix;
    }
  }
}