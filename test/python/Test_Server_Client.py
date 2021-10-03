import time
import random

from robworld.RobotThymio2 import RobotThymio2
from robworld.RobotEPuck import RobotEPuck

# Requiere simulador corriendo el "simple.world"
class TestServerClient():
    def __init__( self ):
        pass

    def run( self ):
        # los datos de conexion al simulador
        host = "127.0.0.1"
        port = 44444

        # usamos try/except para conocer los errores que se produzcan
        try:
            # Accesamos los robots
            rob01 = RobotThymio2( "Thymio-01", host, port )
            rob02 = RobotEPuck( "Epuck-01", host, port )

            # loop clasico
            t = time.time()
            while( time.time() - t < 20 ):
                rob01.setSpeed( random.uniform(-100,100), random.uniform(-100,100) )
                rob02.setSpeed( random.uniform(-100,1000), random.uniform(-100,100) )

                rob02.getSensors()
                print( "pos:", rob02.pos )
                print( "speed:", rob02.speed )
                print( "angle:", rob02.angle )
                print( "proximitySensorValues:", rob02.proximitySensorValues )
                print( "proximitySensorDistances:" , rob02.proximitySensorDistances )

                time.sleep( 1 )
            rob01.setSpeed( 0, 0 )
            rob02.setSpeed( 0, 0 )
            rob01.close()
            rob02.close()
        except ConnectionResetError:
            print( "Conexion abortada" )
        except Exception as e:
            print( e )


# show time
TestServerClient().run()
