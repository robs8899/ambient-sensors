package de.robs.senslog;

import javax.xml.bind.JAXBContext;
import javax.xml.bind.JAXBException;

import de.robs.senslog.config.StationConfig;
import de.robs.senslog.model.StationModel;

/**
 * Singleton to hold a static JAXB Context for the
 * the Station Model and the Station Config.
 * @author robert
 *
 */
public class BindingContext {

  private volatile static BindingContext instance;
  private final JAXBContext configContext;
  private final JAXBContext modelContext;

  private BindingContext() throws JAXBException {
    configContext = JAXBContext.newInstance(StationConfig.class);
    modelContext = JAXBContext.newInstance(StationModel.class);
  }

  public static synchronized BindingContext getInstance() throws JAXBException {
    if (instance == null) instance = new BindingContext();
    return instance;
  }

  public JAXBContext getConfigContext() {
    return configContext;
  }

  public JAXBContext getModelContext() {
    return modelContext;
  }

}
