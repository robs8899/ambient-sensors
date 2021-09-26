package de.robs.senslog.webapp;

import java.util.concurrent.Executors;
import java.util.concurrent.ScheduledExecutorService;
import java.util.concurrent.TimeUnit;

import javax.servlet.ServletContextEvent;
import javax.servlet.ServletContextListener;

import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;

public class WebAppContextInitializer implements ServletContextListener {

  private static final Log LOG = LogFactory.getLog(WebAppContextInitializer.class);
  private ScheduledExecutorService scheduler;

  public WebAppContextInitializer() {
    LOG.info("<init>");
  }
  
  @Override
  public void contextInitialized(ServletContextEvent event) {

    LOG.info("contextInitialized...");
    scheduler = Executors.newSingleThreadScheduledExecutor();
    scheduler.scheduleAtFixedRate(new WebAppContextUpdater(
        event.getServletContext()), 0, 5, TimeUnit.MINUTES);

  }

  @Override
  public void contextDestroyed(ServletContextEvent event) {

    LOG.info("contextDestroyed...");
    scheduler.shutdownNow();    

  }
  
}
