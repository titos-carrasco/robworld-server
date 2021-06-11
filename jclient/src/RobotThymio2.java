package rcr.robworld;

import java.util.Map;
import java.util.ArrayList;
import javax.json.*;

public class RobotThymio2 extends RobotBase {
    private double[] proximitySensorValues = null;
    private double[] proximitySensorDistances = null;
    private double[] groundSensorValues = null;

    public RobotThymio2( String name, String host, int port ) throws Exception {
        super( name, host, port, "thymio2" );
    }

    public void getSensors()  throws Exception {
        JsonObject json = retrieveSensors();

        proximitySensorValues = getDoubleArray( json.getJsonArray( "proximitySensorValues" ) );
        proximitySensorDistances = getDoubleArray( json.getJsonArray( "proximitySensorDistances" ) );
        groundSensorValues = getDoubleArray( json.getJsonArray( "groundSensorValues" ) );
    }

    public double[]  getProximitySensorValues() {
        return proximitySensorValues;
    }

    public double[]  getProximitySensorDistances() {
        return proximitySensorDistances;
    }

    public double[] getGroundSensorValues() {
        return groundSensorValues;
    }

    public void setLedsIntensity( double[] leds ) throws Exception {
        JsonArrayBuilder jleds = Json.createArrayBuilder();
        for( double led : leds )
            jleds.add( led );

        JsonObject jobj = Json.createObjectBuilder()
            .add( "cmd", "setLedsIntensity" )
            .add( "leds", jleds.build() )
            .build();

        String resp = sendCommand( jobj, true );
    }
}
