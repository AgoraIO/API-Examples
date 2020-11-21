#include "config.h"
#include <QDir>
#include <QFile>
#include <QMessageBox>
#include <QProcess>
#include <QApplication>
#include <QLocale>
#include <QTextCodec>
#include <QtWinExtras>

CConfig* CConfig::GetInstance()
{
    static CConfig config;
    return &config;
}

CConfig::CConfig()
{
    if(strlen(APP_ID))
        m_strAppid = APP_ID;
    else {
        QDir iniPath(QString("AgoraConfig.ini"));
        if(!iniPath.exists()){
            QFile file(QString("AgoraConfig.ini"));
            file.open(QIODevice::ReadWrite);
        }
        m_spConfig = std::make_shared<QSettings>("AgoraConfig.ini",QSettings::IniFormat);
        QString strAppId = m_spConfig->value("/BaseInfo/AppId").toString();;
        if(strAppId.isEmpty()){
           m_spConfig->setValue("/BaseInfo/AppId",QString(""));
        }
       m_strAppid = strAppId;
    }


    if (m_strAppid.length() == 0)
    {
        QMessageBox::critical(nullptr, ("API-Examples"),
                                       ("You must specify APP ID before using the demo"));
        QProcess process;
        process.startDetached("notepad.exe", {"AgoraConfig.ini"}, "");
		ExitProcess(0);
    }
	QString zhPath = QApplication::applicationDirPath() + QString("/zh-cn.ini");
    QDir iniPathZh(zhPath);
    if(iniPathZh.exists(zhPath)){
        m_spConfigLangZh = std::make_shared<QSettings>(zhPath,QSettings::IniFormat);
		m_spConfigLangZh->setIniCodec(QTextCodec::codecForName("utf-8"));
    }
   
	QString enPath = QApplication::applicationDirPath() + QString("/en.ini");
    QDir iniPathEn(enPath);
    if(iniPathEn.exists(enPath)){
        m_spConfigLangEn = std::make_shared<QSettings>(enPath,QSettings::IniFormat);
		m_spConfigLangEn->setIniCodec(QTextCodec::codecForName("utf-8"));
    }

    QLocale locale;
    if (locale.language() == QLocale::Chinese) {//chinese
         m_bChinese = true;
    }
}


CConfig::~CConfig()
{
}


QString CConfig::GetStringValue(QString key)
{
    QString strValue = "";
    QString strKey = QString("/BaseInfo/%1").arg(key);
    if(m_bChinese)
        strValue = m_spConfigLangZh->value(strKey).toString();
    else
        strValue = m_spConfigLangEn->value(strKey).toString();
    return strValue;
}


QString CConfig::GetAPP_ID()
{
    return m_strAppid;
}
