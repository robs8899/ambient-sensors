package de.robs.senslog.config;

import java.util.ArrayList;
import java.util.List;

import javax.xml.bind.annotation.XmlAccessType;
import javax.xml.bind.annotation.XmlAccessorType;
import javax.xml.bind.annotation.XmlAttribute;
import javax.xml.bind.annotation.XmlElement;
import javax.xml.bind.annotation.XmlRootElement;

@XmlAccessorType(XmlAccessType.FIELD)
@XmlRootElement(name="location")
public class LocationConfig {

  @XmlAttribute(name="id") private String locationId;
  @XmlAttribute(name="name") private String locationName;
  @XmlElement(name="sensor") private List<SensorConfig> sensors;

  public LocationConfig() {}

  public String getLocationId() {
    return locationId;
  }

  public String getLocationName() {
    return locationName;
  }

  public List<SensorConfig> getSensors() {
    if (sensors == null) sensors = new ArrayList<SensorConfig>();
    return sensors;
  }

  public void setLocationId(String locationId) {
    this.locationId = locationId;
  }

  public void setLocationName(String locationName) {
    this.locationName = locationName;
  }

  public void setSensors(List<SensorConfig> sensors) {
    this.sensors = sensors;
  }

}
