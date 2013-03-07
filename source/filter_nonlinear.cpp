#include "filter_nonlinear.h"

#include "ui_median.h"
#include "ui_perlin noise.h"
#include "ui_oil_settings.h"
#include <ctime>
#include <cmath>

bool FilterMedian::ShowParamsDialog (QWidget *parent)
{
  QDialog dlg(parent);
  Ui::MedianDialog ui;
  ui.setupUi(&dlg);
  dlg.setModal(true);
  dlg.setSizeGripEnabled(false);
  int rc = dlg.exec();
  if (!rc) {
    return false;
  }
  radius = ui.spinBoxDispersion->value();
  return true;
}

void FilterMedian::Apply (const QImage &imageSrc, QImage &imageDest) const
{
  int sx = imageSrc.width(),
      sy = imageSrc.height();
  int bpl = imageSrc.bytesPerLine();


  for (int y = 0; y < sy; y++) {
    for (int x = 0; x < sx; x++) {
      QVector<int> colors[3];
      for (int c = 0; c < 3; c++) {
        colors[c].reserve((2 * radius + 1) * (2 * radius + 1));
      }
      for (int j = -radius; j <= radius; j++) {
        for (int i = -radius; i <= radius; i++) {
          int xx = x + i;
          int yy = y + j;
          if (xx < 0) { xx += sx; }
          if (xx >= sx) { xx -= sx; }
          if (yy < 0) { yy += sy; }
          if (yy >= sy) { yy -= sy; }
          for (int c = 0; c < 3; c++) {
            int cc = imageSrc.constBits()[yy * bpl + (xx << 2) + c];
            colors[c].push_back(cc);
          }
        }
      }
      for (int c = 0; c < 3; c++) {
        qSort(colors[c]);
        imageDest.bits()[y * bpl + (x << 2) + c] = colors[c][colors[c].size() / 2 + 1];
      }
    }
  }

}