package de.robs.senslog.model;

import javax.xml.bind.annotation.XmlAccessType;
import javax.xml.bind.annotation.XmlAccessorType;
import javax.xml.bind.annotation.XmlAttribute;
import javax.xml.bind.annotation.XmlRootElement;

/**
 * A single value of a sample.
 * @author Robert
 */
@XmlAccessorType(XmlAccessType.FIELD)
@XmlRootElement(name="value")
public final class ValueModel {

  @XmlAttribute(name="name") private String name;
  @XmlAttribute(name="unit") private String unit;
  @XmlAttribute(name="value") private String value;

  public ValueModel() {}

  public ValueModel(String name, String unit, String value) {
    this.name = name;
    this.unit = unit;
    this.value = value;
  }

  public String getName() {
    return name;
  }

  public String getUnit() {
    return unit;
  }

  public String getValue() {
    return value;
  }

  public void setName(String name) {
    this.name = name;
  }

  public void setUnit(String unit) {
    this.unit = unit;
  }

  public void setValue(String value) {
    this.value = value;
  }

}
