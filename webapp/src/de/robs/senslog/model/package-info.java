/**
 * @author Robert Schoch
 */
@XmlSchema(namespace = "http://senslog.robs.de/model",
elementFormDefault = XmlNsForm.QUALIFIED, xmlns = {
    @XmlNs(prefix = "model", namespaceURI = "http://senslog.robs.de/model")
})
package de.robs.senslog.model;

import javax.xml.bind.annotation.XmlNs;
import javax.xml.bind.annotation.XmlNsForm;
import javax.xml.bind.annotation.XmlSchema;
