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
public class TestSensorsObject {

    public void run() {
        // Los datos de conexion al playground
        String host = "127.0.0.1";
        int port = 44444;

        // Usamos try/catch para conocer los errores que se produzcan
        try {
            // Accesamos el robots
            MyThymio2 thymio = new MyThymio2( "Thymio-01", host, port );
            MyEPuck epuck = new MyEPuck( "Epuck-01", host, port );

            // Loop clasico
            long t = System.currentTimeMillis() / 1000;
            while( System.currentTimeMillis() / 1000 - t < 20 ) {
                double distanciaThymio = thymio.checkObstacle();
                double distanciaEpuck = epuck.checkObstacle();

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
        TestSensorsObject test = new TestSensorsObject();
        test.run();
    }


    class MyThymio2 extends RobotThymio2 {
        private int s = 10;

        public MyThymio2( String name, String host, int port ) throws Exception {
            super( name, host, port );
            setSpeed( s, s );
        }

        public double checkObstacle() throws Exception {
            getSensors();
            double distancia = getProximitySensorDistances()[2];
            if( distancia < 8 )
                setSpeed( -s*10, s );
            else
                setSpeed( s, s );
            return distancia;
        }

    }

    class MyEPuck extends RobotEPuck {
        private int s = 20;

        public MyEPuck( String name, String host, int port ) throws Exception {
            super( name, host, port );
            setSpeed( s, s );
        }

        public double checkObstacle() throws Exception {
            getSensors();
            double distancia = getProximitySensorDistances()[0];
            if( distancia < 8 )
                setSpeed( -s*10, s );
            else
                setSpeed( s, s );
            return distancia;
        }

    }

}
