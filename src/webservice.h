#ifndef WEBSERVICE_H
#define WEBSERVICE_H

#include <QHttp>
#include <QBuffer>
#include <QStringList>


#include <QSsl>
#include <QSslSocket>


class WebService: public QHttp
{
    Q_OBJECT
public:
    WebService( QObject *parent = 0 );

    QStringList getLastResult() const;

    void addCaCertificate( const QString& certFilePath, const QSsl::EncodingFormat& encoding = QSsl::Pem );


public slots:
    const bool request( const QString &url );

protected slots:
    void handleServerResponse( int id, bool error );

signals:
    void response( const QStringList& buffer );

private:
    QBuffer      _buffer;
    int          _requestId;
    QStringList  _lastResult;

    QSslSocket  *_sslSocket;

};

#endif // WEBSERVICE_H
