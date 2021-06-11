import time
import random
import subprocess

from robworld.RobotMarxbot import RobotMarxbot

# Requiere simulador corriendo el "marxbot.world"
class TestMarxbot():
    def __init__( self ):
        pass

    def run( self ):
        # levantamos el simulador en otro proceso
        try:
            #pg = subprocess.Popen( [ "robworld", "../worlds/marxbot.world" ], shell=False )
            time.sleep( 1 )
        except Exception as e:
            print( e )
            exit()

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
                print( "virtualBumpers:", rob.virtualBumpers[0] )

                time.sleep( 1 )
            rob.setSpeed( 0, 0 )
            rob.close()
        except ConnectionResetError:
            print( "Conexion abortada" )
        except Exception as e:
            print( e )

        # detenemos el simulador
        #pg.send_signal( subprocess.signal.SIGTERM )


# show time
TestMarxbot().run()
