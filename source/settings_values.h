#pragma once

#include <QString>
class SettingsValues
{
public:
  static QString const&filename();
  //comparator settings
  static QString const&comparatorGroupName();
  static QString const&goldenPathValueName();
  static QString const&diffPathValueName();
  static QString const&goldenPathDefault();
  static QString const&diffPathDefault();
};