package de.robs.senslog;

import java.io.IOException;
import java.io.OutputStream;
import java.util.Map;
import java.util.Map.Entry;
import java.util.Properties;
import java.util.concurrent.ConcurrentHashMap;

import javax.xml.bind.JAXBException;
import javax.xml.bind.Marshaller;
import javax.xml.bind.util.JAXBSource;
import javax.xml.parsers.ParserConfigurationException;
import javax.xml.transform.Templates;
import javax.xml.transform.Transformer;
import javax.xml.transform.TransformerConfigurationException;
import javax.xml.transform.TransformerException;
import javax.xml.transform.TransformerFactory;
import javax.xml.transform.stream.StreamResult;
import javax.xml.transform.stream.StreamSource;

/**
 * Singleton for a Transformer.
 * @author robert
 *
 */
public class OutputRenderer {

  private static final String TEMPLATE_PROPS =
      "de/robs/senslog/res/templates.properties";
  
  private static OutputRenderer instance;
  private final TransformerFactory transformerFactory;
  private final Map<String, Templates> xslTemplates;

  private OutputRenderer() throws ParserConfigurationException,
  TransformerConfigurationException, IOException {

    transformerFactory = TransformerFactory.newInstance();
    xslTemplates = new ConcurrentHashMap<String, Templates>();

    Properties props = new Properties();
    ClassLoader cl = Thread.currentThread().getContextClassLoader();
    props.load(cl.getResourceAsStream(TEMPLATE_PROPS));

    for (Entry<Object, Object> prop : props.entrySet()) {
      xslTemplates.put((String)prop.getKey(), transformerFactory.newTemplates(
          new StreamSource(cl.getResourceAsStream((String)prop.getValue()))));
    }

  }

  public static synchronized OutputRenderer getInstance()
      throws TransformerConfigurationException, ParserConfigurationException, IOException {

    if (instance == null) instance = new OutputRenderer();
    return instance;

  }

  public void render(Object model, String xslKey, Map<String, String> param, OutputStream out)
      throws JAXBException, ParserConfigurationException, TransformerException {

    Templates t =  xslTemplates.get(xslKey);
    if (t==null) throw new IllegalArgumentException("Invalid key for XSL Templates.");
    Marshaller m = BindingContext.getInstance().getModelContext().createMarshaller();
    
    Transformer tr = t.newTransformer();
    if (param != null) for (Map.Entry<String, String> e : param.entrySet()) tr.setParameter(e.getKey(), e.getValue());
    tr.transform(new JAXBSource(m, model), new StreamResult(out));

  }

}
