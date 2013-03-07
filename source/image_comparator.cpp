#include <QMessageBox>
#include <QDir>
#include <QSettings>
#include <QFileInfo>
#include "ui_difference_dialog.h"
#include "settings_values.h"
#include "image_comparator.h"

void ImageComparator::Compare( QString const&filepath, QImage const &img )
{
  QSettings sett(SettingsValues::filename(),QSettings::IniFormat);
  sett.beginGroup(SettingsValues::comparatorGroupName());
  QString goldenPath = sett.value(SettingsValues::goldenPathValueName(), SettingsValues::goldenPathDefault()).toString() + "/";
  QString diffPath =  sett.value(SettingsValues::diffPathValueName(), SettingsValues::diffPathDefault()).toString() + "/";  
  sett.endGroup();
  QFileInfo info(filepath);
  QString const samplePath = goldenPath + info.fileName();
  if(!QFile::exists(samplePath))
  {
    QMessageBox::critical(NULL, "Error!", QString("Golden image \"%1\" does not exist!").arg(samplePath));
    return;
  }
  QImage sample(samplePath);
  sample = sample.convertToFormat(QImage::Format_ARGB32_Premultiplied);
  if(sample.isNull())
  {
    QMessageBox::critical(NULL, "Error!", QString("Could not open file \"%1\"!").arg(samplePath));
    return;
  }
  if(sample.size() != img.size())
  {
    QMessageBox::critical(NULL, "Error!", QString("Sample and current images have different sizes!"));
    return;
  }
  unsigned long long accum = 0;
  int sx = sample.width();
  int sy = sample.height();
  int bpl = sample.bytesPerLine();
  QImage diffImg(sample.size(), QImage::Format_ARGB32);
  for (int y = 0; y < sy; ++y) {
    for (int x = 0; x < sx; ++x) 
    {
      for (int c = 0; c < 3; ++c) 
      {
        unsigned idx = y * bpl + (x << 2) + c;
        uchar diff = abs((int)sample.constBits()[idx] - (int)img.constBits()[idx]);
        diffImg.bits()[idx] = diff;
        accum += diff;
      }
      diffImg.bits()[y * bpl + (x << 2) + 3] = sample.bits()[y * bpl + (x << 2) + 3];
    }
  }
  QString const diffName = diffPath + info.fileName();
  QDir diffDir(diffPath);
  if(!diffDir.exists(diffPath))
  {
    if(!QDir(diffPath + "/..").mkpath(diffDir.dirName()))
    {
      QMessageBox::critical(NULL, "Error!", QString("Could not create diff folder \"%1\"!").arg(diffPath));
      return;
    }
  }
  if(!diffImg.save(diffName, info.suffix().toAscii().data()))
  {
    QMessageBox::critical(NULL, "Error!", QString("Could not save the difference image \"%1\"!").arg(diffName));
    return;
  }
  double diff = ((double)accum / (img.size().width() * img.size().height() * 3) * 100.0 / 255.0);

  QDialog dlg;
  Ui::ImgDifferenceDialog ui;
  ui.setupUi(&dlg);
  dlg.setModal(true);
  dlg.setSizeGripEnabled(false);
  dlg.layout()->setSizeConstraint( QLayout::SetFixedSize );
  ui.label->setText(QString("The difference is: %1").arg(diff, 0, 'f', 2));
  QPixmap pxmp = QPixmap::fromImage(diffImg);
  QRect r = ui.frame->frameRect();
  if(r.width() < pxmp.width())
    pxmp = pxmp.scaledToWidth(r.width(), Qt::SmoothTransformation);
  if(r.height() < pxmp.height())
    pxmp = pxmp.scaledToHeight(r.height(), Qt::SmoothTransformation);
  //ui.differenceLabel->resize(pxmp.size());
  ui.differenceLabel->setPixmap(pxmp);
  
  dlg.exec();

}
