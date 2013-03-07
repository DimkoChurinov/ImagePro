#pragma once

#include <QDialog>
#include <QColor>
#include "ui_grid_settings_dialog.h"

class GridDialog : public QDialog
{
  Q_OBJECT
public:
  explicit GridDialog(QWidget *parent = 0, Qt::WindowFlags f = 0);
private slots:
  void ColorButtonClicked();
private:
  void RefreshPalette();
public:
  Ui::GridSettingsDialog ui;
  QColor m_currColor;
};