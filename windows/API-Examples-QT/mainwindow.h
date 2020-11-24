#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <mutex>
#include <vector>
#include <map>
#include <QListWidgetItem>
#include <QListWidget>
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE
class AgoraScreenDlg;
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
public slots:
	void  on_currentItemChanged_slot(QListWidgetItem *current, QListWidgetItem *previous);
private:
    void InitSceneDialog();
    void InitSceneList();
    void InitCtrlText();

	void ReleaseScene(QListWidgetItem* item);
	void CreateScene(QListWidgetItem *item);
private:
	std::vector<QString> m_vecBasic, m_vecAdvanced;
	std::mutex m_mutex;

    Ui::MainWindow *ui;
    AgoraScreenDlg* m_pDlgScreen;
};
#endif // MAINWINDOW_H
