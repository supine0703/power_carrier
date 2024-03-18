#include "configurations.h"

#include <QFontDatabase>

PROJECT_USING_NAMESPACE;


QSettings &Configurations::GetSettings()
{
    static QSettings settings(CONFIG_INI, QSettings::IniFormat);
    return settings;
}


bool Configurations::SettingsContains(const QStringList& keys)
{
    bool all_contains = true;
    for (const auto& key : keys)
    {
        if (!SETTINGS().contains(key))
        {
            all_contains = false;
            break;
        }
    }
    return all_contains;
}


QFont Configurations::LoadIconFont()
{
    static QFont iconFont = QFont(
        QFontDatabase::applicationFontFamilies(
            QFontDatabase::addApplicationFont(
                ":/icon/res/iconfont/iconfont.ttf"
        )
    ).at(0)
);
    return iconFont;
}
