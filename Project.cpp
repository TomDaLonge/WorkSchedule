#include "Project.h"

Project::Project(QString name, QString user) {
    this->p_name = name;
    this->p_user = user;
}

QString Project::get_name() {
    return p_name;
}

QString Project::get_user() {
    return p_user;
}
