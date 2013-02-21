#ifndef ACCOUNTSMANAGER_H
#define ACCOUNTSMANAGER_H

#include <QObject>
#include <QList>
#include <QtXml>

class AccountsManager : public QObject
{
    Q_OBJECT
private:
    QDomDocument doc;
    QDomNode currentNode;
    void save();

public:
    explicit AccountsManager(int id, QObject *parent = 0);
    int currentId;
    QString configFile;
    bool oauthTokenFound();
    QList<int> idList();
    int addId(int id, QString name, QString oauth="", QString server="");
    int getNewIdNumber();
    QStringList nameList();
    QString getOAuthToken();
    void setOAuthToken(QString token);
    void setName(QString name, int id=0);
    QString getServer();
    QString getName();
    bool removeId(int id);

signals:
    
public slots:
    
};

#endif // ACCOUNTSMANAGER_H
