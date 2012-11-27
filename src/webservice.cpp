#include "webservice.h"
#include <QFile>
#include <QSslCertificate>
#include <QUrl>

WebService::WebService( QObject *parent )
    :QHttp( parent )
{
    _sslSocket = new QSslSocket( this );
    setSocket( _sslSocket );

    connect( this, SIGNAL( requestFinished ( int, bool ) ), this, SLOT( handleServerResponse( int, bool ) ) );
}

QStringList WebService::getLastResult() const
{
    return _lastResult;
}

void WebService::addCaCertificate( const QString& certFilePath, const QSsl::EncodingFormat& encoding )
{
    QFile certFile( certFilePath );
    if ( certFile.open( QIODevice::ReadOnly ) )
    {
        QSslCertificate cert( &certFile, encoding );
        _sslSocket->addCaCertificate( cert );
    }
}

void WebService::handleServerResponse( int id, bool error )
{

    if ( id != this->_requestId )
    {
        return;
    }

    //
    // Read the data from the server response
    // Format
    //   ErrorCode\n
    //   Authorization Code
    //
    _lastResult.clear();
    _buffer.seek( 0 );
    while( !_buffer.atEnd() )
    {
        QString line( _buffer.readLine() );

        if ( !line.length() )
        {
            break;
        }

        line = line.remove( "\n" );
        _lastResult.append( line );
    }

    this->_buffer.close();
    QHttp::close();

    emit response( _lastResult );
}

const bool WebService::request( const QString &url )
{
    if ( this->_buffer.open( QIODevice::ReadWrite ) )
    {
        QUrl tmpUrl( url );

        QHttp::setHost( tmpUrl.host() );
        this->_requestId = QHttp::get( url, &this->_buffer );

        return true;
    }

    return false;
}
