#ifndef WRITER_H
#define	WRITER_H

#include <QtXml>
#include "Project.h"
#include "Reader.h"

class Writer {
public:
    Writer();
    void writeNewDatePoint(Project *project, bool isStart);
    void clearDocument();
    Reader* getReader();
private:
    QDomDocument *w_document;
    QDomElement *w_root;
    Reader* w_reader;

    void Write();
    QDomElement getParent(Project *project, bool isStart);
    void noAcceptableDateType();
    void updateDurations(Project *project);

};

#endif	/* WRITER_H */

