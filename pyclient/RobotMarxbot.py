import socket

from robworld import RobotBase

class RobotMarxbot( RobotBase.RobotBase ):
    """
    Clase "wrapper" para acceder a un robot remoto del tipo Marxbot

    Parameters
        name: nombre del robot a controlar en el simulador
        host: servidor en donde se encuenra este robot
        port: puerta en donde se encuentra este robot
    """
    tipo = "marxbot"

    def __init__( self, name:str, host:str, port:int  ):
        super().__init__( name, host, port )

    def getSensors( self ):
        """
        Actualiza el valor de los sensores del robot
        """
        resp = super().getSensors()
        self.virtualBumpers = tuple( resp["virtualBumpers"] )


    def __str__( self ):
        return f"RobotMarxbot >> name:{self.name} - host={self.host} - port={self.port}"
