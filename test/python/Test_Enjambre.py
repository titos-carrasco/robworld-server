import time
import random
import subprocess

from robworld.RobotThymio2 import RobotThymio2

# Requiere simulador corriendo el "enjambre.world"
class TestEnjambre():
    def __init__( self ):
        pass

    def run( self ):
        # Levantamos el simulador en otro proceso
        try:
            #pg = subprocess.Popen( [ "robworld", "-m", "../worlds/enjambre.world" ], shell=False )
            time.sleep( 1 )
        except Exception as e:
            print( e )
            exit()

        # Los datos de conexion al simulador
        host = "127.0.0.1"
        port = 44444

        # Usamos try/except para conocer los errores que se produzcan
        try:
            # Accesamos los robots del enjambre
            robots = []
            for p in range( 44444, 44480 ):  # 44480
                name = f"Thymio-{p}"
                r = RobotThymio2( name, host, port )
                robots.append( r)

            # Loop clasico
            t = time.time()
            while( time.time() - t < 30 ):
                for r in robots:
                    r.setSpeed( random.uniform( -1000,1000 ), random.uniform( -1000,1000 ) )
                time.sleep( 2 )
            for r in robots:
                r.setSpeed( 0, 0 )
                r.close()
        except ConnectionResetError:
            print( "Conexion abortada" )
        except Exception as e:
            print( e )

        # Detenemos el simulador
        #time.sleep( 5 )
        #pg.send_signal( subprocess.signal.SIGTERM )


# show time
TestEnjambre().run()
