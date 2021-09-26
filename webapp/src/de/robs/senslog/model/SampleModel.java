package de.robs.senslog.model;

import java.util.ArrayList;
import java.util.List;

import javax.xml.bind.annotation.XmlAccessType;
import javax.xml.bind.annotation.XmlAccessorType;
import javax.xml.bind.annotation.XmlAttribute;
import javax.xml.bind.annotation.XmlElement;
import javax.xml.bind.annotation.XmlRootElement;

/**
 * A sample of a sensor at certain time.
 * @author Robert
 */
@XmlAccessorType(XmlAccessType.FIELD)
@XmlRootElement(name="sample")
public class SampleModel {

  @XmlAttribute(name="timestamp") private String timestamp;
  @XmlElement(name="value") private final List<ValueModel> values;

  public SampleModel() {
    this.values = new ArrayList<ValueModel>();
  }
  
  public SampleModel(String timestamp) {
    this.values = new ArrayList<ValueModel>();
    this.timestamp = timestamp;
  }

  public String getTimestamp() {
    return timestamp;
  }

  public List<ValueModel> getValues() {
    return values;
  }

  public void setTimestamp(String timestamp) {
    this.timestamp = timestamp;
  }

}
