#ifndef PROJECT_H
#define	PROJECT_H

#include <QString>

class Project {
public:
    Project(QString name, QString user);
    QString get_name();
    QString get_user();

private:
    QString p_name;
    QString p_user;
};

#endif	/* PROJECT_H */