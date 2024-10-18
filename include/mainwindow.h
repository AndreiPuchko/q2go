#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <iostream>
#include <string>
#include <QProcess>

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
    std::string color_cmd = "<font color=red>";
    std::string color_out = "<font color=green>";
    std::string font_size2 = "<font size=+2>";
    ~MainWindow();

private slots:
    void on_toolButton_Ok_clicked();
    void on_toolButton_Cancel_clicked();
    void mylog(std::string s);
    void mylog(std::string s, std::string format);
    bool is_python();

private:
    QProcess *process;
    QStringList process_output_list;
    Ui::MainWindow *ui;
    bool run_q2rad();
    int install_local_python();
    int install_on_system_python();
    void download_python_zip();

    void process_output();
    QString process_start(QString program, QStringList arguments, bool async);
};

#endif // MAINWINDOW_H
