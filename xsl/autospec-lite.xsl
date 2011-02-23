<?xml version="1.0" encoding="utf-8"?>
<xsl:stylesheet
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  xmlns:xsd="http://www.w3.org/2001/XMLSchema"
  xmlns:mddl="http://www.mddl.org/mddl/3.0-beta"
  xmlns:xlink="http://www.w3.org/1999/xlink"
  version="1.0">

  <xsl:output method="text"/>

  <xsl:template match="/xsd:schema">
    <xsl:apply-templates/>
  </xsl:template>

  <xsl:template match="xsd:element[@type]">
    <xsl:variable name="type" select="@type"/>
    <xsl:variable name="type_nons" select="substring($type, 6)"/>

    <xsl:text>struct __</xsl:text>
    <xsl:value-of select="@name"/>
    <xsl:text>&#0009;</xsl:text>
    <xsl:value-of select="$type"/>
    <xsl:text>{</xsl:text>
    <xsl:text>&#0010;</xsl:text>

    <xsl:apply-templates mode="porn"/>

    <xsl:apply-templates
      select="/xsd:schema/xsd:complexType[@name=$type_nons]" mode="porn"/>
    <xsl:text>}</xsl:text>
    <xsl:text>&#0010;</xsl:text>
  </xsl:template>

  <xsl:template match="xsd:annotation" mode="porn">
    <xsl:text>&#0009;</xsl:text>
    <xsl:value-of
      select="xsd:appinfo/mddl:schema-classification/@type"/>
    <xsl:text>&#0009;</xsl:text>
    <xsl:value-of select="xsd:appinfo/mddl:schema-classification"/>
    <xsl:text>&#0010;</xsl:text>
  </xsl:template>

  <xsl:template match="xsd:complexType" mode="porn">
    <xsl:value-of select="name()"/>
    <xsl:text>&#0009;</xsl:text>
    <xsl:value-of select="@name"/>
    <xsl:apply-templates mode="porn"/>
    <xsl:text>&#0010;</xsl:text>
  </xsl:template>

  <xsl:template match="xsd:sequence" mode="porn">
    <xsl:param name="indent"/>
    <xsl:apply-templates mode="porn">
      <xsl:with-param name="indent" select="concat('&#0009;', $indent)"/>
    </xsl:apply-templates>
  </xsl:template>

  <xsl:template match="xsd:element[@ref]" mode="porn">
    <xsl:param name="indent"/>
    <xsl:value-of select="$indent"/>
    <xsl:value-of select="@ref"/>
    <xsl:text>&#0010;</xsl:text>
  </xsl:template>

  <xsl:template match="xsd:group[@name]" mode="porn">
    <xsl:param name="indent"/>
    <xsl:apply-templates mode="porn">
      <xsl:with-param name="indent" select="$indent"/>
    </xsl:apply-templates>
  </xsl:template>

  <xsl:template match="xsd:group[@ref]" mode="porn">
    <xsl:param name="indent"/>
    <xsl:variable name="ref" select="@ref"/>
    <xsl:variable name="ref_nons" select="substring($ref, 6)"/>

    <xsl:value-of select="$indent"/>
    <xsl:value-of select="$ref"/>
    <xsl:text> {&#0010;</xsl:text>

    <xsl:apply-templates
      select="/xsd:schema/xsd:group[@name=$ref_nons]" mode="porn">
      <xsl:with-param name="indent" select="concat('&#0009;', $indent)"/>
    </xsl:apply-templates>

    <xsl:value-of select="$indent"/>
    <xsl:text>}&#0010;</xsl:text>
  </xsl:template>

  <xsl:template match="xsd:choice" mode="porn">
    <xsl:param name="indent"/>
    <xsl:value-of select="$indent"/>
    <xsl:text>union {&#0010;</xsl:text>
    <xsl:apply-templates mode="porn">
      <xsl:with-param name="indent" select="concat('&#0009;', $indent)"/>
    </xsl:apply-templates>
    <xsl:value-of select="$indent"/>
    <xsl:text>}</xsl:text>
    <xsl:text>&#0010;</xsl:text>
  </xsl:template>

  <xsl:template match="xsd:attribute" mode="porn">
    <xsl:text>&#0009;ATTR</xsl:text>
    <xsl:text>&#0009;</xsl:text>
    <xsl:value-of select="@name"/>
    <xsl:text>&#0009;</xsl:text>
    <xsl:value-of select="@type"/>
    <xsl:if test="@use">
      <xsl:text>&#0009;USE:</xsl:text>
      <xsl:value-of select="@use"/>
    </xsl:if>
    <xsl:if test="@default">
      <xsl:text>&#0009;DEFAULT:</xsl:text>
      <xsl:value-of select="@default"/>
    </xsl:if>
    <xsl:text>&#0010;</xsl:text>
  </xsl:template>

  <!-- catch all -->
  <xsl:template match="text()"/>
  <xsl:template match="text()" mode="porn"/>
  <xsl:template match="xsd:*"/>
  <xsl:template match="xsd:*" mode="porn"/>

</xsl:stylesheet>
