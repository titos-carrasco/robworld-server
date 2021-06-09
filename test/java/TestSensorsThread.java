import java.lang.Thread;

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
public class TestSensorsThread {

    public void run() {
        // Los datos de conexion al playground
        String host = "127.0.0.1";
        int port = 44444;

        // Usamos try/catch para conocer los errores que se produzcan
        try {
            // Accesamos el robots
            MyThymio2 thymio = new MyThymio2( "Thymio-01", host, port );
            MyEPuck epuck = new MyEPuck( "Epuck-01", host, port );

            // los levantamoso en hilos separados
            Thread tThymio = new Thread( thymio, "Thymio-01" );
            Thread tEpuck = new Thread( epuck,"Epuck-01" );

            tThymio.start();
            tEpuck.start();

            // Loop clasico
            long t = System.currentTimeMillis() / 1000;
            while( System.currentTimeMillis() / 1000 - t < 20 ) {
                Thread.sleep( 1000 );
            }

            thymio.finish();
            epuck.finish();
        }
        catch( Exception e ) {
            System.out.println( e );
        }
    }

    public static void main( String[] args ) {
        TestSensorsThread test = new TestSensorsThread();
        test.run();
    }

    class MyThymio2 extends RobotThymio2 implements Runnable {
        private int s = 20;
        private Thread me = null;
        private boolean quit = false;
        private Object sync = new Object();

        public MyThymio2( String name, String host, int port ) throws Exception {
            super( name, host, port );
        }

        public void run() {
            me = Thread.currentThread();
            try {
                setSpeed( s, s );
                while( true ) {
                    getSensors();
                    double distancia = getProximitySensorDistances()[2];
                    if( distancia < 3 ) {
                        setSpeed( -s*10, s );
                        Thread.sleep( 1000 );
                        setSpeed( s, s );
                    }
                    boolean abort;
                    synchronized( sync ) { abort = quit; }
                    if( abort ) break;
                }
            }
            catch( Exception e ){}

            try {
                setSpeed( 0, 0 );
                close();
            }
            catch( Exception e ){}
        }

        public void finish() throws Exception {
            synchronized( sync ) { quit = true; }
            me.join();
        }

    }


    class MyEPuck extends RobotEPuck implements Runnable{
        private int s = 10;
        private Thread me = null;
        private boolean quit = false;
        private Object sync = new Object();

        public MyEPuck( String name, String host, int port ) throws Exception {
            super( name, host, port );
        }

        public void run() {
            me = Thread.currentThread();
            try {
                setSpeed( s, s );
                while( true ) {
                    getSensors();
                    double distanciaR = getProximitySensorDistances()[0];
                    double distanciaL = getProximitySensorDistances()[7];
                    if( distanciaL < 4 || distanciaR < 4 ) {
                        setSpeed( -s*10, s );
                        Thread.sleep( 1000 );
                        setSpeed( s, s );
                    }
                    boolean abort;
                    synchronized( sync ) { abort = quit; }
                    if( abort ) break;
                }
            }
            catch( Exception e ){}

            try {
                setSpeed( 0, 0 );
                close();
            }
            catch( Exception e ){}
        }

        public void finish() throws Exception {
            synchronized( sync ) { quit = true; }
            me.join();
        }

    }

}
