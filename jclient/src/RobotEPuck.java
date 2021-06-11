package rcr.robworld;

import java.util.Map;
import java.util.ArrayList;
import java.util.List;
import javax.json.*;

public class RobotEPuck extends RobotBase {
    private double[] proximitySensorValues = null;
    private double[] proximitySensorDistances = null;

    public RobotEPuck( String name, String host, int port ) throws Exception {
        super( name, host, port, "epuck" );
    }

    public void setLedRing( boolean on_off ) throws Exception {
        JsonObject jobj = Json.createObjectBuilder()
            .add( "cmd", "setLedRing" )
            .add( "estado", on_off ? 1 : 0 )
            .build();

        String resp = sendCommand( jobj, true );
    }

    public void getSensors() throws Exception {
        JsonObject json = retrieveSensors();

        proximitySensorValues = getDoubleArray( json.getJsonArray( "proximitySensorValues" ) );
        proximitySensorDistances = getDoubleArray( json.getJsonArray( "proximitySensorDistances" ) );
    }

    public double[]  getProximitySensorValues() {
        return proximitySensorValues;
    }

    public double[]  getProximitySensorDistances() {
        return proximitySensorDistances;
    }

    public List<int[]> getCameraImage() throws Exception {
        JsonObject jobj = Json.createObjectBuilder()
            .add( "cmd", "getCameraImage" )
            .build();

        sendCommand( jobj, false );

        int[] s = new int[4];
        for( int i = 0; i<4; i++ )
            s[i] = in.read();

        int len = (s[0]<<24) + (s[1]<<16) + (s[2]<<8) + s[3];

        int[] data = new int[len];
        for( int i = 0; i<len; i++ )
            data[i] = in.read();

        List<int[]> imagen = new ArrayList<>();
        for( int i=0; i<len; i+= 4 ) {
            int[] color = { data[i+0], data[i+1], data[i+2], data[i+3] };
            imagen.add( color );
        }

        return imagen;
    }

}
