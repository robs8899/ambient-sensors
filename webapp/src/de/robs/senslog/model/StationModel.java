package de.robs.senslog.model;

import java.util.ArrayList;
import java.util.List;

import javax.xml.bind.annotation.XmlAccessType;
import javax.xml.bind.annotation.XmlAccessorType;
import javax.xml.bind.annotation.XmlAttribute;
import javax.xml.bind.annotation.XmlElement;
import javax.xml.bind.annotation.XmlRootElement;

/**
 * The base station.
 * @author Robert
 */
@XmlAccessorType(XmlAccessType.FIELD)
@XmlRootElement(name="station")
public class StationModel {

  @XmlAttribute(name="id") private String stationId;
  @XmlAttribute(name="name") private String stationName;
  @XmlElement(name="location") private final List<LocationModel> locations;

  public StationModel() {
    this.locations = new ArrayList<LocationModel>();
  }

  public StationModel(String id, String name) {
    this.locations = new ArrayList<LocationModel>();
    this.stationId = id;
    this.stationName = name;
  }

  public String getStationId() {
    return stationId;
  }

  public String getStationName() {
    return stationName;
  }

  public List<LocationModel> getLocations() {
    return locations;
  }

  public void setStationId(String stationId) {
    this.stationId = stationId;
  }

  public void setStationName(String stationName) {
    this.stationName = stationName;
  }

  public String toString() {

    return this.getClass().getSimpleName() +
        ": name=" + stationName + ", locations=" + locations.toString();

  }

}
