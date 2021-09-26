package de.robs.senslog.webapp;

import java.io.ByteArrayOutputStream;
import java.io.InputStream;
import java.util.Date;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import javax.servlet.ServletContext;
import javax.xml.bind.Unmarshaller;
import javax.xml.transform.stream.StreamSource;

import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;
import org.jfree.chart.ChartUtilities;

import de.robs.senslog.BindingContext;
import de.robs.senslog.config.LocationConfig;
import de.robs.senslog.config.SampleValueConfig;
import de.robs.senslog.config.SensorConfig;
import de.robs.senslog.config.StationConfig;
import de.robs.senslog.model.LocationModel;
import de.robs.senslog.model.SensorModel;
import de.robs.senslog.model.StationModel;

public class WebAppContextUpdater implements Runnable {

  private static final Log LOG = LogFactory.getLog(WebAppContextUpdater.class);

  private final ServletContext servletContext;
  private final StationConfig stationConfig;

  public WebAppContextUpdater(ServletContext ctx) {

    try {

      LOG.info("<init>");
      servletContext = ctx;

      InputStream is = servletContext.getResourceAsStream("/WEB-INF/station.xml");
      Unmarshaller um = BindingContext.getInstance().getConfigContext().createUnmarshaller();
      stationConfig = (StationConfig)um.unmarshal(new StreamSource(is));

    } catch (Exception e) {

      throw new RuntimeException(
          "Error initializing the WebAppContextUpdater.", e);

    }

  }

  @Override
  public void run() {

    try {

      LOG.info("starting...");

      // The three models we put as Servlet Attributes

      StationModel stationModel = new StationModel();
      Map<String, SensorModel> sensorSeriesMap = new HashMap<String, SensorModel>();
      Map <String, byte[]> sensorChartMap = new HashMap<String,byte[]>();

      stationModel.setStationId(stationConfig.getStationId());
      stationModel.setStationName(stationConfig.getStationName());

      SampleBuilder sampleBuilder = new SampleBuilder(stationConfig);
      Date now = new Date();

      for (LocationConfig lcfg : stationConfig.getLocations()) {

        LocationModel location = new LocationModel();
        location.setLocationId(lcfg.getLocationId());
        location.setLocationName(lcfg.getLocationName());

        for (SensorConfig scfg : lcfg.getSensors()) {

          // TODO use this parser instance for all!

          LogParser parser = new LogParser(scfg.getLogPath(), scfg.getLogFile(),
              scfg.getLogFmtTs(), scfg.getLogFmtSamples(), scfg.getLogLocale());
          parser.parse(now);

          // The current sample for the station model

          SensorModel sensor = new SensorModel();
          sensor.setSensorId(scfg.getSensorId());
          sensor.setSensorName(scfg.getSensorName());
          sensor.getSamples().add(sampleBuilder.create(parser.getLast(), scfg));
          location.getSensors().add(sensor);

          // All samples for the series model

          SensorModel sensorSeriesModel = new SensorModel();
          sensorSeriesModel.setSensorId(scfg.getSensorId());
          sensorSeriesModel.setSensorName(scfg.getSensorName());
          sensorSeriesModel.getSamples().addAll(sampleBuilder.create(parser.getAll(), scfg));
          sensorSeriesMap.put(scfg.getSensorId(), sensorSeriesModel);

          // The chart for this sensor id

          ChartBuilder chartBuilder = new ChartBuilder(stationConfig);
          List<SampleValueConfig> svcl = scfg.getSampleValueConfig();

          for (int i=0; i<svcl.size(); i++) {

            SampleValueConfig svc = svcl.get(i);
            if (svc.isIgnored()) continue;
            
            chartBuilder.addValueAxis(svc.getName() + " (" + svc.getUnit() + ")", svc.getName());
            List<LogEntry> logEntries = parser.getAll();
            
            for (LogEntry logEntry:logEntries) chartBuilder.addValue(
                logEntry.getTimeStamp(), logEntry.getValues()[i]);

          }

          ByteArrayOutputStream bos = new ByteArrayOutputStream();
          ChartUtilities.writeChartAsPNG(bos, chartBuilder.getChart(), 640, 380);
          sensorChartMap.put(scfg.getSensorId(), bos.toByteArray());

        }

        stationModel.getLocations().add(location);

      }

      servletContext.setAttribute("stationModel", stationModel);
      servletContext.setAttribute("sensorSeriesMap", sensorSeriesMap);
      servletContext.setAttribute("sensorChartMap", sensorChartMap);

      LOG.info("done.");

    } catch (Exception e) {

      LOG.error(e, e);

    }

  }

}
