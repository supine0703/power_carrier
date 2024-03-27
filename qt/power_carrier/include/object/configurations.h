#ifndef CONFIGURATIONS_H
#define CONFIGURATIONS_H

#include <QSettings>


#define SETTINGS Configurations::GetSettings
#define SETTINGS_CONTAINS Configurations::SettingsContains

#define ICON_FONT Configurations::LoadIconFont

class Configurations
{
    Configurations() = delete;
    ~Configurations() = delete;
    Configurations(const Configurations&) = delete;
    Configurations(const Configurations&&) = delete;
    Configurations& operator=(const Configurations&) = delete;
    Configurations& operator=(Configurations&&) = delete;

public:
    static QSettings &GetSettings();

    static bool SettingsContains(const QStringList& keys);


    static QFont LoadIconFont();
};

#define _COM_RANGE_MIN_       "/com_port/range_min"
#define _COM_RANGE_MAX_       "/com_port/range_max"
#define _COM_PORT_NUM_        "/com_port/index"

#define _SERIAL_PORT_BRI_     "/serial_port_info/baud_rate_index"
#define _SERIAL_PORT_DBI_     "/serial_port_info/data_bits_index"
#define _SERIAL_PORT_SBI_     "/serial_port_info/stop_bits_index"
#define _SERIAL_PORT_PI_      "/serial_port_info/parity_index"
#define _SERIAL_PORT_LOCK_    "/serial_port_info/lock"
#define _LOG1_SELECTOR1_      "/log_settings/log1_selector1"
#define _LOG2_SELECTOR1_      "/log_settings/log2_selector1"
#define _LOG1_SELECTOR2_      "/log_settings/log1_selector2"
#define _LOG2_SELECTOR2_      "/log_settings/log2_selector2"
#define _LOG1_EYES_STATE_     "/log_settings/log1_eyes_state"
#define _LOG2_EYES_STATE_     "/log_settings/log2_eyes_state"
#define _GROUP1_HIDDEN_       "/group_settings/group1_hidden"
#define _GROUP2_HIDDEN_       "/group_settings/group2_hidden"
#define _GROUP3_HIDDEN_       "/group_settings/group3_hidden"
#define _LOG_SPLITTER_STATE_  "/group_settings/log_splitter_state"

#define _SP_ASSISTANT_HIDDEN_ "/left_tool/sp_assistant_hidden"

#define _LEFT_TOOL_INDEX_     "/widget/left_tool_index"

#define _DB_DEFAULT_PATH_     "/db_setting/default_path"
#define _DB_LAST_OPEN_        "/db_setting/last_open"
#define _DB_SAVE_PATH_        "/db_setting/save_path"

#define _MAIN_WIDGET_WIDTH_   "/widget/width"


#endif // CONFIGURATIONS_H
