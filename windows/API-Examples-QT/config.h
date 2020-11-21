
#include <QString>
#include <memory>
#include <QSettings>
#include <QObject>
#define APP_ID ""
#define APP_TOKEN ""

#define Str(key) CConfig::GetInstance()->GetStringValue(key)
#define GET_APP_ID CConfig::GetInstance()->GetAPP_ID().toUtf8()

class CConfig
{
public:
    CConfig();
    ~CConfig();

    static CConfig* GetInstance();
    QString GetStringValue(QString key);
    QString GetAPP_ID();
private:
    std::shared_ptr<QSettings>  m_spConfig;
    std::shared_ptr<QSettings>  m_spConfigLangZh;
     std::shared_ptr<QSettings>  m_spConfigLangEn;
    QString m_strAppid;

    bool m_bChinese = false;

    int hashSize = 1000;
};

