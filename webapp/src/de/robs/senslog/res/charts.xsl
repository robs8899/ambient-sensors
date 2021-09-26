<?xml version="1.0"?>
<xsl:stylesheet version="1.0"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns:xslt="http://xml.apache.org/xslt"
  xmlns:model="http://senslog.robs.de/model" xmlns="http://www.w3.org/1999/xhtml">

  <xsl:output omit-xml-declaration="yes" method="xml"
    encoding="utf-8" indent="yes" xslt:indent-amount="2"
    doctype-public="-//W3C//DTD XHTML 1.0 Transitional//EN"
    doctype-system="http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd" />

  <xsl:param name="contextPath" />
  <xsl:param name="requestURI" />

  <xsl:variable name="amp">
    <xsl:text disable-output-escaping="yes">&amp;</xsl:text>
  </xsl:variable>

  <xsl:template match="/">

    <html>

      <head>
        <meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
        <meta name="theme-color" content="#000000" />
        <meta name="viewport" content="width=device-width, initial-scale=1.0" />
        <meta name="google-site-verification" content="1PZSmB7k7WyS4n_tHtaRtNkxRqoZC_v0bVE49Jkz0I8" />
        <link rel="stylesheet" href="css/screen.css" />
        <title>Index</title>
      </head>

      <body>

        <div class="frameBox">

        <img class="bannerImage" src="{$contextPath}/img/bkgr3.jpg" />

          <h1>
            <table style="width:100%;"><tr>
            <td style="width:auto;text-align:left;"><xsl:value-of select="model:station/@name" /></td>
            <td style="width:40px;text-align:right;"><a href="{$requestURI}?target=index">
              <img src="{$contextPath}/img/home.png" style="margin:4px;vertical-align:middle;" />
            </a></td>
            </tr></table>
          </h1>

          <xsl:for-each select="model:station/model:location">

            <h2><xsl:value-of select="@name" /></h2>

            <xsl:for-each select="model:sensor">

              <div class="itemBox">

                <table class="tb0 wFull">
                  <tr>
                    <td style="width:auto;text-align:left;">
                      <h3><xsl:value-of select="@name" /></h3>
                    </td>
                    <td style="width:40px;text-align:right;">
                      <a href="{$requestURI}?target=sensor{$amp}id={@id}">
                        <img src="{$contextPath}/img/table.png" style="margin:2px;vertical-align:middle;" />
                      </a>
                    </td>
                  </tr>
                </table>

                <table class="tb1 wFull">
                  <tr class="tr1">
                    <td class="tdc" style="width:auto;">
                      <img src="{$contextPath}/chart.png?id={@id}" class="chartImage" />
                    </td>
                  </tr>
                </table>

              </div>

            </xsl:for-each>

          </xsl:for-each>

        </div>

      </body>

    </html>

  </xsl:template>

</xsl:stylesheet>

