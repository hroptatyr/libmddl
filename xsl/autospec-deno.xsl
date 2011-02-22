<?xml version="1.0" encoding="utf-8"?>
<xsl:stylesheet
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  xmlns:UML="omg.org/UML1.3"
  version="1.0">

  <xsl:output method="text"/>

  <xsl:template match="*[@name]">
    <xsl:call-template name="make_name">
      <xsl:with-param name="node" select="."/>
    </xsl:call-template>
    <xsl:text>{&#0010;</xsl:text>

    <xsl:call-template name="make_value">
      <xsl:with-param name="type" select="@type"/>
    </xsl:call-template>

    <xsl:text>&#0010;</xsl:text>
    <xsl:apply-templates mode="hit"/>
    <xsl:text>}&#0010;&#0010;</xsl:text>
  </xsl:template>

  <!-- special treatment -->
  <xsl:template match="group[@name]">
    <xsl:call-template name="make_name">
      <xsl:with-param name="node" select="."/>
    </xsl:call-template>
    <xsl:text>{&#0010;</xsl:text>

    <xsl:call-template name="make_value">
      <xsl:with-param name="type" select="@type"/>
    </xsl:call-template>

    <xsl:text>&#0010;</xsl:text>
    <xsl:apply-templates mode="hit"/>
    <xsl:text>}&#0010;&#0010;</xsl:text>
  </xsl:template>

  <xsl:template match="*[@ref]" mode="hit">
    <xsl:text>  </xsl:text>
    <xsl:value-of select="name()"/>
    <xsl:text>:</xsl:text>
    <xsl:value-of select="@ref"/>
    <xsl:text>&#0010;</xsl:text>
  </xsl:template>

  <xsl:template match="*[@name]" mode="hit">
    <xsl:text>  </xsl:text>
    <xsl:value-of select="name()"/>
    <xsl:text>:</xsl:text>
    <xsl:value-of select="@name"/>
    <xsl:text>&#0010;</xsl:text>
  </xsl:template>

  <xsl:template match="denormalized-data" mode="hit"/>


  <xsl:template name="make_name">
    <xsl:param name="node"/>
    <xsl:choose>
      <xsl:when test="name($node) = 'element'">
        <xsl:text>struct __e_</xsl:text>
      </xsl:when>
      <xsl:when test="name($node) = 'domain'">
        <xsl:text>struct __d_</xsl:text>
      </xsl:when>
      <xsl:when test="name($node) = 'class'">
        <xsl:text>struct __c_</xsl:text>
      </xsl:when>
      <xsl:when test="name($node) = 'subclass'">
        <xsl:text>struct __c_</xsl:text>
      </xsl:when>
      <xsl:when test="name($node) = 'group'">
        <xsl:text>struct __g_</xsl:text>
      </xsl:when>
      <xsl:when test="name($node) = 'property'">
        <xsl:text>struct __p_</xsl:text>
      </xsl:when>
      <xsl:otherwise>
        <xsl:text>struct __</xsl:text>
        <xsl:value-of select="name($node)"/>
        <xsl:text>_</xsl:text>
      </xsl:otherwise>
    </xsl:choose>
    <xsl:value-of select="@name"/>
    <xsl:text>_s</xsl:text>
  </xsl:template>

  <xsl:template name="make_value">
    <xsl:param name="type"/>

    <xsl:if test="$type">
      <xsl:text>&#0009;</xsl:text>
      <xsl:text>md_</xsl:text>
      <xsl:value-of select="$type"/>
      <xsl:text>_t value;&#0010;</xsl:text>
    </xsl:if>
  </xsl:template>


  <!-- catch all -->
  <xsl:template match="text()"/>
  <xsl:template match="text()" mode="hit"/>

</xsl:stylesheet>
