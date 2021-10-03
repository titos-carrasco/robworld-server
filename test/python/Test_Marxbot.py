import time
import random

from robworld.RobotMarxbot import RobotMarxbot

# Requiere simulador corriendo el "marxbot.world"
class TestMarxbot():
    def __init__( self ):
        pass

    def run( self ):
        # los datos de conexion al simulador
        host = "127.0.0.1"
        port = 44444

        # usamos try/except para conocer los errores que se produzcan
        try:
            # Accesamos los robots
            rob = RobotMarxbot( "Marxbot-01", host, port )

            # loop clasico
            t = time.time()
            while( time.time() - t < 20 ):
                rob.setSpeed( random.uniform( -50, 50 ), random.uniform( -50, 50 ) )

                rob.getSensors()
                print( "pos:", rob.pos )
                print( "speed:", rob.speed )
                print( "angle:", rob.angle )
                print( "virtualBumpers:", rob.virtualBumpers[0] )

                time.sleep( 1 )
            rob.setSpeed( 0, 0 )
            rob.close()
        except ConnectionResetError:
            print( "Conexion abortada" )
        except Exception as e:
            print( e )


# show time
TestMarxbot().run()
