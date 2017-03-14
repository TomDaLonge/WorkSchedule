#include "Writer.h"
#include <QString>
#include <QMessagebox>
#include <QTime>
#include <QDate>
#include <QDateTime>

Writer::Writer() {
    w_reader = new Reader;
    w_document = w_reader->getDocument();
    w_root = w_reader->getRoot();
}    

void Writer::writeNewDatePoint(Project *project, bool isStart){
//    QDateTime date_time = QDateTime::currentDateTime();
    QDate date = QDate::currentDate();
    QTime time = QTime::currentTime();
    QDomElement tmp_parent;
    if (isStart){
        QDomElement datePoint = w_document->createElement("Start");
        datePoint.setAttribute("Date", date.toString());
        datePoint.setAttribute("Time", time.toString());
        tmp_parent = getParent(project, isStart);
        tmp_parent.appendChild(datePoint);
    } else {
        QDomElement datePoint = w_document->createElement("End");
        datePoint.setAttribute("Date", date.toString());
        datePoint.setAttribute("Time", time.toString());
        tmp_parent = getParent(project, isStart);
        tmp_parent.appendChild(datePoint);
        
        QTime start_time = QTime::fromString(tmp_parent.firstChildElement().attribute("Time"), "hh:mm:ss");
        QTime end_time = QTime::fromString(tmp_parent.lastChildElement().attribute("Time"), "hh:mm:ss");

        QDate start_date = QDate::fromString(tmp_parent.firstChildElement().attribute("Date"));
        QDate end_date = QDate::fromString(tmp_parent.lastChildElement().attribute("Date"));

        QTime time_duration(0, 0, 0);
        int day_duration;

        if (start_date == end_date){
            int secs_duration = start_time.secsTo(end_time);
            time_duration = time_duration.addSecs(secs_duration);
            day_duration = 0;
        } else {
            int secs_duration = start_time.secsTo(QTime(23, 59, 59)) + QTime(0, 0, 0).secsTo(end_time) + 1;
            time_duration = time_duration.addSecs(secs_duration);
            day_duration = start_date.daysTo(end_date) - 1;
        }
        tmp_parent.setAttribute("Time_Duration", time_duration.toString("hh:mm:ss"));
        tmp_parent.setAttribute("Day_Duration", day_duration);
        
        updateDurations(project);
        }
    
    Write();
}



void Writer::clearDocument(){
    w_reader->createDocument();
    w_document = w_reader->getDocument();
    w_root = w_reader->getRoot();
    Write();
}

void Writer::Write(){
    QFile file(QDir::currentPath() + "/Schedule.xml");
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)){
        QMessageBox::warning(NULL, "Warning!", "Failed to open file.");
    } else {
        QTextStream stream(&file);
        stream << w_document->toString();
        file.close();
    }
}

QDomElement Writer::getParent(Project *project, bool isStart){
    //search for user in doc, if not there, create it
    QDomNodeList userList = w_root->elementsByTagName(project->get_user());
    
    QDomElement userParent;
    if (userList.isEmpty()){
        QDomElement userChild = w_document->createElement(project->get_user());
        w_root->appendChild(userChild);
        userParent = userChild;
        
        userParent.setAttribute("Project_Quantity", double(0));
        
        QString user_quantity = w_root->attribute("Quantity");
        w_root->setAttribute("Quantity", user_quantity.toDouble() + double(1));
    } else {
        QDomNode itemnode = userList.at(0);
        userParent = itemnode.toElement();
    }
    
    
    //same goes for project name
    QDomNodeList nameList = userParent.elementsByTagName(project->get_name());
    
    QDomElement nameParent;
    if (nameList.isEmpty()){
        QDomElement nameChild = w_document->createElement(project->get_name());
        userParent.appendChild(nameChild);
        nameParent = nameChild;
        
        double session_quantity = 1.0;
        nameParent.setAttribute("Session_Quantity", session_quantity);
        
        QString project_quantity = userParent.attribute("Project_Quantity");
        userParent.setAttribute("Project_Quantity", project_quantity.toDouble() + double(1));
    } else {
        QDomNode itemnode = nameList.at(0);
        nameParent = itemnode.toElement();
    }
    //if no sessions exist, create first
    //if sessions exist, check if last session has ended yet
    //yes -> create new session
    //no -> add new date to last session
    //plus error handling (last session still active, no new session possible)
    QDomNodeList sessionList = nameParent.elementsByTagName("Session");
    
    QDomElement sessionParent;
    if (sessionList.isEmpty()){
        QDomElement sessionChild = w_document->createElement("Session");
        nameParent.appendChild(sessionChild);
        sessionChild.setAttribute("Nr", nameParent.attribute("Session_Quantity"));
        //sessionChild.setAttribute("Time", double(0));
        sessionParent = sessionChild;
    } else {
        QDomElement lastSession = nameParent.lastChildElement();
        QDomElement lastDate = lastSession.lastChildElement();
        
        if (lastDate.tagName() == "Start" && !isStart){
            sessionParent = lastSession;
        } else if ((lastDate.tagName() == "End" && isStart) || lastDate.tagName() == NULL){
            QString session_quantity = nameParent.attribute("Session_Quantity");
            nameParent.setAttribute("Session_Quantity", session_quantity.toDouble() + double(1));
            
            QDomElement sessionChild = w_document->createElement("Session");
            nameParent.appendChild(sessionChild);
            sessionChild.setAttribute("Nr", nameParent.attribute("Session_Quantity"));
            sessionParent = sessionChild;
        } else {
            noAcceptableDateType();
            
        }
    }
    return sessionParent;
}

Reader* Writer::getReader(){
    return w_reader;
}

void Writer::noAcceptableDateType(){
    QMessageBox error;
    
    error.setWindowTitle("Wrong Input");
    error.setText("You can't set two dates of the same type (start/end) in a row.");
    
    error.exec();
}

void Writer::updateDurations(Project *project){
    QDomElement user = w_root->firstChildElement(project->get_user());
    QDomElement name = user.firstChildElement(project->get_name());
    
    QDomNodeList sessionList = name.childNodes();
    int nameTimeDuration = 0;
    int nameDayDuration = 0;
    
    QTime zero(0,0);
    
    for (int i = 0; i < sessionList.count(); i++){
        QDomElement session = sessionList.at(i).toElement();
        QTime tmp_time = QTime::fromString(session.attribute("Time_Duration"), "hh:mm:ss");
        nameTimeDuration += (zero.secsTo(tmp_time) + session.attribute("Day_Duration").toInt()*86400);
    }
    
    if (nameTimeDuration > 86400){
        nameDayDuration = trunc(nameTimeDuration / 86400);
        nameTimeDuration = nameTimeDuration % 86400;
    } else {
        nameDayDuration = 0;
    }
    QTime time;
    time = zero.addSecs(nameTimeDuration);
    name.setAttribute("Time_Duration", time.toString("hh:mm:ss"));
    name.setAttribute("Day_Duration", nameDayDuration);
    
    
    QDomNodeList nameList = user.childNodes();
    int userTimeDuration = 0;
    int userDayDuration = 0;
    
    for (int i = 0; i < nameList.count(); i++){
        QDomElement name = nameList.at(i).toElement();
        QTime tmp_time = QTime::fromString(name.attribute("Time_Duration"), "hh:mm:ss");
        userTimeDuration += (zero.secsTo(tmp_time) + name.attribute("Day_Duration").toInt()*86400);
    }
    
    if (userTimeDuration > 86400){
        userDayDuration = trunc(userTimeDuration / 86400);
        userTimeDuration = userTimeDuration % 86400;
    } else {
        userDayDuration = 0;
    }
    
    time = zero.addSecs(userTimeDuration);
    user.setAttribute("Time_Duration", time.toString("hh:mm:ss"));
    user.setAttribute("Day_Duration", userDayDuration);
    
    
    
    QDomNodeList userList = w_root->childNodes();
    int rootTimeDuration = 0;
    int rootDayDuration = 0;
    
    for (int i = 0; i < userList.count(); i++){
        QDomElement user = userList.at(i).toElement();
        QTime tmp_time = QTime::fromString(user.attribute("Time_Duration"), "hh:mm:ss");
        rootTimeDuration += (zero.secsTo(tmp_time) + user.attribute("Day_Duration").toInt()*86400);
    }
    
    if (rootTimeDuration > 86400){
        rootDayDuration = trunc(rootTimeDuration / 86400);
        rootTimeDuration = rootTimeDuration % 86400;
    } else {
        rootDayDuration = 0;
    }
    
    time = zero.addSecs(rootTimeDuration);
    w_root->setAttribute("Time_Duration", time.toString("hh:mm:ss"));
    w_root->setAttribute("Day_Duration", rootDayDuration);
}