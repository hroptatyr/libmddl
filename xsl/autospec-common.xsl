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
      <xsl:otherwise>
        <xsl:value-of select="$type"/>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>

  <xsl:template name="make_type">
    <xsl:param name="type"/>
    <xsl:variable name="stem">
      <xsl:call-template name="make_stem">
        <xsl:with-param name="type" select="$type"/>
      </xsl:call-template>
    </xsl:variable>

    <xsl:choose>
      <xsl:when test="$stem = 'ID'">
        <xsl:text>mddl_ID_t</xsl:text>
      </xsl:when>
      <xsl:when test="$stem = 'mdString'">
        <xsl:text>mddl_mdString_t</xsl:text>
      </xsl:when>
      <xsl:when test="$stem = 'string'">
        <xsl:text>mddl_string_t</xsl:text>
      </xsl:when>
      <xsl:when test="$stem = 'mdDecimal'">
        <xsl:text>mddl_mdDecimal_t</xsl:text>
      </xsl:when>
      <xsl:when test="$stem = 'mdUri'">
        <xsl:text>mddl_mdUri_t</xsl:text>
      </xsl:when>
      <xsl:when test="$stem = 'anyURI'">
        <xsl:text>mddl_anyURI_t</xsl:text>
      </xsl:when>
      <xsl:when test="$stem = 'mdBoolean'">
        <xsl:text>mddl_mdBoolean_t</xsl:text>
      </xsl:when>
      <xsl:when test="$stem = 'mdDateTime'">
        <xsl:text>mddl_mdDateTime_t</xsl:text>
      </xsl:when>
      <xsl:when test="$stem = 'mdDuration'">
        <xsl:text>mddl_mdDuration_t</xsl:text>
      </xsl:when>
      <xsl:when test="$stem = 'mdInteger'">
        <xsl:text>mddl_mdInteger_t</xsl:text>
      </xsl:when>
      <xsl:when test="$stem = 'integer'">
        <xsl:text>mddl_integer_t</xsl:text>
      </xsl:when>
      <xsl:when test="$stem = 'mdNonNegativeDecimal'">
        <xsl:text>mddl_mdNonNegativeDecimal_t</xsl:text>
      </xsl:when>
      <xsl:when test="$stem = 'QualityEnumeration'">
        <xsl:text>mddl_QualityEnumeration_t</xsl:text>
      </xsl:when>
      <xsl:otherwise>
        <xsl:text>struct __</xsl:text>
        <xsl:value-of select="$stem"/>
        <xsl:text>_s</xsl:text>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>

</xsl:stylesheet>
