#ifndef OAUTHTOKENIZER_H
#define OAUTHTOKENIZER_H

#include <QObject>

class OAuthTokenizer : public QObject
{
    Q_OBJECT
public:
    explicit OAuthTokenizer(QObject *parent = 0);
    QString oauth_token;
    QString edam_shard;
    QString edam_userId;
    QString edam_expires;
    QString edam_noteStoreUrl;
    QString edam_webApiUrlPrefix;
    void tokenize(QString decoded);

    
signals:
    
public slots:
    
};

#endif // OAUTHTOKENIZER_H
