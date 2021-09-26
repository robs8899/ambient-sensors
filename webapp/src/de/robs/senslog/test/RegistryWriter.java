package de.robs.senslog.test;

import java.util.UUID;

import javax.xml.bind.JAXBContext;
import javax.xml.bind.util.JAXBSource;
import javax.xml.transform.OutputKeys;
import javax.xml.transform.Source;
import javax.xml.transform.Transformer;
import javax.xml.transform.TransformerFactory;
import javax.xml.transform.stream.StreamResult;

import de.robs.senslog.config.LocationConfig;
import de.robs.senslog.config.SampleValueConfig;
import de.robs.senslog.config.SensorConfig;
import de.robs.senslog.config.StationConfig;

public class RegistryWriter {

  public static void main(String[] args) {

    try {

      StationConfig station = new StationConfig();
      station.setStationId(UUID.randomUUID().toString());
      station.setStationName("Sensor Station Gönnsdorf");
      station.setDspFmtTsLong("EEEE dd.MM.yyyy HH:mm:ss zzz");
      station.setDspFmtTsShort("dd.MM.yyyy HH:mm");
      station.setDspFmtSamples("#,##0.00");
      station.setDspLocale("de_DE");

      for (int i=0; i<2; i++) {

        LocationConfig location = new LocationConfig();
        location.setLocationId(UUID.randomUUID().toString());
        location.setLocationName("Außenbereich " + i);

        for (int k=0; k<2; k++) {

          SensorConfig sensor = new SensorConfig();
          sensor.setSensorId(UUID.randomUUID().toString());
          sensor.setSensorName("Sensor " + i + "-" + k);
          sensor.setLogPath("logs/sensor" + i + "-" + k);
          sensor.setLogLocale("en_US");
          sensor.setLogFmtTs("yyyy-MM-dd HH:mm:ss zzz");
          sensor.setLogFmtSamples("#0.0#");

          for (int m=0; m<2; m++) {

            SampleValueConfig sample = new SampleValueConfig();
            sample.setName("Label #" + i + "-" + k + "-" + m);
            sample.setUnit("Unit #" + i + "-" + k + "-" + m);
            
            sensor.getSampleValueConfig().add(sample);

          }

          location.getSensors().add(sensor);
          
        }

        station.getLocations().add(location);

      }

      JAXBContext ctx = JAXBContext.newInstance(StationConfig.class);
      TransformerFactory tf = TransformerFactory.newInstance();
      Transformer tr = tf.newTransformer();

      tr.setOutputProperty(OutputKeys.INDENT, "yes");
      tr.setOutputProperty("{http://xml.apache.org/xslt}indent-amount", "2");
      tr.setOutputProperty("{http://xml.apache.org/xalan}indent-amount", "2");

      Source source = new JAXBSource(ctx, station);
      StreamResult result = new StreamResult(System.out);
      tr.transform(source, result);

    } catch (Exception e) {

      e.printStackTrace();

    }

  }

}
