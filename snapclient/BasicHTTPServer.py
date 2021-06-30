import time
import socket
import threading

from RobWorldProxy import RobWorldProxy

class BasicHTTPServer():
    def __init__( self, addr_me, params ):
        self.addr_me = addr_me
        self.params = params

    def run( self ):
        sock = socket.socket( socket.AF_INET, socket.SOCK_STREAM )
        sock.setsockopt( socket.SOL_SOCKET, socket.SO_REUSEADDR, 1 )
        sock.bind( self.addr_me )
        sock.listen( 5 )
        sock.settimeout( 2 )

        print( f"Esperando conexiones en {self.addr_me[0]}:{self.addr_me[1]}" )
        running = True
        while( running ):
            try:
                # recibe las conexiones entrantes
                conn, addr = sock.accept()

                # trata de procesarlas en un thread
                t = threading.Thread( target=self.httpRequest, args=( conn, addr, self.params ) )
                t.start()
            except socket.timeout as e:
                pass
            except Exception as e:
                print( e )
                running = False

        try:
            sock.shutdown( socket.SHUT_RDWR )
        except Exception as e:
            pass
        sock.close()

    def httpRequest( self, conn, addr, params ):
        print( "Procesando requerimiento desde", addr )
        conn.settimeout( 2 )

        try:
            # request GET /xx/xx/... HTTP/1.x
            line = self.readline( conn )[:-1]
            request = line.split( " " )
            if( len( request ) != 3 ): raise Exception( "Metodo invalido" )
            if( request[0] != "GET" ): raise Exception( "Metodo invalido" )
            if( request[2][:7] != "HTTP/1." ): raise Exception( "Metodo invalido" )

            # headers
            headers = []
            n = 0
            t = time.time()
            while( True ):
                header = self.readline( conn )[:-1]
                if( len( header ) == 0 ): break
                headers.append( header )

                n = n + 1
                if( n > 20 ):
                    raise Exception( "Demasiados encabezados" )

                if( time.time() - t > 4 ):
                    raise Exception( "Timeout en encabezados" )

            proxy = RobWorldProxy()
            resp = proxy.doGET( request, headers, params )

            # response
            conn.sendall( f"HTTP/1.1 200 OK\r\n".encode( "iso-8859-1" ) )
            conn.sendall( f"Access-Control-Allow-Origin: *\r\n".encode( "iso-8859-1" ) )
            conn.sendall( f"Access-Control-Allow-Headers: X-Requested-With, X-Application\r\n".encode( "iso-8859-1" ) )
            conn.sendall( f"Content-Type: text/plain; charset=iso-8859-1\r\n".encode( "iso-8859-1" ) )
            conn.sendall( f"Content-Length: {len( resp )}\r\n".encode( "iso-8859-1" ) )
            conn.sendall( f"\r\n".encode( "iso-8859-1" ) )
            conn.sendall( resp.encode( "iso-8859-1" ) )
        except Exception as e:
            print( e )

        print( "Finalizando requerimiento desde", addr )
        try:
            conn.shutdown( socket.SHUT_RDWR )
        except Exception as e:
            pass
        conn.close()

    def readline( self, conn ):
        nchars = 1024
        buff = bytearray( nchars )
        n = 0
        while( n < nchars ):
            c = conn.recv( 1 )
            if( c == b"" ):
                raise Exception( "Conexion fue cerrada remotamente" )
            if( c == b"\n" ):
                return buff[:n].decode( "iso-8859-1" )
            buff[n] = c[0]
            n += 1
        raise Exception( "Linea demasiado larga" )

#---
addr_http_server = ( '127.0.0.1', 8888 )
addr_robworld_server = ( '127.0.0.1', 44444 )

server = BasicHTTPServer( addr_http_server, addr_robworld_server )
server.run()
