#ifndef CONFIGURATIONS_H
#define CONFIGURATIONS_H

#include <QSettings>

#define CFGS Configurations::Get

PROJECT_BEGIN_NAMESPACE

class Configurations
{
    Configurations() = delete;
    ~Configurations() = delete;
    Configurations(const Configurations&) = delete;
    Configurations(const Configurations&&) = delete;
    Configurations operator=(const Configurations&) = delete;

public:
    static QSettings& Get()
    {
        static QSettings settings(CONFIG_INI, QSettings::IniFormat);
        return settings;
    }
};

#define _COM_RANGE_MIN_ "/settings/com_range_min"
#define _COM_RANGE_MAX_ "/settings/com_range_max"
#define _COM_PORT_NUM_     "/settings/com_index"


PROJECT_END_NAMESPACE

#endif // CONFIGURATIONS_H
