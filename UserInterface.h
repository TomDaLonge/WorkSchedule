#ifndef USERINTERFACE_H
#define	USERINTERFACE_H
#include <QWidget>
#include <QLineEdit>
#include "Writer.h"
#include "Reader.h"
#include "Project.h"
#include <QTreeWidget>
#include <QVBoxLayout>

class UserInterface : public QWidget {
    Q_OBJECT
public:
    UserInterface();
private:
    void InitializeLayout();
    bool checkFields();
    void noAcceptableInput();
//    void setProject();
    
    
    Writer *UIWrite;
    Reader *UIRead;
    QLineEdit *nameEdit;
    QLineEdit *userEdit;
//    Project *UIProject;
    QTreeWidget *UI_tree;
    QVBoxLayout *upperLayout;
    
private slots:
    void newStartPoint();
    void newEndPoint();
    void clear();
    
    
    
};

#endif	/* USERINTERFACE_H */

