package de.robs.senslog.model;

import java.util.ArrayList;
import java.util.List;

import javax.xml.bind.annotation.XmlAccessType;
import javax.xml.bind.annotation.XmlAccessorType;
import javax.xml.bind.annotation.XmlAttribute;
import javax.xml.bind.annotation.XmlElement;
import javax.xml.bind.annotation.XmlRootElement;

/**
 * A sensor at a location.
 * @author Robert
 */
@XmlAccessorType(XmlAccessType.FIELD)
@XmlRootElement(name="sensor")
public final class SensorModel {
  
  @XmlAttribute(name="id") private String sensorId;
  @XmlAttribute(name="name") private String sensorName;
  @XmlElement(name="sample") private final List<SampleModel> samples;
  
  public SensorModel() {
    this.samples = new ArrayList<SampleModel>();
  }

  public SensorModel(String id, String name) {
    this.samples = new ArrayList<SampleModel>();
    this.sensorId = id;
    this.sensorName = name;
  }  
  
  public String getSensorId() {
    return sensorId;
  }
  
  public String getSensorName() {
    return sensorName;
  }

  public List<SampleModel> getSamples() {
    return samples;
  }

  public void setSensorId(String sensorId) {
    this.sensorId = sensorId;
  }

  public void setSensorName(String name) {
    this.sensorName = name;
  }

  public String toString() {
    
    return this.getClass().getSimpleName() +
        ": name=" + sensorName;
    
  }
  
}
