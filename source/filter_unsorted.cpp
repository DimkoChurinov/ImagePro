
#include "filter_unsorted.h"

#include "ui_whitenoise.h"

bool FilterWhiteNoise::ShowParamsDialog (QWidget *parent)
{
  QDialog dlg(parent);
  Ui::WhiteNoiseDialog ui;
  ui.setupUi(&dlg);
  dlg.setModal(true);
  dlg.setSizeGripEnabled(false);
  int rc = dlg.exec();
  if (!rc) {
    return false;
  }
  p = ui.doubleSpinBoxProbaility->value();
  d = ui.spinBoxDispersion->value();
  return true;
}


void FilterWhiteNoise::Apply (const QImage &imageSrc, QImage &imageDest) const
{
  int sx = imageSrc.width(),
      sy = imageSrc.height();
  int bpl = imageSrc.bytesPerLine();
  

  for (int y = 0; y < sy; y++) {
    for (int x = 0; x < sx; x++) {
      double curp =  (double)qrand() / RAND_MAX;
      if (curp > p) {
        continue;
      }
      int dd = qrand() % (2 * d + 1) - d;
      for (int c = 0; c < 3; c++) {
        int cc = dd + imageSrc.constBits()[y * bpl + (x << 2) + c];
        imageDest.bits()[y * bpl + (x << 2) + c] = clamp(cc, 0, 255);
      }
    }
  }
}

#include "ui_dust.h"

bool FilterDust::ShowParamsDialog (QWidget *parent)
{
  QDialog dlg(parent);
  Ui::DustDialog ui;
  ui.setupUi(&dlg);
  dlg.setModal(true);
  dlg.setSizeGripEnabled(false);
  int rc = dlg.exec();
  if (!rc) {
    return false;
  }
  p = ui.doubleSpinBoxProbaility->value();
  min = ui.spinBoxDispersion->value();
  return true;
}

void FilterDust::Apply (const QImage &imageSrc, QImage &imageDest) const
{
  int sx = imageSrc.width(),
      sy = imageSrc.height();
  int bpl = imageSrc.bytesPerLine();


  for (int y = 0; y < sy; y++) {
    for (int x = 0; x < sx; x++) {
      double curp =  (double)qrand() / RAND_MAX;
      if (curp > p) {
        continue;
      }
      uchar dd = qrand() % (255 - min + 1) + min;
      for (int c = 0; c < 3; c++) {
        imageDest.bits()[y * bpl + (x << 2) + c] = dd;
      }
    }
  }
}

#include <QPainter>
#include "grid_dialog.h"

void FilterGrid::Apply( const QImage &imageSrc, QImage &imageDest ) const
{
  imageDest = imageSrc;
  int sx = imageSrc.width();
  int sy = imageSrc.height();
  QPainter p;
  p.begin(&imageDest);
  p.setPen(QPen(m_color)); 
  for(int x = 1; x < sx; x += m_wStep)
  {
    p.drawLine(x, 1, x, sy);
  }
  for(int y = 1; y < sy; y += m_wStep)
  {
    p.drawLine(1, y, sx, y);
  }
  p.end();
  
}

bool FilterGrid::ShowParamsDialog( QWidget *parent )
{
  GridDialog dlg(parent);
  int rc = dlg.exec();
  if(!rc)
    return false;
  m_wStep = dlg.ui.spinBox->value();
  m_hStep = dlg.ui.spinBox_2->value();
  m_color = dlg.m_currColor;
  return true;
}
