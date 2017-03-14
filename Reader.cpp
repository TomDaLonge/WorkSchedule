#include "Reader.h"
#include <QMessageBox>
#include <QTreeWidgetItem>

Reader::Reader() {
    r_document = new QDomDocument;
    QFile file(QDir::currentPath() + "/Schedule.xml");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        loadDocumentFailed();
        createDocument();
    } else {
        if(!r_document->setContent(&file)){
            QMessageBox::warning(NULL, "Warning!", "Failed to load document.");
        }
        file.close();
    }
}

QDomDocument* Reader::getDocument(){
    return r_document;
}

void Reader::loadDocumentFailed(){
    QMessageBox error;
    
    error.setWindowTitle("Warning.");
    error.setText("Failed to load file. Creating new file.");
    
    error.exec();
}

void Reader::createDocument(){
    delete r_document;
    r_document = new QDomDocument;
    r_root = r_document->createElement("User");
    r_document->appendChild(r_root);
    double user_quantity = 0.0;
    r_root.setAttribute("Quantity", user_quantity);
}

QDomElement* Reader::getRoot(){
    return &r_root;
}

QTreeWidget* Reader::getTree(){
    r_root = r_document->firstChildElement();
    
    delete r_tree;
    r_tree = new QTreeWidget;
    QTreeWidgetItem *rootItem = new QTreeWidgetItem;
    rootItem->setText(0, r_root.tagName());
    rootItem->setText(1, r_root.attribute("Quantity"));
    rootItem->setText(2, r_root.attribute("Day_Duration"));
    rootItem->setText(3, r_root.attribute("Time_Duration"));
    r_tree->addTopLevelItem(rootItem);
    
    QTreeWidgetItem* headerItem = new QTreeWidgetItem();
    headerItem->setText(0,"Name");
    headerItem->setText(1,"Elements");
    headerItem->setText(2,"Days");
    headerItem->setText(3,"Hours");
    headerItem->setText(4,"Date");
    headerItem->setText(5,"Time");
    r_tree->setHeaderItem(headerItem);
    r_tree->expandAll();
    
    r_tree->setColumnWidth(0, 150);
    r_tree->setColumnWidth(1, 60);
    r_tree->setColumnWidth(2, 35);
    r_tree->setColumnWidth(3, 60);
    
    QDomNodeList userList = r_root.childNodes();
    if (userList.isEmpty()){
        return r_tree;
    } else {
        for (int i = 0; i < userList.count(); i++){
            QDomNode userNode = userList.at(i);
            QDomElement userElement = userNode.toElement();
            QTreeWidgetItem *userItem = new QTreeWidgetItem;
            userItem->setText(0, userElement.tagName());
            userItem->setText(1, userElement.attribute("Project_Quantity"));
            userItem->setText(2, userElement.attribute("Day_Duration"));
            userItem->setText(3, userElement.attribute("Time_Duration"));
            rootItem->addChild(userItem);
        
            QDomNodeList projectList = userElement.childNodes();
        
            for (int i = 0 ; i < projectList.count(); i++){
                QDomNode projectNode = projectList.at(i);
                QDomElement projectElement = projectNode.toElement();
                QTreeWidgetItem *projectItem = new QTreeWidgetItem;
                projectItem->setText(0, projectElement.tagName());
                projectItem->setText(1, projectElement.attribute("Session_Quantity"));
                projectItem->setText(2, projectElement.attribute("Day_Duration"));
                projectItem->setText(3, projectElement.attribute("Time_Duration"));
                userItem->addChild(projectItem);

                QDomNodeList sessionList = projectElement.childNodes();
                
                for (int i = 0 ; i < sessionList.count(); i++){
                    QDomNode sessionNode = sessionList.at(i);
                    QDomElement sessionElement = sessionNode.toElement();
                    QTreeWidgetItem *sessionItem = new QTreeWidgetItem;
                    sessionItem->setText(0, sessionElement.tagName() + " " + sessionElement.attribute("Nr"));
                    projectItem->addChild(sessionItem);
                    
                    QDomNodeList dateList = sessionElement.childNodes();
                    
                    for (int i = 0 ; i < dateList.count(); i++){
                        QDomNode dateNode = dateList.at(i);
                        QDomElement dateElement = dateNode.toElement();
                        QTreeWidgetItem *dateItem = new QTreeWidgetItem;
                        dateItem->setText(0, dateElement.tagName());
                        dateItem->setText(4, dateElement.attribute("Date"));
                        dateItem->setText(5, dateElement.attribute("Time"));
                        sessionItem->addChild(dateItem);
                        
                        if (i == 1){
                            sessionItem->setText(2, sessionElement.attribute("Day_Duration"));
                            sessionItem->setText(3, sessionElement.attribute("Time_Duration"));
                        }
                    }
                }
            }
        }       
    }
    return r_tree;
}
