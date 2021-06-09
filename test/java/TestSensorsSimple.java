import rcr.robworld.RobotThymio2;
import rcr.robworld.RobotEPuck;

/*
Robot thymio2:
    Sensores de Proximidad:
        0: frontal izquierdo izquierdo
        1: frontal izquierdo
        2: frontal frontal
        3: frontal derecho
        4: frontal derecho derecho
        5: trasero izquierdo
        6: trasero derecho
    Sensores de línea:
        0: izquierdo
        1: derecho

Robot epuck  :
    Sensores de Proximidad:
        0: frontal frontal derecho
        1: frontal derecho
        2: derecho
        3: trasero derecho
        4: trasero izquierdo
        5: izquierdo
        6: frontal izquierdo
        7: frontal frontal izquierdo
*/

// Requiere servidor en el playground "simple.world"
public class TestSensorsSimple {

    public void run() {
        // Los datos de conexion al playground
        String host = "127.0.0.1";
        int port = 44444;

        // Usamos try/catch para conocer los errores que se produzcan
        try {
            // Accesamos el robot y configuramos algunos de sus atributos
            RobotThymio2 thymio = new RobotThymio2( "Thymio-01", host, port );
            RobotEPuck epuck = new RobotEPuck( "Epuck-01", host, port );

            thymio.setSpeed( 10, 10 );
            epuck.setSpeed( 10, 10 );

            // Loop clasico
            long t = System.currentTimeMillis() / 1000;
            while( System.currentTimeMillis() / 1000 - t < 20 ) {
                thymio.getSensors();
                epuck.getSensors();

                double distanciaThymio = thymio.getProximitySensorDistances()[2];
                double distanciaEpuck = epuck.getProximitySensorDistances()[0];

                if( distanciaThymio < 8 )
                    thymio.setSpeed( -120, 10 );
                else
                    thymio.setSpeed( 10, 10 );

                if( distanciaEpuck < 8 )
                    epuck.setSpeed( -10, 10 );
                else
                    epuck.setSpeed( 10, 10 );

                // mostramos el valor de los sensores utilizados
                System.out.printf( "thymio: %10.5f - epuck: %10.5f \n", distanciaThymio, distanciaEpuck );

                Thread.sleep( 1000 );
            }

            thymio.setSpeed( 0, 0 );
            epuck.setSpeed( 0, 0 );
            thymio.close();
            epuck.close();
        }
        catch( Exception e ) {
            System.out.println( e );
        }
    }

    public static void main( String[] args ) {
        TestSensorsSimple test = new TestSensorsSimple();
        test.run();
    }

}
