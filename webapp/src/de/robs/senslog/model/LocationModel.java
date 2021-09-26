package de.robs.senslog.model;

import java.util.ArrayList;
import java.util.List;

import javax.xml.bind.annotation.XmlAccessType;
import javax.xml.bind.annotation.XmlAccessorType;
import javax.xml.bind.annotation.XmlAttribute;
import javax.xml.bind.annotation.XmlElement;
import javax.xml.bind.annotation.XmlRootElement;

/**
 * A sensor location at the base station.
 * @author Robert
 */
@XmlAccessorType(XmlAccessType.FIELD)
@XmlRootElement(name="location")
public final class LocationModel {

  @XmlAttribute(name="id") private String locationId;
  @XmlAttribute(name="name") private String locationName;
  @XmlElement(name="sensor") private final List<SensorModel> sensors;

  public LocationModel() {
    this.sensors = new ArrayList<SensorModel>();
  }

  public LocationModel(String id, String name) {
    this.sensors = new ArrayList<SensorModel>();
    this.locationId = id;
    this.locationName = name;
  }  
  
  public String getLocationId() {
    return locationId;
  }
  
  public String getLocationName() {
    return locationName;
  }

  public List<SensorModel> getSensors() {
    return sensors;
  }

  public void setLocationId(String locationId) {
    this.locationId = locationId;
  }

  public void setLocationName(String locationName) {
    this.locationName = locationName;
  }

  public String toString() {
    
    return this.getClass().getSimpleName() +
        ": name=" + locationName + ", sensors=" + sensors.toString();
    
  }
  
}
