import time
import threading

from robworld.RobotThymio2 import RobotThymio2

# Requiere simulador corriendo el "Pista01.world"
class Pista01():
    def __init__( self ):
        pass

    def run( self ):
        # los datos de conexion al simulador
        host = "127.0.0.1"
        port = 44444

        # creamos los robots
        thymio01 = Thymio01( "Thymio-01", host, port )
        thymio02 = Thymio02( "Thymio-02", host, port )

        # los levantamos en hilos separados
        threading.Thread( target=thymio01.run, args=(), name="Thymio-01" ).start()
        threading.Thread( target=thymio02.run, args=(), name="Thymio-02"  ).start()

        # loop clasico
        t = time.time()
        while( time.time() - t < 20 ):
            time.sleep( 0.0001 )

        # detenemos los robots
        thymio01.finish()
        thymio02.finish()


class Thymio01( RobotThymio2 ):
    def __init__( self, name, host, port ):
        super().__init__( name, host, port )
        self.running = False
        self.me = None

    def run( self ):
        self.me = threading.current_thread()
        speed = 50
        th = 700
        self.setSpeed( speed, speed )
        self.running = True
        while(self.running ):
            self.getSensors()
            l, r = self.groundSensorValues
            #print( "%05.1f %02.1f" % (l,r) )
            if( l>=th and r>=th ):
                self.setSpeed( -speed, -speed )
            elif( l>=th and r<th  ):
                self.setSpeed( speed, 0 )
            elif( l<th and r>=th ):
                self.setSpeed( 0, speed )
            else:
                self.setSpeed( speed, speed )
            time.sleep( 0.01 )
        self.setSpeed( 0, 0 )
        self.close()

    def finish( self ):
        self.running = False
        self.me.join()
        self.close()

class Thymio02( RobotThymio2 ):
    def __init__( self, name, host, port ):
        super().__init__( name, host, port )
        self.running = False
        self.me = None

    def run( self ):
        self.me = threading.current_thread()
        speed = 50
        th = 700
        self.setSpeed( speed, speed )
        self.running = True
        while(self.running ):
            self.getSensors()
            l, r = self.groundSensorValues
            #print( "%05.1f %02.1f" % (l,r) )
            if( l>=th and r>=th ):
                self.setSpeed( -speed, -speed )
            elif( l>=th and r<th  ):
                self.setSpeed( speed, 0 )
            elif( l<th and r>=th ):
                self.setSpeed( 0, speed )
            else:
                self.setSpeed( speed, speed )
            time.sleep( 0.01 )
        self.setSpeed( 0, 0 )
        self.close()

    def finish( self ):
        self.running = False
        self.me.join()
        self.close()


# show time
Pista01().run()
