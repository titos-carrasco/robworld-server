import java.awt.Graphics;
import java.awt.Color;
import java.awt.Dimension;
import javax.swing.JFrame;
import javax.swing.JPanel;
import java.util.List;

import rcr.robworld.RobotEPuck;

// Requiere servidor en el playground "epuck.world"
public class TestCamera extends JPanel {

    private JFrame frame;
    private List<int[]> imagen;

    public TestCamera() {
        frame = new JFrame( "Test Camara EPuck" );
        frame.setDefaultCloseOperation( JFrame.EXIT_ON_CLOSE );

        setPreferredSize( new Dimension( 600, 80 ) );

        frame.getContentPane().add( this );
        frame.pack();
    }

    public void paint( Graphics g ) {
        super.paint(g);
        if( imagen == null ) return;

        List<int[]> img = imagen;
        for( int i=0; i<img.size(); i++ ) {
            int[] rgb = img.get(i);
            Color color = new Color( rgb[0], rgb[1], rgb[2] );
            g.setColor( color );
            g.fillRect( i*10, 0, 10, 79 );
        }
    }

    public void run() {
        // nos hacemos visible
        frame.setVisible( true );

        // Los datos de conexion al playground
        String host = "127.0.0.1";
        int port = 44444;

        // Usamos try/catch para conocer los errores que se produzcan
        try {
            // Accesamos el robot y configuramos algunos de sus atributos
            RobotEPuck epuck = new RobotEPuck( "Epuck-01", host, port );
            epuck.setLedRing( true );
            epuck.setSpeed( -5, 5 );

            // Loop clasico
            long t = System.currentTimeMillis() / 1000;
            while( System.currentTimeMillis() / 1000 - t < 20 ) {
                imagen = epuck.getCameraImage();
                repaint();
                Thread.sleep( 1 );
            }

            epuck.setSpeed( 0, 0 );
            epuck.close();
        }
        catch( Exception e ) {
            System.out.println( e );
        }

        frame.setVisible( false );
        frame.dispose();
    }

    public static void main( String[] args ) {
        TestCamera test = new TestCamera();
        test.run();
    }
}
