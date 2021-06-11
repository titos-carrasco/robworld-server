import socket

from robworld import RobotBase

class RobotEPuck( RobotBase.RobotBase ):
    """
    Clase "wrapper" para acceder a un robot remoto del tipo EPuck

    Parameters
        name: nombre del robot a controlar en el simulador
        host: servidor en donde se encuenra este robot
        port: puerta en donde se encuentra este robot
    """
    tipo = "epuck"

    def __init__( self, name:str, host:str, port:int ):
        super().__init__( name, host, port )

    def getSensors( self ):
        """
        Actualiza el valor de los sensores del robot
        """
        resp = super().getSensors()
        self.proximitySensorValues = tuple( resp["proximitySensorValues"] )
        self.proximitySensorDistances = tuple( resp["proximitySensorDistances"] )

    def setLedRing( self, on_off:bool ):
        """
        Apaga o enciende el anillo que rodea al robot

        Parameters
          on_off: True para encender, False para apagar
        """
        led_on = 1 if on_off else 0
        pkg = { "cmd":"setLedRing", "estado": led_on }
        resp = self.sendPkg( pkg )

    def getCameraImage( self ) -> list :
        """
        Obtiene la imagen de la camara lineal del robot.
        La imagen es de 60x1 pixeles

        Returns
            La magen lineal
        """
        pkg = { "cmd":"getCameraImage" }
        resp = self.sendPkg( pkg, bytes )
        l = len( resp )
        resp = [ tuple( resp[i:i+4] ) for i in range( 0, l, 4 ) ]
        return resp

    def __str__( self ):
        return f"RobotEnki >> name:{self.name} - host={self.host} - port={self.port}"
