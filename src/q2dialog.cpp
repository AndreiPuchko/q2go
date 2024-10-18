#include "q2dialog.h"
#include <QMessageBox>
#include <QPushButton>
#include <QString>
#include <string>
#include <vector>


int q2ask(std::string message)
{
    return q2mess(message, {"Cancel", "Ok"});
}

int q2mess(std::string message)
{
    return q2mess(message, {"Ok"});
}

int q2mess(std::string message, std::vector<std::string> buttons)
{
    // std::cout << buttons;
    QMessageBox msgBox;
    std::vector<QPushButton *> dialog_buttons_vector;
    for ( auto  elem : buttons)
    {
        dialog_buttons_vector.push_back(msgBox.addButton(QString::fromStdString(elem), QMessageBox::ActionRole));
    }
    msgBox.setText(QString::fromStdString(message));
    msgBox.exec();
    for(int i=0; i < dialog_buttons_vector.size(); i++)
    {
        if( dialog_buttons_vector[i] == msgBox.clickedButton())
        {
            return i+1;
        }
    }
    return 0;
}
