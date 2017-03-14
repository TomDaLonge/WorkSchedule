#ifndef READER_H
#define	READER_H

#include <QtXml>
#include <QTreeWidget>

class Reader {
public:
    Reader();
    QDomDocument* getDocument();
    void createDocument();
    QTreeWidget* getTree();
    QDomElement* getRoot();
private:
    QDomDocument *r_document;
    QDomElement r_root;
    QTreeWidget *r_tree;
    
    void loadDocumentFailed();
};

#endif	/* READER_H */

