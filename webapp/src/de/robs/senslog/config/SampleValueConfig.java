package de.robs.senslog.config;

import javax.xml.bind.annotation.XmlAccessType;
import javax.xml.bind.annotation.XmlAccessorType;
import javax.xml.bind.annotation.XmlAttribute;
import javax.xml.bind.annotation.XmlRootElement;

@XmlAccessorType(XmlAccessType.FIELD)
@XmlRootElement(name="sample-value")
public class SampleValueConfig {

  @XmlAttribute(name="ignored") private boolean ignored;
  @XmlAttribute(name="name") private String name;
  @XmlAttribute(name="unit") private String unit;

  public SampleValueConfig() {}

  public boolean isIgnored() {
    return ignored;
  }

  public String getName() {
    return name;
  }

  public String getUnit() {
    return unit;
  }

  public void setIgnored(boolean ignored) {
    this.ignored = ignored;
  }

  public void setName(String name) {
    this.name = name;
  }

  public void setUnit(String unit) {
    this.unit = unit;
  }

}
