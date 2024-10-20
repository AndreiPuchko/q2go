#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <windows.h>
#include <iostream>
#include <string>
#include "q2dialog.h"
#include <QProcess>
#include <QString>
#include <QDir>
#include <QByteArray>
#include "q2net.h"
#include <QDebug>
#include <QBuffer>
#include <QTextStream>
#include <QThread>
#include <quazip/quazipfile.h>
#include <quazip/quazip.h>
#include <future>
#include <thread>
#include <chrono>

// #include "fmt/core.h"

using namespace std;

const string PYTHON_VERSION = "3.11.7";
const string PYTHON_FOLDER = "q2rad/python.loc." + PYTHON_VERSION;
const string PYTHON_SOURCE = "https://www.python.org/ftp/python/" + PYTHON_VERSION + "/python-" + PYTHON_VERSION + "-embed-amd64.zip";

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent),
                                          ui(new Ui::MainWindow)
{
    process = new QProcess(this);
    busy = false;
    connect(process, &QProcess::readyReadStandardOutput, this, &MainWindow::process_output);
    connect(process, &QProcess::finished, this, &MainWindow::process_finished);

    // install_local_python();
    // exit(0);
    if (run_q2rad())
    {
        close();
        exit(0);
    }
    ui->setupUi(this);
    if (!is_python())
    {
        ui->radioButton_system->setDisabled(true);
    }
    ui->progressBar->hide();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::mylog(string s)
{
    mylog(s, "");
}

void MainWindow::mylog(string s, string format)
{
    if (s.length() > 0 && s.at(0) == '\n')
    {
        // ui->textEdit_log->append(QString().fromStdString(format + s));
        ui->textEdit_log->moveCursor(QTextCursor::End);
        ui->textEdit_log->insertHtml(QString().fromStdString(format + s.substr(1)).replace("\n", "").replace("\r", ""));
        ui->textEdit_log->moveCursor(QTextCursor::End);
    }
    else
    {
        ui->textEdit_log->append(QString().fromStdString(format + s));
    }
}

void MainWindow::process_output()
{
    QByteArray output = process->readAllStandardOutput();
    QString outputString = QString::fromLocal8Bit(output);
    QStringList lines = outputString.split("\n", Qt::SkipEmptyParts);
    // qDebug() << lines;
    QApplication::processEvents();
    process_output_list.append(lines);
    foreach (const QString &line, lines)
    {
        mylog(QString(line).replace("\r", "").toStdString());
    }
}

QString MainWindow::process_start(QString program, QStringList arguments, bool async = true)
{
    process_output_list.clear();
    process->kill();
    process->start(program, arguments);
    QString command = program;
    command.append(" ");
    command.append(arguments.join((" ")));
    // process_output_list.append(program);
    mylog(command.toStdString(), color_cmd);
    QApplication::processEvents();
    if (async)
    {
        busy = true;
        return QString();
    }
    else
    {
        qDebug() << program;
        process->waitForFinished(9999999);
        this->setDisabled(false);
        return QString(process_output_list.join("\n"));
    }
}

void MainWindow::process_finished(int exitCode, QProcess::ExitStatus ExitStatus)
{
    busy = false;
    qDebug() << "done";
}

bool MainWindow::run_q2rad()
{
    QDir python_folder(QString::fromStdString(PYTHON_FOLDER));
    QDir q2rad_folder(QString::fromStdString(PYTHON_FOLDER) + "/Lib/site-packages/q2rad");
    qint64 *pid;
    if (python_folder.exists() and q2rad_folder.exists())
    {
        // process->setWorkingDirectory(QApplication::applicationDirPath() + "/q2rad");
        process->startDetached(QApplication::applicationDirPath() + "/" + QString().fromStdString(PYTHON_FOLDER) + "/python.exe", {"-m", "q2rad"}, QApplication::applicationDirPath() + "/q2rad", pid);
        exit(0);
        qDebug() << process->state() << busy;
        // busy = true;
        // while (busy)
        // {
        // }
        qDebug() << process->state() << busy;
        // if (process->processId() != 0)
        //     exit(0);
    }
    process->setWorkingDirectory(QApplication::applicationDirPath());
    return false;
}

bool MainWindow::download_python_zip()
{
    QDir folder(QString::fromStdString(PYTHON_FOLDER));
    if (!folder.exists())
    {
        if (!folder.mkpath("."))
        {
            mylog("Cannot create folder: " + PYTHON_FOLDER, color_err);
            return false;
        }
        else
        {
        }
    }
    else
    {
        mylog("Local Python is ready!", color_task);
        return true;
    }
    QByteArray python_zip_file;
    string font_color_green = "<font color=green>";
    mylog("Downloading Python", font_size2 + color_task);
    mylog(PYTHON_SOURCE, font_color_green);

    python_zip_file = urlretrieve(PYTHON_SOURCE);
    QBuffer buffer(&python_zip_file);
    buffer.open(QIODevice::ReadOnly);

    mylog("Downloading complete", font_size2);
    mylog("Start extracting", font_size2 + color_task);
    mylog("");
    QuaZip zip(&buffer);
    zip.open(QuaZip::mdUnzip);
    QuaZipFile file(&zip);
    for (bool f = zip.goToFirstFile(); f; f = zip.goToNextFile())
    {
        // if (zip.getCurrentFileName() == "python311._pth")
        //     continue;
        file.open(QIODevice::ReadOnly);
        mylog("\n☐", font_color_green);
        QFile newFile(QString().fromStdString(PYTHON_FOLDER) + "/" + zip.getCurrentFileName());
        newFile.open(QIODevice::WriteOnly);
        newFile.write(file.readAll());
        QThread::msleep(50);
        QApplication::processEvents();
        newFile.close();
        file.close();
    }
    QFile python_pth(QString().fromStdString(PYTHON_FOLDER) + "/python311._pth");
    if (python_pth.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QTextStream out(&python_pth);
        out << "python311.zip\n";
        out << "Lib\n";
        out << "Scripts\n";
        out << ".\n";
        out << "\n";
        out << "# Uncomment to run site.main() automatically\n";
        out << "import site\n";
    }
    else
    {
        mylog("Error occured while trying to create file : <b>" + PYTHON_FOLDER + "/python311._pth" + "</b>", color_err);
        return false;
    }
    python_pth.close();

    mylog("Local Python successfully installed!", font_size2);
    buffer.close();
    zip.close();
    return true;
}

bool MainWindow::install_pip()
{
    mylog("Checking if pip is there:", color_task);
    process->setWorkingDirectory(QApplication::applicationDirPath() + "/" + QString().fromStdString(PYTHON_FOLDER));
    QString ret = process_start(this->process->workingDirectory() + "/python.exe", {"-m", "pip", "-V"}, false);
    if (ret.length() == 0)
    {
        mylog("Donloading & Installing pip", color_task);
        urlretrieve("https://bootstrap.pypa.io/get-pip.py", PYTHON_FOLDER + "/get-pip.py");
        process_start(this->process->workingDirectory() + "/python.exe", {"get-pip.py", "--no-warn-script-location"}, false);
        mylog("pip is installed!", color_task);
    }
    else
        mylog("Pip is ready!", color_task);
    return true;
}

bool MainWindow::install_local_python()
{
    if (!download_python_zip())
        return 0;
    if (!install_pip())
        return 0;
    if (!install_local_q2rad())
        return 0;
}

bool MainWindow::install_local_q2rad()
{
    process->setWorkingDirectory(QApplication::applicationDirPath());
    QString ret = process_start(this->process->workingDirectory() + "/" + QString().fromStdString(PYTHON_FOLDER) + "/python.exe", {"-m", "pip", "install", "--no-warn-script-location", "q2rad"}, false);
    mylog("q2rad is installed!", color_task);
    run_q2rad();
    return true;
}

int MainWindow::install_on_system_python()
{
    return 0;
}

void MainWindow::on_toolButton_Cancel_clicked()
{
    // std::cout << q2mess("Close");
    if (busy)
        return;
    close();
}

void MainWindow::on_toolButton_Ok_clicked()
{
    if (busy)
        return;
    if (ui->radioButton_local->isChecked())
    {
        install_local_python();
    }
    else
    {
        install_on_system_python();
    }
}

bool MainWindow::is_python()
{
    mylog("Checking if system python there", color_task);
    QString ret = process_start("python", QStringList() << "-V", false);
    if (ret.length() == 0)
    {
        return false;
    }
    else
    {
        return true;
    }
}
