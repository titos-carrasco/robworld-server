import time
import socket
import threading
import json

class RobWorldProxy():
    def __init__( self ):
        pass

    def doGET( self, request, headers, robworld_server ):
        resp = "{}"

        # [GET, /robot/cmd/param/.., HTTP/1.x]
        parts = request[1].split( "/" )[1:]
        if( len( parts ) < 2 ): return resp

        # convertimos la url a json
        pkg = {}
        robot = parts[0]
        cmd = parts[1]
        try:
            if( cmd == "getSensors" ):
                pkg = { "cmd":"getSensors" }
            elif( cmd == "setSpeed" ):
                left = float( parts[2] )
                right = float( parts[3] )
                pkg = { "cmd":"setSpeed", "leftSpeed": left, "rightSpeed": right }
            elif( cmd == "setLedRing" ):
                led_on = int( parts[2] )
                pkg = { "cmd":"setLedRing", "estado": led_on }
            elif( cmd == "setLedsIntensity" ):
                leds = []
                for i in range( len(parts)-2 ):
                    leds.append( float( parts[2+i] ) )
                pkg = { "cmd":"setLedsIntensity", "leds": leds }
        except Exception as e:
            print( e )

        if( len( pkg ) == 0 ):
            return resp

        # enviamos el requerimiento al servidor robworld
        sock = socket.socket( socket.AF_INET, socket.SOCK_STREAM )
        sock.connect( robworld_server )

        try:
            connect = { "connect": robot }
            print( json.dumps( connect ) )
            sock.sendall( bytes( json.dumps( connect ) + "\n", "iso-8859-1" ) )
            tipo = self.readline( sock )
            print( tipo )

            print( json.dumps( pkg ) )
            sock.sendall( bytes( json.dumps( pkg ) + "\n", "iso-8859-1" ) )
            resp = self.readline( sock )
            print( resp )

        except Exception as e:
            print( e )

        try:
            sock.shutdown( socket.SHUT_RDWR )
        except Exception as e:
            pass
        sock.close()

        return resp


    def readline( self, conn ):
        nchars = 512*3
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
