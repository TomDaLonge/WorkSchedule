#include "UserInterface.h"
#include <QGridLayout>
#include <QPushButton>
#include <QHBoxLayout>

#include <QLabel>
#include <QPushButton>
#include <QMessageBox>
#include <QRegExpValidator>

UserInterface::UserInterface() {
    
    UIWrite = new Writer;
    UIRead = UIWrite->getReader();
    UI_tree = UIRead->getTree();
    
    InitializeLayout();
}

void UserInterface::InitializeLayout(){
    QGridLayout *mainLayout = new QGridLayout();
    this->setLayout(mainLayout);
    this->setWindowTitle("Work Schedule 1.0");
    this->setWindowIcon(QIcon(trUtf8(":/images/mustache.png")));
    //https://lh5.googleusercontent.com/-3LuNrBYkwMs/Uo3uvx0wzBI/AAAAAAAAAN4/Q-TNFtW442g/w1024-h1024/mustache.png
    
    upperLayout = new QVBoxLayout();
    mainLayout->addLayout(upperLayout, 1, 1);
    QVBoxLayout *bottomLayout = new QVBoxLayout();
    mainLayout->addLayout(bottomLayout, 3, 1);
    
    upperLayout->addWidget(UI_tree);
    
    QGridLayout *bUpperLayout = new QGridLayout();
    QHBoxLayout *bBottomLayout = new QHBoxLayout();
    bottomLayout->addLayout(bUpperLayout);
    bottomLayout->addLayout(bBottomLayout);
    
    QLabel *userLabel = new QLabel("User:");
    bUpperLayout->addWidget(userLabel, 1, 1);
    userEdit = new QLineEdit();
    userEdit->setValidator(new QRegExpValidator( QRegExp("[A-Za-z]{0,255}"), this));
    bUpperLayout->addWidget(userEdit, 1, 2);
    
    QLabel *nameLabel = new QLabel("Project name:");
    bUpperLayout->addWidget(nameLabel, 2, 1);
    nameEdit = new QLineEdit();
    nameEdit->setValidator(new QRegExpValidator( QRegExp("[A-Za-z]{0,255}"), this));
    bUpperLayout->addWidget(nameEdit, 2, 2);
    
    QPushButton *startButton = new QPushButton("Start");
    bBottomLayout->addWidget(startButton);
    QPushButton *endButton = new QPushButton("End");
    bBottomLayout->addWidget(endButton);
    QPushButton *clearButton = new QPushButton("Clear");
    bBottomLayout->addWidget(clearButton);
    QPushButton *closeButton = new QPushButton(trUtf8("Close"));
    bBottomLayout->addWidget(closeButton);
    
    connect(startButton, SIGNAL(clicked()), this, SLOT(newStartPoint()));
    connect(endButton, SIGNAL(clicked()), this, SLOT(newEndPoint()));
    connect(clearButton, SIGNAL(clicked()), this, SLOT(clear()));
    connect(closeButton, SIGNAL(clicked()), this, SLOT(close()));
}

void UserInterface::newStartPoint(){
    QString name = nameEdit->text();
    QString user = userEdit->text();
    Project *UIProject = new Project(name, user);
    
    if (!checkFields()){
        noAcceptableInput();
    } else {
        UIWrite->writeNewDatePoint(UIProject, true);
        delete UIProject;
        //maybe better storage management possible??
        UI_tree = UIRead->getTree();
        upperLayout->addWidget(UI_tree);
    }
}

void UserInterface::newEndPoint(){
    QString name = nameEdit->text();
    QString user = userEdit->text();
    Project *UIProject = new Project(name, user);
    
    if (!checkFields()){
        noAcceptableInput();
    } else {
        UIWrite->writeNewDatePoint(UIProject, false);
        delete UIProject;
        //maybe better storage management possible??
        UI_tree = UIRead->getTree();
        upperLayout->addWidget(UI_tree);
    }
}

void UserInterface::clear(){
    UIWrite->clearDocument();
    //maybe better storage management possible??
    UI_tree = UIRead->getTree();
    upperLayout->addWidget(UI_tree);
}

bool UserInterface::checkFields(){
    if (nameEdit->hasAcceptableInput() && userEdit->hasAcceptableInput()){
        return true;
    } else {
        return false;
    }
}

void UserInterface::noAcceptableInput(){
    QMessageBox error;
    
    error.setWindowTitle("Wrong Input");
    error.setText(trUtf8("Please give an allowed Input."));
    
    error.exec();
}