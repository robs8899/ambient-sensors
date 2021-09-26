/**
 * @author Robert Schoch
 */
@XmlSchema(namespace = "http://senslog.robs.de/registry",
elementFormDefault = XmlNsForm.QUALIFIED, xmlns = {
    @XmlNs(prefix = "registry", namespaceURI = "http://senslog.robs.de/registry")
})
package de.robs.senslog.config;

import javax.xml.bind.annotation.XmlNs;
import javax.xml.bind.annotation.XmlNsForm;
import javax.xml.bind.annotation.XmlSchema;
