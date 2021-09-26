package de.robs.senslog.webapp;

import java.io.IOException;
import java.math.BigInteger;
import java.security.MessageDigest;
import java.util.Enumeration;

import javax.servlet.ServletConfig;
import javax.servlet.ServletException;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;

public final class HoneyPotServlet extends HttpServlet {

  private static final long serialVersionUID = 8054487432225473539L;
  private static final Log LOG = LogFactory.getLog(HoneyPotServlet.class);

  public void init(ServletConfig config) throws ServletException {
    super.init(config);
  }

  protected void doGet(HttpServletRequest req, HttpServletResponse rsp)
      throws ServletException, IOException {

    try {

      LOG.info("GET: " + req.getRequestURI());

      Enumeration<?> names;
      names = req.getHeaderNames();
      
      while (names.hasMoreElements()) {
        String name = (String)names.nextElement();
        LOG.info("GET: header=" + name + ", value=" + req.getHeader(name));
      }
      
      MessageDigest md5 = MessageDigest.getInstance("MD5");
      String token = new BigInteger(1, md5.digest("Hello".getBytes())).toString(0x10);
      
      rsp.setContentType("text/html");
      rsp.getWriter().write("<html><head><title>Setup</title></head>"
          + "<body><h1>Login</h1>"
          + "<form action=\"#\" method=\"POST\">"
          + "<input type=\"hidden\" name=\"token\" value=\"" + token + "\" />"
          + "</form>"
          + "</body></html>");

    } catch (Exception e) {

      throw new ServletException(e.toString(), e);

    }

  }

  @Override
  protected void doPost(HttpServletRequest req, HttpServletResponse rsp)
      throws ServletException, IOException {

    try {

      LOG.info("POST: " + req.getRequestURI());
      LOG.info("POST: " + req.getContentType());
      
      Enumeration<?> names;
      names = req.getHeaderNames();
      
      while (names.hasMoreElements()) {
        String name = (String)names.nextElement();
        LOG.info("POST: header=" + name + ", value=" + req.getHeader(name));
      }
      
      names = req.getParameterNames();
      
      while (names.hasMoreElements()) {
        String name = (String)names.nextElement();
        LOG.info("POST: param=" + name + ", value=" + req.getParameter(name));
      }
      
      rsp.setContentType("text/html");
      rsp.getWriter().write("<html><head><title>Setup</title></head><body><h1>Success!</h1></body></html>");

    } catch (Exception e) {

      throw new ServletException(e.toString(), e);

    }
    
  }

}
