import rcr.robworld.RobotThymio2;
import rcr.robworld.RobotEPuck;

// Requiere servidor en el playground "simple.world"
public class TestLed {

    public void run() {
        // Los datos de conexion al playground
        String host = "127.0.0.1";
        int port = 44444;

        // Usamos try/catch para conocer los errores que se produzcan
        try {
            // Accesamos el robot y configuramos algunos de sus atributos
            RobotThymio2 thymio = new RobotThymio2( "Thymio-01", host, port );
            RobotEPuck epuck = new RobotEPuck( "Epuck-01", host, port );

            int status = 0;
            double[] leds = new double[23];

            // Loop clasico
            long t = System.currentTimeMillis() / 1000;
            while( System.currentTimeMillis() / 1000 - t < 20 ) {
                for( int i=3; i<23; i++ )
                    leds[i] = status;
                thymio.setLedsIntensity( leds );
                epuck.setLedRing( status == 0 );

                status = status == 0 ? 1 : 0;
                Thread.sleep( 200 );
            }

            thymio.close();
            epuck.close();
        }
        catch( Exception e ) {
            System.out.println( e );
        }
    }

    public static void main( String[] args ) {
        TestLed test = new TestLed();
        test.run();
    }

}
