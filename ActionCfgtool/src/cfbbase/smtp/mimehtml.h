﻿#ifndef MIMEHTML_H
#define MIMEHTML_H

#include "mimetext.h"

#include "smtpexports.h"

class SMTP_EXPORT MimeHtml : public MimeText
{
    Q_OBJECT
public:

    

    MimeHtml(const QString &html = "");
    ~MimeHtml();

    


    

    void setHtml(const QString & html);

    const QString& getHtml() const;

    

protected:

    

    


    

    virtual void prepare();

    
};

#endif // MIMEHTML_H
