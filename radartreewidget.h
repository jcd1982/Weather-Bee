#ifndef RADARTREEWIDGET_H
#define RADARTREEWIDGET_H

#include <QObject>
#include <QTreeWidget>

#include "scnetworkaccessmanager.h"

class RadarTreeWidget : public QTreeWidget
{
    Q_OBJECT

public:

    explicit RadarTreeWidget(QWidget *parent = 0);

    bool isValid();

private:

    void init();
    void reqRadarIDs();
    void parseRadarIDs(QString replyHtml, bool fallback=false);

    bool m_isValid;
    SCNetworkAccessManager *m_nam;
    std::map<QString, QList<QString>> m_map_station_list_by_state;

signals:

    void signalDoubleClick(QString);

private slots:
    void slotHandleNetworkReplies(QNetworkReply* reply);
    void slotEmitDoubleClick();

};

#endif // RADARTREEWIDGET_H
