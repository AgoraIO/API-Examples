#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "Basic/ScreenShare/agorascreendlg.h"
#include "Language.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
	, m_pDlgScreen(nullptr)
{
    ui->setupUi(this);

	connect(ui->listWidgetBasic, &QListWidget::currentItemChanged, this, &MainWindow::on_currentItemChanged_slot);

    InitCtrlText();
    InitSceneDialog();
    InitSceneList();
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::InitCtrlText()
{
    if(!commonGroupDoc.isEmpty())
   ui->groupBoxDoc->setTitle(commonGroupDoc);
   if(!commonDocumentWebsite.isEmpty())
   ui->btnAgoraSite->setText(commonDocumentWebsite);
   if(!commonFAQWebsite.isEmpty())
   ui->btnFAQ->setText(commonFAQWebsite);
   if(!commonRegisterWebsite.isEmpty())
   ui->btnRegister->setText(commonRegisterWebsite);
   if(!commonDemoWebsite.isEmpty())
   ui->btnGithub->setText(commonDemoWebsite);
   if(!commonBasicScene.isEmpty())
   ui->groupBoxBasic->setTitle(commonBasicScene);
   if(!commonAdvanceScene.isEmpty())
   ui->groupBoxAdvanced->setTitle(commonAdvanceScene);

}

void MainWindow::InitSceneDialog()
{
    //basic list
	 //basic list
	m_vecBasic.push_back(basicScreenCap);

	m_pDlgScreen = new AgoraScreenDlg(this);
	m_pDlgScreen->setGeometry(ui->groupBoxScene->geometry());
	//m_pDlgScreen->show();
}


void MainWindow::InitSceneList()
{
    for (size_t i = 0; i < m_vecBasic.size(); i++){
        QString text = m_vecBasic[i];
        QListWidgetItem *item = new QListWidgetItem(nullptr);
        item->setText(text);
		ui->listWidgetBasic->addItem(item);
    }


}

void  MainWindow::on_currentItemChanged_slot(QListWidgetItem *current, QListWidgetItem *previous)
{
	if (previous) {
		ReleaseScene(previous);
	}

	if (current) {
		CreateScene(current);
	}
}

void MainWindow::ReleaseScene(QListWidgetItem* item)
{
	QString itemText = item->text();

	if (itemText.compare(basicScreenCap) == 0) {
		m_pDlgScreen->UnInitAgora();
		m_pDlgScreen->hide();
	}

	
}

void MainWindow::CreateScene(QListWidgetItem* item)
{
	QString itemText = item->text();
	if (itemText.compare(basicScreenCap) == 0) {
		m_pDlgScreen->InitAgora();
		m_pDlgScreen->show();
	}
}