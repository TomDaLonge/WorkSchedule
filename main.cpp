#include <QApplication>
#include "Writer.h"
#include "UserInterface.h"
#include "Project.h"


int main(int argc, char *argv[]) {
    

    QApplication app(argc, argv);

    UserInterface test;
    test.resize(700, 500);
    test.show();
    
    return app.exec();
}
