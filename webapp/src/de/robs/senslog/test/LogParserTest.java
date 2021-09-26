package de.robs.senslog.test;

import java.util.Date;

import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;

import de.robs.senslog.webapp.LogParser;

public class LogParserTest {

  private static final Log LOG = LogFactory.getLog(LogParserTest.class);

  public static void main(String[] args) {

    try {

      LogParser parser = new LogParser("WebContent/WEB-INF/log/bmp180",
          "yyyyMMdd'.log'", "yyyy-MM-dd HH:mm:ss zzz", "#0.0#", "en");
      parser.parse(new Date());

      for (int i=0; i<parser.getCount(); i++)
        LOG.info(parser.getAt(i));

      LOG.info(" --- max/min/mean ---");
      LOG.info("max: " + parser.getMax(0));
      LOG.info("min: " + parser.getMin(0));
      LOG.info("mean: " + parser.getMean());

    } catch (Exception e) {

      LOG.error(e, e);

    }

  }

}
