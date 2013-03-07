#include <QSettings>
#include <QFileDialog>
#include "settings_values.h"
#include "settings_dialog.h"

SettingsDialog::SettingsDialog( QWidget *parent /*= 0*/, Qt::WindowFlags f /*= 0*/ )
  : QDialog(parent, f)
{
  ui.setupUi(this);
  setModal(true);
  setSizeGripEnabled(false);

  QSettings sett(SettingsValues::filename(),QSettings::IniFormat);
  sett.beginGroup(SettingsValues::comparatorGroupName());
  ui.lineEditGolden->setText(sett.value(SettingsValues::goldenPathValueName(), SettingsValues::goldenPathDefault()).toString());
  ui.lineEditDiff->setText(sett.value(SettingsValues::diffPathValueName(), SettingsValues::diffPathDefault()).toString());  
  sett.endGroup();
  connect(ui.setupGoldenButton, SIGNAL(clicked()), this, SLOT(SetupGolden()));
  connect(ui.setupDiffButton, SIGNAL(clicked()), this, SLOT(SetupDiff()));
}

void SettingsDialog::accept()
{
  QSettings sett(SettingsValues::filename(), QSettings::IniFormat);
  sett.beginGroup(SettingsValues::comparatorGroupName());
  sett.setValue(SettingsValues::goldenPathValueName(), ui.lineEditGolden->text());
  sett.setValue(SettingsValues::diffPathValueName(), ui.lineEditDiff->text());  
  sett.endGroup();
  QDialog::accept();
}

void SettingsDialog::SetupGolden()
{
  QString dir = QFileDialog::getExistingDirectory(this, tr("Select Directory"),
    QDir::currentPath(),
    QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
  if(dir.isEmpty())
    return;
  ui.lineEditGolden->setText(dir);
}

void SettingsDialog::SetupDiff()
{
  QString dir = QFileDialog::getExistingDirectory(this, tr("Select Directory"),
    QDir::currentPath(),
    QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
  if(dir.isEmpty())
    return;
  ui.lineEditDiff->setText(dir);
}
