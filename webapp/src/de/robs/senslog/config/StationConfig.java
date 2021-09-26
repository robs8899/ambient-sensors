package de.robs.senslog.config;

import java.util.ArrayList;
import java.util.List;

import javax.xml.bind.annotation.XmlAccessType;
import javax.xml.bind.annotation.XmlAccessorType;
import javax.xml.bind.annotation.XmlAttribute;
import javax.xml.bind.annotation.XmlElement;
import javax.xml.bind.annotation.XmlRootElement;

@XmlAccessorType(XmlAccessType.FIELD)
@XmlRootElement(name="station")
public class StationConfig {

  @XmlAttribute(name="id") private String stationId;
  @XmlAttribute(name="name") private String stationName;
  @XmlAttribute(name="dsp-locale") private String dspLocale;
  @XmlAttribute(name="dsp-fmt-tslng") private String dspFmtTsLong;
  @XmlAttribute(name="dsp-fmt-tssht") private String dspFmtTsShort;
  @XmlAttribute(name="dsp-fmt-smpls") private String dspFmtSamples;
  @XmlElement(name="location") private List<LocationConfig> locations;

  public StationConfig() {}

  public String getStationId() {
    return stationId;
  }

  public String getStationName() {
    return stationName;
  }

  public String getDspLocale() {
    return dspLocale;
  }

  public String getDspFmtTsLong() {
    return dspFmtTsLong;
  }

  public String getDspFmtTsShort() {
    return dspFmtTsShort;
  }

  public String getDspFmtSamples() {
    return dspFmtSamples;
  }

  public List<LocationConfig> getLocations() {
    if (locations == null) locations = new ArrayList<LocationConfig>();
    return locations;
  }
  
  public void setStationId(String stationId) {
    this.stationId = stationId;
  }

  public void setStationName(String stationName) {
    this.stationName = stationName;
  }

  public void setDspLocale(String dspLocale) {
    this.dspLocale = dspLocale;
  }

  public void setDspFmtTsLong(String dspFmtTsLong) {
    this.dspFmtTsLong = dspFmtTsLong;
  }

  public void setDspFmtTsShort(String dspFmtTsShort) {
    this.dspFmtTsShort = dspFmtTsShort;
  }

  public void setDspFmtSamples(String dspFmtSamples) {
    this.dspFmtSamples = dspFmtSamples;
  }

  public void setLocations(List<LocationConfig> locations) {
    this.locations = locations;
  }

}
