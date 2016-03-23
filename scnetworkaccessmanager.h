#ifndef SCNETACCESS_H
#define SCNETACCESS_H

#include <QNetworkAccessManager>
#include <QNetworkRequest>

// TODO: document this, provide attributation.

class SCNetworkAccessManager : public QNetworkAccessManager
{
    Q_OBJECT

public:
    SCNetworkAccessManager(QObject *parent);
    ~SCNetworkAccessManager();

protected:
    virtual QNetworkReply *	createRequest ( Operation op, const QNetworkRequest & req, QIODevice * outgoingData = 0 );
private:
   QNetworkRequest m_request;
   QString m_url;
};

#endif // SCNETACCESS_H
