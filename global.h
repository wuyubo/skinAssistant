#ifndef GLOBAL_H
#define GLOBAL_H
#define VERSION   "skinAssistant V2.3(图片反转)"
#define WORK_FILE  "./ZUI_WORK.xml"
#define BACK_UP_FILE "./ZUI_bak.xml"

#define XML_PATH  "./"

#define DEBUG(x) (qDebug()<<x)

#define SETTING_FILE "./setting.ini"
typedef enum
{
    ADD_NEW_BEFORE,
    ADD_NEW_AFTER,
    ADD_NEW_CHILD,
    ADD_MOVE_UP,
    ADD_MOVE_DOWN
}ADD_NODE_TYPE;

typedef enum
{
    MODE_SIMPLE,
    MODE_UI,

}OPTION_MODE;

typedef enum
{
    ALIGN_NORMAL,
    ALIGN_LEFT,
    ALIGN_RIGHT,
    ALIGN_CENTER,
    ALIGN_UP,
    ALIGN_DOWN,
    ALIGN_CENTER_UD,
    ALIGN_PARENT,
    ALIGN_POSITION,
    ALIGN_RESIZE,
    ALIGN_MAX,
}ALIGN_MODE;
#endif // GLOBAL_H
