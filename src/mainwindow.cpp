#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <windows.h>
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
#include <QtConcurrent>
#include <QFuture>
#include <QPromise>
#include <QCloseEvent>
#include <QSysInfo>
// #include "fmt/core.h"

using namespace std;

const string PYTHON_VERSION = "3.11.7";
const string PYTHON_FOLDER = "q2rad/python.loc." + PYTHON_VERSION;
const string PYTHON_SOURCE = "https://www.python.org/ftp/python/" + PYTHON_VERSION + "/python-" + PYTHON_VERSION + "-embed-amd64.zip";

#if defined(Q_OS_WIN)
const string SCRIPT_FOLDER = "Scripts";
#else
const string SCRIPT_FOLDER = "bin";
#endif

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent),
                                          ui(new Ui::MainWindow)
{
    process = new QProcess(this);
    busy = false;
    connect(process, &QProcess::readyReadStandardOutput, this, &MainWindow::process_output);
    connect(process, &QProcess::finished, this, &MainWindow::process_finished);

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
    // ui->progressBar->hide();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (busy)
    {
        if (q2mess("Installation is in progress! Should I stop??", {"No", "Yes"}) == 2)
            exit(0);
        else
            event->ignore();
    }
    else
        exit(0);
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
    QApplication::processEvents();
    process_output_list.append(lines);
    foreach (const QString &line, lines)
    {
        mylog(QString(line).replace("\r", "").toStdString(), color_out);
    }
}

QString MainWindow::process_start(QString program, QStringList arguments, bool async = false)
{
    process_output_list.clear();
    QString command = program;
    command.append(" ");
    command.append(arguments.join((" ")));
    mylog(command.toStdString(), color_cmd);
    process->kill();
    process->start(program, arguments);
    busy = true;
    ui->frame_top->setDisabled(true);
    if (!process->waitForStarted())
    {
        busy = false;
        ui->frame_top->setDisabled(false);
        return QString("");
    }
    while (busy)
    {
        QThread::msleep(50);
        QApplication::processEvents();
    }
    ui->frame_top->setDisabled(false);
    return QString(process_output_list.join("\n"));
}

void MainWindow::process_finished(int exitCode, QProcess::ExitStatus ExitStatus)
{
    busy = false;
}

bool MainWindow::run_q2rad()
{
    QDir python_folder(QString::fromStdString(PYTHON_FOLDER));
    QDir q2rad_folder(QString::fromStdString(PYTHON_FOLDER) + "/Lib/site-packages/q2rad");
    qint64 *pid;
    QString python_bin = "";
    // QString q2rad_install_folder = "";
    QString q2rad_install_folder = QString("/q2rad");
    QStringList python_test_args = {"-c", "import q2rad;print(q2rad.version.__version__)"};
    QStringList q2rad_run_args = {"-c", "from q2rad.q2rad import main;main()"};

    if (python_folder.exists() and q2rad_folder.exists())
    {
        python_bin = QApplication::applicationDirPath() +
                     "/" + QString().fromStdString(PYTHON_FOLDER) +
                     "/python";
    }
    else if (QDir("q2rad/q2rad").exists())
    {
        python_bin = QApplication::applicationDirPath() +
                     QString().fromStdString("/q2rad/q2rad/" + SCRIPT_FOLDER + "/python");
    }

    if (python_bin.length() != 0)
    {
        QProcess tmp_process;
        tmp_process.start(python_bin, python_test_args);
        tmp_process.waitForFinished();
        QString ret = QString(tmp_process.readAllStandardOutput());
        if (ret.length() > 0)
        {
            tmp_process.startDetached(python_bin, {"-m", "q2rad"}, QApplication::applicationDirPath() + q2rad_install_folder);
            QThread::msleep(3000);
            process->waitForStarted();
            exit(0);
        }
    }

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
        QString ret = process_start(qApp->applicationDirPath() + "/" + QString().fromStdString(PYTHON_FOLDER) + "/python", {"-V"}, false);
        if (ret.length() > 0)
        {
            mylog("Local Python is ready!", color_task);
            return true;
        }
    }
    // no local python - download
    busy = true;
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
    busy = false;
    return true;
}

bool MainWindow::install_pip()
{
    mylog("Checking if pip is there:", color_task);
    process->setWorkingDirectory(QApplication::applicationDirPath() + "/" + QString().fromStdString(PYTHON_FOLDER));
    QString ret = process_start(this->process->workingDirectory() + "/python", {"-m", "pip", "-V"}, false);
    if (ret.length() == 0)
    {
        mylog("Donloading & Installing pip", color_task);
        urlretrieve("https://bootstrap.pypa.io/get-pip.py", PYTHON_FOLDER + "/get-pip.py");
        process_start(this->process->workingDirectory() + "/python", {"get-pip.py", "--no-warn-script-location"}, false);
        mylog("pip is installed!", color_task);
    }
    else
        mylog("Pip is ready!", color_task);
    return true;
}

bool MainWindow::install_global_python()
{
    mylog("Donloading & Installing q2rad", color_task);
    if (!QDir("q2rad").exists())
    {
        QDir("q2rad").mkpath(".");
    }
    urlretrieve("https://raw.githubusercontent.com/AndreiPuchko/q2rad/main/install/get-q2rad.py", "./q2rad/get-q2rad.py");
    process->setWorkingDirectory(QApplication::applicationDirPath());
    QString ret = process_start("python", {"./q2rad/get-q2rad.py", "--no-warn-script-location"}, false);
    mylog("q2rad is installed!", color_task);
    QFile("./q2rad/get-q2rad.py").remove();
    QFile("./start-q2rad.bat").remove();
    exit(0);
    return true;
}

bool MainWindow::install_local_python()
{
    if (!download_python_zip())
        return true;
    if (!install_pip())
        return true;
    if (!install_local_q2rad())
        return true;
    return false;
}

bool MainWindow::install_local_q2rad()
{
    process->setWorkingDirectory(QApplication::applicationDirPath());
    QString ret = process_start(this->process->workingDirectory() + "/" + QString().fromStdString(PYTHON_FOLDER) + "/python", {"-m", "pip", "install", "--no-warn-script-location", "q2rad"}, false);
    mylog("q2rad is installed!", color_task);
    run_q2rad();
    return true;
}

void MainWindow::on_toolButton_Cancel_clicked()
{
    if (busy)
        return;
    close();
}

void MainWindow::on_toolButton_Ok_clicked()
{
    if (busy)
        return;
    ui->progressBar->setMaximum(0);
    if (ui->radioButton_local->isChecked())
    {
        install_local_python();
    }
    else
    {
        install_global_python();
    }
    ui->progressBar->setMaximum(100);
}

bool MainWindow::is_python()
{
    mylog("Checking if system Python is there:", color_task);
    QString ret = process_start("python", {"-V"}, false);
    if (ret.length() == 0)
    {
        mylog("System Python not found", font_size2);
        return false;
    }
    else
    {
        mylog("System Python is available", font_size2);
        return true;
    }
}
