import socket
import json

class RobotBase():
    """
    Clase base de los robots

    Parameters
        name: nombre del robot a controlar en el simulador
        host: servidor en donde se encuenra este robot
        port: puerta en donde se encuentra este robot
    """

    def __init__( self, name:str, host:str, port:int ):
        self.name = name
        self.host = host
        self.port = port
        self.sock = None
        self.buff = bytearray( 512*3 )

        self.pos = None
        self.speed = None
        self.angle = None

        self.connect( name, host, port )

    def close( self ):
        """
        Cierra la conexion con el robot
        """
        if( self.sock is None ): return
        self.sock.shutdown( socket.SHUT_RDWR )
        self.sock.close()
        self.name = None
        self.host = None
        self.port = None
        self.sock = None
        self.buff = None

    def getName( self ) -> str:
        """
        Obtiene el nombre del robot

        Return
            El nombre del robot
        """
        return self.name

    def setSpeed( self, left:int, right:int ):
        """
        Cambia la velocidad de las ruedas del robot

        Parameters
            leftSpeed : valor para la rueda izquierda
            rightSpeed: valor para la rueda derecha
        """
        pkg = { "cmd":"setSpeed", "leftSpeed": left, "rightSpeed": right }
        resp = self.sendPkg( pkg )

    def getSensors( self ) -> dict :
        """
        Obtiene el valor de los sensores del robot

        Return
            Los sensores del robot y sus valores
        """
        pkg = { "cmd":"getSensors" }
        resp = self.sendPkg( pkg )
        self.pos = tuple( resp["pos"] )
        self.speed = tuple( resp["speed"]  )
        self.angle = resp["angle"]
        return resp

    def connect(self, name:str, host:str, port:int ):
        """
        Conecta a un robot del simulador

        El metodo es interno

        Parameters
          name: Nombre del robot a conectar
          host: Servidor en donde se encuentra el simulador
          port: Puerta en donde escucha el simulador
        """
        if( host == "" ): host = "0.0.0.0"
        try:
            # nos conectamos al servidor
            self.sock = socket.socket( socket.AF_INET, socket.SOCK_STREAM )
            self.sock.connect( ( host, port ) )

            # pedimos conexion al robot
            pkg = { "connect": name }
            resp = self.sendPkg( pkg )

            # validamos la respuesta
            if( resp["type"] != self.tipo ):
                raise Exception( f"Robot '{name}' no aceptado" )
        except Exception as e:
            try:
                self.sock.shutdown( socket.SHUT_RDWR )
                self.sock.close()
            except Exception:
                pass
            self.sock = None
            raise


    def sendPkg( self, pkg:dict, clase:type=dict ) -> object:
        self.sock.sendall( bytes( json.dumps( pkg ) + "\n", "iso-8859-1" ) )
        if( clase is bytes ):
            resp = self.readbytes()
        elif( clase is dict ):
            resp = self.readline()
            resp = json.loads( resp )
        else:
            raise Exception( "Clase a recuperar es invalida" )
        return resp

    def readline( self ) -> str:
        """
        Lee una linea desde el socket

        Este metodo es interno a la clase

        Parameters
            conn: el socket desde el cual leer

        Return
            La linea leida
        """
        ll = len( self.buff )
        n = 0
        while( n < ll ):
            c = self.sock.recv(1)
            if( c == b"" ): return ""       # la conexion fue cerrada remotamente
            if( c == b"\n" ): break         # fin de linea
            self.buff[n] = ord( c )
            n += 1
        return self.buff[:n].decode( "iso-8859-1" )

    def readbytes( self ) -> bytearray:
        size = bytearray( 4 )
        for i in range(4):
            c = self.sock.recv(1)
            if( c == b"" ): return bytes()  # la conexion fue cerrada remotamente
            size[i] = ord(c)
        size = int.from_bytes( size, byteorder="big" )

        data = bytearray( size )
        for i in range(size):
            c = self.sock.recv(1)
            if( c == b"" ): return bytes()  # la conexion fue cerrada remotamente
            data[i] = ord(c)
        return data


    def __str__( self ):
        return f"RobotControl >> name:{self.name} - host={self.host} - port={self.port}"
