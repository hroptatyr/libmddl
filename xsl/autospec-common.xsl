<?xml version="1.0" encoding="utf-8"?>
<xsl:stylesheet
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  version="1.0">

  <xsl:template name="make_stem">
    <xsl:param name="type"/>

    <xsl:choose>
      <xsl:when test="starts-with($type, 'mddl:')">
        <xsl:value-of select="substring($type, 6)"/>
      </xsl:when>
      <xsl:when test="starts-with($type, 'xsd:')">
        <xsl:value-of select="substring($type, 5)"/>
      </xsl:when>
      <xsl:when test="starts-with($type, 'xml:')">
        <xsl:value-of select="substring($type, 5)"/>
      </xsl:when>
      <xsl:when test="not($type)">
        <xsl:text>mdString</xsl:text>
      </xsl:when>
      <xsl:otherwise>
        <xsl:value-of select="$type"/>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>

  <xsl:template name="make_type">
    <xsl:param name="type"/>

    <xsl:choose>
      <xsl:when test="starts-with($type, 'mddl:') or
                      starts-with($type, 'xsd:') or
                      starts-with($type, 'xml:')">
        <xsl:value-of select="translate($type, ':', '_')"/>
        <xsl:text>_t</xsl:text>
      </xsl:when>
      <xsl:otherwise>
        <xsl:text>struct __</xsl:text>
        <xsl:value-of select="$type"/>
        <xsl:text>_s</xsl:text>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>

  <xsl:template name="make_tag">
    <xsl:param name="name"/>
    <xsl:text>MDDL_TAG_</xsl:text>
    <xsl:value-of select="$name"/>
  </xsl:template>

  <xsl:template name="make_attr">
    <xsl:param name="name"/>
    <xsl:text>MDDL_ATTR_</xsl:text>
    <xsl:value-of select="$name"/>
  </xsl:template>

</xsl:stylesheet>
