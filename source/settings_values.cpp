#include "settings_values.h"

QString const c_groupName      = "comparator";
QString const c_goldenPathName = "golden_path";
QString const c_diffPathName   = "diff_path";
QString const c_goldenDefault  = "./golden";
QString const c_diffDefault    = "./diff";

QString const c_filename = "settings.ini";

QString const& SettingsValues::comparatorGroupName()
{
  return c_groupName;
}

QString const& SettingsValues::goldenPathValueName()
{
  return c_goldenPathName;
}

QString const& SettingsValues::diffPathValueName()
{
  return c_diffPathName;
}

QString const& SettingsValues::goldenPathDefault()
{
  return c_goldenDefault;
}

QString const& SettingsValues::diffPathDefault()
{
  return c_diffDefault;
}

QString const& SettingsValues::filename()
{
  return c_filename;
}
