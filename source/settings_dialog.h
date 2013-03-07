#pragma once

#include <QDialog>
#include "filter_linear.h"
#include "ui_settings_dialog.h"

class SettingsDialog : public QDialog
{
  Q_OBJECT
public:
  explicit SettingsDialog(QWidget *parent = 0, Qt::WindowFlags f = 0);
  virtual void accept();
private slots:
  void SetupGolden();
  void SetupDiff();
private:
  Ui::SettingsDialog ui;
};