#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <iostream>
#include <string>
#include <QProcess>
#include <QSplashScreen>

namespace Ui
{
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    std::string color_task = "<font color=cyan>";
    std::string color_cmd = "<font color=pink>";
    std::string color_err = "<font color=red font size=+3>";
    std::string color_out = "<font color=green>";
    std::string font_size2 = "<font size=+2>";
    ~MainWindow();

private slots:
    void on_toolButton_Ok_clicked();
    void on_toolButton_Cancel_clicked();
    void mylog(std::string s);
    void mylog(std::string s, std::string format);
    bool is_python();
    void process_finished(int exitCode, QProcess::ExitStatus ExitStatus);
    void closeEvent (QCloseEvent *event);
    void showSplash();
private:
    QProcess *process;
    QStringList process_output_list;
    Ui::MainWindow *ui;
    bool busy;
    QSplashScreen *splash;
    bool run_q2rad();
    
    bool install_local_python();
    bool install_global_python();
    bool install_pip();
    bool install_local_q2rad();
    bool download_python_zip();
    
    void process_output();
    QString process_start(QString program, QStringList arguments, bool async);
};

#endif // MAINWINDOW_H
